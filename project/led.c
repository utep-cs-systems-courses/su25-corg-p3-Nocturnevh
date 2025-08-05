#include <msp430.h>
#include "led.h"
#include "switches.h"

void led_init()
{
    P1DIR |= LEDS;
    P1OUT &= ~LED_GREEN;
    P1OUT |= LED_RED;
}

void led_off(){
    P1OUT &= ~LEDS;
}

int secondCount = 0;
void alternate_leds(){
    secondCount ++;
    if (secondCount >= 250) { 	/* once each sec... */
      secondCount = 0;		    /* reset count */
      P1OUT ^= LEDS;            /* toggle LEDS */
    }
}

int sCount = 0;
int blinkLimit = 5;
int blinkCount = 0;

void blink_leds(){
  blinkCount ++;
  if (blinkCount >= blinkLimit) {
    blinkCount = 0;
    P1OUT |= LED_RED ;
    P1OUT &= ~LED_GREEN;
  }
  else{
    P1OUT &= ~LED_RED;
    P1OUT |= LED_GREEN;
  }

  sCount ++;
  if (sCount >= 250) {
    sCount = 0;
    blinkLimit ++;
    if (blinkLimit >= 8)
      blinkLimit = 0;
  }
} 
