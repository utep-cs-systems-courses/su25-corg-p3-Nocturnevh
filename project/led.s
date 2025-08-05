	.arch msp430g2553
	.p2align 1,0
	.text

	.global led_off
	.extern P1OUT

led_off:
    and.b #~LEDS, P1OUT
    pop r0
    