#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/delay.h>

#include <stdlib.h>

static const int port[] = { PORTB0, PORTB1, PORTB3, PORTB4 };
volatile uint8_t* pwm[] = { &OCR0A, &OCR0B, &OCR1A, &OCR1B };

#define ports 4
int ch[ports];

ISR(TIMER1_COMPA_vect) {
	if (!bit_is_set(TIFR, TOV1))
		PORTB |= _BV(port[2]);
}

ISR(TIMER1_OVF_vect) {
	PORTB &= ~_BV(port[2]);
}

int main() {
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

	// Enable all four ports for output.
	DDRB |= _BV(DDB0) | _BV(DDB1) | _BV(DDB3) | _BV(DDB4);

	sei();

	for (;;) {
		for (int i = 0; i < ports; ++i) {
			if (!ch[i]--) {
				*pwm[i] = random() % 235;
				ch[i] = 5 + random() % 10;
			}
		}

		_delay_ms(10);
	}	

	return 0;
}
