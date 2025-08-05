#ifndef switches_included
#define switches_included

#define SW1 BIT0		/* switch1 is p2.0 */
#define SW2 BIT1		/* switch2 is p2.1 */
#define SW3 BIT2		/* switch3 is p2.2 */
#define SW4 BIT3		/* switch4 is p2.3 */
#define SWITCHES (SW1 | SW2 | SW3 | SW4)		/* only 1 switch on this board */

void switch_init();
void switch_interrupt_handler();

extern char sw1_state_down, sw2_state_down, sw3_state_down, sw4_state_down, switch_state_changed; /* effectively boolean */

#endif // included
