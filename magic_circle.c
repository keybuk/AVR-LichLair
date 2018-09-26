#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/delay.h>

#include <stdlib.h>

static const int port[] = { PORTB0, PORTB1, PORTB3, PORTB4 };
volatile uint8_t* pwm[] = { &OCR0A, &OCR0B, &OCR1A, &OCR1B };

#define ports 4

ISR(TIMER1_COMPA_vect) {
	if (!bit_is_set(TIFR, TOV1))
		PORTB |= _BV(port[2]);
}

ISR(TIMER1_OVF_vect) {
	PORTB &= ~_BV(port[2]);
}

int main() {
	int pattern = 0;
	int phase = 0;

	cli();

	// Configure Timer/Counter0 for fast PWM on PB0 and PB1.
	// Use inverted (set/clear) mode for consistency.
	TCCR0A = (_BV(COM0A1) | _BV(COM0A0) |
		      _BV(COM0B1) | _BV(COM0B0) |
		      _BV(WGM01) | _BV(WGM00));
	TCCR0B = _BV(CS01) | _BV(CS00);

	// Configure Timer/Counter1 for PWM on PB4.
	// Match the PCK/64 prescale of Timer/Counter0 and use set/clear
	// mode for consistency.
	TCCR1 = (_BV(CTC1) | _BV(PWM1A) |
		     _BV(COM1A1) | _BV(COM1A0) |
		     _BV(CS12) | _BV(CS11) | _BV(CS10));
	GTCCR = _BV(PWM1B) | _BV(COM1B1) | _BV(COM1B0);

	// We can't connect the OCR1A PWM output to PB3 in hardware; enable
	// timer interrupts on match and overflow to do it in software.
	TIMSK |= _BV(OCIE1A) | _BV(TOIE1);

	// Enable four ports for output and one for input.
	DDRB |= _BV(DDB0) | _BV(DDB1) | _BV(DDB3) | _BV(DDB4);
	DDRB &= ~_BV(DDB2);

	sei();

	for (;;) {
		switch (pattern) {
		case 0:
			for (int i = 0; i < ports; ++i) {
				*pwm[i] = abs(255 - phase);
			}

			phase += 16;
			phase %= 512;
			break;

		case 1:
			*pwm[0] = abs(255 - phase);
			*pwm[1] = abs(255 - (phase + 128) % 512);
			*pwm[2] = abs(255 - (phase + 256) % 512);
			*pwm[3] = abs(255 - (phase + 384) % 512);

			phase += 16;
			phase %= 512;
			break;
		case 2:
			*pwm[phase/3] = 0;
			*pwm[(phase/3 + 1) % ports] = 200;
			*pwm[(phase/3 + 2) % ports] = 224;
			*pwm[(phase/3 + 3) % ports] = 224;

			phase++;
			phase %= ports * 3;
			break;
		}

		// Switch pattern on the button push.
		if ((PINB & _BV(PINB2)) == 0) {
			for (int i = 0; i < ports; ++i) {
				*pwm[i] = 255;
			}

			while ((PINB & _BV(PINB2)) == 0)
				;

			pattern++;
			pattern %= 3;

			phase = 0;
		}

		_delay_ms(41);
	}

	return 0;
}
