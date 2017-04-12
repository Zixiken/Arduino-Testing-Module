/*
 * TimerTest.c
 *
 * Created: 1/23/2017 4:13:31 PM
 * Author : zangamj
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

typedef struct {
	unsigned char stateChange; //The detected state
	unsigned char pinNo; //The pin the change occurred on
	unsigned long absTime; //Absolute time since the capture was started
} timestamp;

volatile timestamp timestamps[9];
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

	DDRB |= 0x40; //Set port B pin 6 as output
	DDRH |= 0x8; //Set port H bit 3 (OC4A) for output

	TCCR4A = 0xC2; //Set OC4A to set on match, and half of fast PWM mode
	TCCR4B = 0x18; //Other half of fast PWM mode
	OCR4A = 0x7FFF; //Match on half max timer value
	ICR4 = 0xFFFF; //Timer's top is max value

	/* Uncommenting this block enables all 4 external interrupts
	EICRA = 0x50; //Any edge on INT2 & INT3 pins (19 & 18) triggers interrupt.
	EICRB = 0x5; //Any edge on INT4 & INT5 pins (2 & 3) triggers interrupt.
	EIMSK |= 0x3C; //Enable INT2-5*/
	EICRB = 4;
	EIMSK |= 0x20;
	EIFR = 0;

	sei(); //Enable global interrupts.

	TCCR5B = 0x6; //Clock on falling edge
	TCCR4B |= 1; //Set clock source to no prescaling
	
	_delay_ms(32); //This removed the short time difference for the first interrupts
	
	while(1) {
		PORTB |= 0x40;
		_delay_ms(1);
		PORTB &= 0xBF;
		_delay_ms(1000);

		if(tsIndex >= 8) {
			EIMSK &= 0xC3;
			tsIndex = 0;
			sendTimestamps();
		}
	}
}
