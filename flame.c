#include <avr/io.h>
#include <avr/interrupt.h>

#include <util/delay.h>

#include <stdlib.h>

static const int port[] = { PB0, PB1, PB3, PB4 };
volatile uint8_t* pwm[] = { &OCR0A, &OCR0B, &OCR1A, &OCR1B };
static const int ports = 4;

ISR(TIMER1_COMPA_vect) {
	if ((TIFR & _BV(TOV1)) == 0)
		PORTB |= _BV(port[2]);
}

ISR(TIMER1_OVF_vect) {
	PORTB &= ~_BV(port[2]);
}

int main() {
	int ch[ports];
	int i;

	cli();

	// Decode in a sec.
	  // Configure counter/timer0 for fast PWM on PB0 and PB1
	TCCR0A = 3<<COM0A0 | 3<<COM0B0 | 3<<WGM00;
	TCCR0B = 0<<WGM02 | 3<<CS00; // Optional; already set
	// Configure counter/timer1 for fast PWM on PB4
	TCCR1 = 1<<CTC1 | 1<<PWM1A | 3<<COM1A0 | 7<<CS10;
	GTCCR = 1<<PWM1B | 3<<COM1B0;
	// Interrupts on OC1A match and overflow
	TIMSK = TIMSK | 1<<OCIE1A | 1<<TOIE1;

	for (i = 0; i < ports; ++i) {
		DDRB |= _BV(port[i]);
		// PORTB |= _BV(port[i]);
		ch[i] = 5 + random() % 10;
	}

	sei();

	for (;;) {
		for (i = 0; i < ports; ++i) {
			if (!--ch[i]) {
				*pwm[i] = 135 + random() % 120;
				ch[i] = 5 + random() % 10;
			}
		}

		_delay_ms(10);
	}	

	return 0;
}
