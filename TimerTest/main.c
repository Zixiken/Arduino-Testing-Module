/*
 * TimerTest.c
 *
 * Created: 1/23/2017 4:13:31 PM
 * Author : zangamj
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <util/delay.h>

#define HIGH 1
#define LOW 0

typedef struct {
	unsigned char stateChange; //The detected state
	unsigned char pinNo; //The pin the change occurred on (always 21 in this case)
	unsigned long absTime; //Absolute time since the capture was started
} timestamp;

volatile timestamp timestamps[9]; //Only capturing 8 timestamps for this test
volatile unsigned char tsIndex = 0; //index for the timestamp array

//Sends a timestamp on the serial port as a char array
void send(volatile timestamp * t) {
	char * c = (char *)t;
	for(char i = 0; i < sizeof(timestamp); i++) {
		while(!(UCSR0A & 0x20));
		UDR0 = *c;
		c++;
	}
}

//Function to write the contents of the timestamp array to the serial port.
void sendTimestamps(void) {
	for(char i = 0; i < 8; i++) send(timestamps+i);
}

//Setup I/O registers, and then flash PB6 with 1.25s period, duty cycle 20%
int main(void) {
	UCSR0A = 0x2; //U2X enable
	UBRR0 = 16; //Set baud rate to 115.2K
	UCSR0B = 0x18; //Enable transmitter and receiver

	DDRB |= 0x80; //Set port B pin 6 as output
	DDRH = 0x8; //Set porg H bit 3 (OC4A) for output

	TCCR4A = 0xC2; //Set OC4A to set on match, and half of fast PWM mode
	TCCR4B = 0x18; //Other half of fast PWM mode
	OCR4A = 0x7FFF; //Match on half max timer value
	ICR4 = 0xFFFF; //Timer's top is max value

	EICRA = 1; //Any edge on INT0 pin (21) triggers interrupt.
	EIMSK |= 1; //Enable INT0

	sei(); //Enable global interrupts.

	TCCR5B = 0x6; //Clock on falling edge
	TCCR4B |= 1; //Set clock source to no prescaling

	while(1) {
		PORTB |= 0x80;
		_delay_ms(250);
		PORTB &= 0x7F;
		_delay_ms(1000);

		if(tsIndex >= 8) {
			EIMSK &= 0xFE; //Apparently this interrupt disable isn't working.
			tsIndex = 0;
			sendTimestamps();
		}
	}
}

/*
 * External interrupt 0 vector. Sets fields of the current timestamp,
 * and increments the index value. The absolute time is created with the number
 * of overflows as the high order bytes, and the current timer value as the lower.
 */
ISR(INT0_vect) {
	unsigned int timerValue4 = TCNT4, timerValue5 = TCNT5;
	timestamps[tsIndex].absTime = ((unsigned long)(timerValue5) << 16) + timerValue4;
	timestamps[tsIndex].stateChange = PIND & 1 ? HIGH : LOW;
	timestamps[tsIndex].pinNo = 21;
	tsIndex++;
}
