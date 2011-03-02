#include <avr/io.h>
#include "led.h"

void
led_init(void) {
	DDRB=0xFF;
}

void
led_control(void) {
	PORTB |=(1<<PB3);
}

void
led_red(void) {
	PORTB |=(1<<PB2);
}

void
led_orange(void) {
	PORTB |=(1<<PB1);
}

void
led_green(void) {
	PORTB |=(1<<PB0);
}

void
summer(void) {
	PORTB=32;
}

void
led_off(void) {
	PORTB=0;
}
