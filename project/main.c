#include <msp430.h>
#include <libTimer.h>
#include "lcdutils.h"
#include "lcddraw.h"
#include "buzzer.h"
#include "switches.h"

#define LED BIT6

typedef enum {
    STATE_DRAWING_STRIPES,
    STATE_OPENING_PRESENT,
    STATE_SHOWING_MESSAGE,
    STATE_PAUSED,
    STATE_SONG,
    STATE_OFF
} DemoState;

DemoState current_state = STATE_DRAWING_STRIPES;

// Global animation variables
static unsigned char row = screenHeight / 2, col = screenWidth / 2;
static char presentHeight = (screenWidth * 3) / 4;
static char currStripeColor = 0;
static char openPresentStep = 0, stripeStep = 0;
static char rowStep = 0;
static short limits[2] = {screenWidth-36, screenHeight-8}; // Add limits array

short redrawScreen = 1;

// Forward declarations for internal functions
static void reset_animation(void);
static void draw_stripes(void);
static void animate_opening(void);
static void show_message(void);
static void demo_state_machine(void);
static void open_top(int leftWallPos[], int rightWallPos[], char presentHeight, char rowStep);
static void write_helloThere(unsigned char col, unsigned char row);

static void reset_animation() {
    currStripeColor = 0;
    openPresentStep = 0;
    stripeStep = 0;
    rowStep = 0;
    clearScreen(COLOR_BLACK);
}

static void draw_stripes() {
    int startingCol = col / 4;
    char color = (currStripeColor) ? COLOR_BROWN : COLOR_YELLOW;
    int leftWallPos[2] = {col/4, row - 25}; 

    if(stripeStep + 16 <= presentHeight){
        fillRectangle(startingCol + stripeStep, row - 25, 16, presentHeight, color);
        stripeStep += 16;
        currStripeColor ^= 1;
    } else {
        fillRectangle(startingCol + (presentHeight - stripeStep), row - 25, 16, presentHeight, color);
        // Draw top
        fillRectangle(leftWallPos[0] - 8, leftWallPos[1] - 16, presentHeight + 16, 16, COLOR_YELLOW);
        current_state = STATE_DRAWING_STRIPES; // Stay in this state until button pressed
    }
}

static void animate_opening() {
    int leftWallPos[2] = {col/4, row - 25}; 
    int rightWallPos[2] = {limits[0] + 16, row - 25};

    // Draw present walls
    fillRectangle(leftWallPos[0], leftWallPos[1], 4, presentHeight, COLOR_BLUE);
    fillRectangle(rightWallPos[0], rightWallPos[1], 4, presentHeight, COLOR_BLUE);

    // Open the inside
    if(openPresentStep <= presentHeight - 4) {
        fillRectangle(leftWallPos[0] + 4, leftWallPos[1], presentHeight - 8, openPresentStep+=3, COLOR_BLACK);
    }

    // Open top
    if(rowStep < 30) {
        open_top(leftWallPos, rightWallPos, presentHeight, rowStep);
        rowStep += 3;
    } else {
        current_state = STATE_SHOWING_MESSAGE;
    }
}

static void show_message() {
    write_helloThere(col, row);
    // Stay in this state until reset
}

static void demo_state_machine() {
    switch (current_state) {
        case STATE_SONG:
	        play_song();
	        break;
        case STATE_DRAWING_STRIPES:
            draw_stripes();
	        buzzer_off();
            break;
        case STATE_OPENING_PRESENT:
            animate_opening();
	    buzzer_off();
            break;
        case STATE_SHOWING_MESSAGE:
            show_message();
            buzzer_off();
            break;
        case STATE_PAUSED:
	        buzzer_off();
	        break;
        case STATE_OFF:
	        clearScreen(COLOR_BLACK);
	        buzzer_off();
	        break;
    }
}

void wdt_c_handler()
{
    static int secCount = 0;
    secCount++;
    if (secCount >= 25) { /* 10/sec */
        secCount = 0;
        redrawScreen = 1;
    }
}

void main()
{
    P1DIR |= LED;   /*Green led on when CPU on */
    P1OUT |= LED;
    configureClocks();
    lcd_init();
    switch_init();
    buzzer_init();

    enableWDTInterrupts();      /*enable periodic interrupt */
    or_sr(0x8);                 /*GIE (enable interrupts) */

    clearScreen(COLOR_BLACK);
    while (1) { /* forever */
        if (redrawScreen) {
            redrawScreen = 0;
            demo_state_machine();
        }
        P1OUT &= ~LED; /* led off */
        or_sr(0x10);   /*CPU OFF */
        P1OUT |= LED;  /* led on */
    }
}

/* Switch interrupt handler */
void
__interrupt_vec(PORT2_VECTOR) Port_2(){
    if (P2IFG & SWITCHES) {         /* did a button cause this interrupt? */
        P2IFG &= ~SWITCHES;         /* clear pending sw interrupts */
        switch_interrupt_handler(); /* single handler for all switches */

        if(sw1_state_down) {
            current_state = STATE_SONG;
        }
        else if(sw2_state_down) {
            if(current_state == STATE_DRAWING_STRIPES) {
                current_state = STATE_OPENING_PRESENT;
            }
        }
        else if(sw3_state_down) {
            current_state = STATE_PAUSED; /*Reset state*/
        }
        else if(sw4_state_down) { /*clear screen and say bye*/
            current_state = STATE_DRAWING_STRIPES;
            reset_animation();
        }
    }
}

void open_top(int leftWallPos[], int rightWallPos[], char presentHeight, char rowStep)
{
    fillRectangle(leftWallPos[0] - 8, leftWallPos[1] - rowStep, presentHeight + 16, 3, COLOR_BLACK);
    fillRectangle(leftWallPos[0] - 8, leftWallPos[1] - 16 - rowStep, presentHeight + 16, 16, COLOR_BROWN);
}

void write_helloThere(unsigned char col, unsigned char row)
{
    drawString5x7(15, row - 50, "Hello There", COLOR_WHITE, COLOR_BLACK);
}

void write_bye(unsigned char col, unsigned char row)
{
    drawString5x7(15, row - 50, "Good Bye", COLOR_WHITE, COLOR_BLACK);
}
