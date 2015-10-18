/*
 * TINY13A_TeaTimer.c
 *
 * Created: 2015-05-01 16:58:33
 *  Author: Mruczek
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define LED1 (1<<PB0)
#define LED2 (1<<PB1)
#define LED3 (1<<PB2)
#define BUTTON1 (1<<PB4)
#define BUZZER (1<<PB3)

const uint8_t teatime= 180;
uint8_t timer = 0;

int vTmCount = 0;

int main(void)
{
    DDRB |= (LED1 | LED2 | LED3);			// LEDY to wyjœcia
	DDRB &= ~(BUTTON1);						// Button to wejœcie
	DDRB |= BUZZER;							// Buzzer to te¿ wyjœcie
	
	PORTB = 0b00000111;						// Gasimy diody
	PORTB |= BUTTON1;						// podci¹gniêcie do VCC
	PORTB &= ~BUZZER;						// g³oœnik w tej pozycji nie pobiera tony pr¹du (0 jest jego stanem spoczynku, gdy sterujemy przez Mosfet-N)

	TCCR0A |= (1<<WGM01);					// set CTC mode	
	TCCR0B |= (1<<CS01);					// prescaller = 8
	OCR0A  = 149;							// licznik dla timera0
	TIMSK0  |= (1<<OCIE0A);
	
	while(1)
    {
		while(!timer) {
			if (!(PINB & BUTTON1)) {
				_delay_ms(30);
				if (!(PINB & BUTTON1)) {
					timer = teatime;
				}
			}
		}

		sei();
		while (timer) {
			if (timer >= 120) PORTB &= ~(LED1 | LED2 | LED3);
			if (timer < 120 && timer >= 60 ) PORTB |= (LED3);
			if (timer < 60 && timer > 0) PORTB |= (LED2);
		}
		PORTB |= (LED1);
		cli();
		
		while(PINB & BUTTON1) {
			PORTB |= BUZZER;
			_delay_us(400);
			PORTB &= ~BUZZER;
			_delay_us(400);
		}
		_delay_ms(200);
	}
}

ISR( TIM0_COMPA_vect ) {
	if (vTmCount++ >= 1000) {
		vTmCount = 0;
		timer--;	
	}
}