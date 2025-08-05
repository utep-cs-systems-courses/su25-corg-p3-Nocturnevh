#include <msp430.h>
#include "libTimer.h"
#include "buzzer.h"

int melody[] = { /*  MMPR Morpher Melody: E6, E6, D6, E6, G6, E6, Rest */
  NOTE_E6, NOTE_E6, NOTE_D6,
  NOTE_E6, NOTE_G6, NOTE_E6,
  NOTE_REST
};

int durations[] = {
  1, 1, 2,
  3, 1, 1,
  4
};

void buzzer_init()
{
    /* 
       Direct timer A output "TA0.1" to P2.6.  
        According to table 21 from data sheet:
          P2SEL2.6, P2SEL2.7, anmd P2SEL.7 must be zero
          P2SEL.6 must be 1
        Also: P2.6 direction must be output
    */
    timerAUpmode();		/* used to drive speaker */
    P2SEL2 &= ~(BIT6 | BIT7);
    P2SEL &= ~BIT7; 
    P2SEL |= BIT6;
    P2DIR = BIT6;		/* enable output to speaker (P2.6) */
}

void buzzer_set_period(short cycles) /* buzzer clock = 2MHz.  (period of 1k results in 2kHz tone) */
{
  CCR0 = cycles; 
  CCR1 = cycles >> 1;		/* one half cycle */
}

void buzzer_off() {
    buzzer_set_period(0);
}

void delay_ms(int ms) {
  while (ms--) {
    __delay_cycles(2000);  /* 1 ms delay @ 2 MHz */
  }
}


void play_song() {
  int notes = sizeof(melody) / sizeof(melody[0]);

  for (int i = 0; i < notes; i++) {
    int note = melody[i];
    int duration = 750 / durations[i];  /* ms */

    if (note == NOTE_REST) {
      buzzer_set_period(0);
    } else {
      buzzer_set_period(note);
    }

    delay_ms(duration);
    buzzer_set_period(0);   /* pause between notes */
    delay_ms(1500);
  }
}
