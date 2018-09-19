#include <avr/io.h>
#include <util/delay.h>

int main() {
	DDRB |= (1 << PB1) | (1 << PB3);
	PORTB |= (1 << PB3);

	for (;;) {
		PORTB ^= (1 << PB1);
		_delay_ms(1000);
	}	

	return 0;
}
