/*
 * TimerTest.c
 *
 * Created: 1/23/2017 4:13:31 PM
 * Author : zangamj
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define ARRAY_SIZE 64

typedef struct {
	unsigned char stateChange; //The detected state
	unsigned long absTime; //Absolute time since the capture was started
	unsigned char pinNo; //The pin the change occurred on
} timestamp;

//Timestamp arrays
volatile timestamp int2Timestamps[ARRAY_SIZE];
volatile timestamp int3Timestamps[ARRAY_SIZE];
volatile timestamp int4Timestamps[ARRAY_SIZE];
volatile timestamp int5Timestamps[ARRAY_SIZE];
//Next write index for the timestamp arrays
volatile unsigned char int2Write = 0;
volatile unsigned char int3Write = 0;
volatile unsigned char int4Write = 0;
volatile unsigned char int5Write = 0;
//Next read index for the timestamp arrays
unsigned char int2Read = 0;
unsigned char int3Read = 0;
unsigned char int4Read = 0;
unsigned char int5Read = 0;

//Sends a timestamp on the serial port as a char array
void send(volatile timestamp * t) {
	char * c = (char *)t;
	for(char i = 0; i < sizeof(timestamp); i++) {
		while(!(UCSR0A & 0x20));
		UDR0 = *c;
		c++;
	}
}

//Setup I/O registers, interrupts, and timers, set pin numbers in timestamp arrays,
//and then flash PB6 with 1.25s period, duty cycle 20%
int main(void) {
	unsigned int i;
	for(i = 0; i < ARRAY_SIZE; i++) int2Timestamps[i].pinNo = 19;
	for(i = 0; i < ARRAY_SIZE; i++) int3Timestamps[i].pinNo = 18;
	for(i = 0; i < ARRAY_SIZE; i++) int4Timestamps[i].pinNo = 2;
	for(i = 0; i < ARRAY_SIZE; i++) int5Timestamps[i].pinNo = 3;
	
	UCSR0A = 0x2; //U2X enable
	UBRR0 = 1; //Set baud rate to 1M
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
	EICRA = 0x10;
	EIMSK |= 0x04;
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
		
		while(int2Read != int2Write) {
			send(int2Timestamps+int2Read);
			int2Read = (int2Read+1) % ARRAY_SIZE;
		}
		while(int3Read != int3Write) {
			send(int3Timestamps+int3Read);
			int3Read = (int3Read+1) % ARRAY_SIZE;
		}
		while(int4Read != int4Write) {
			send(int4Timestamps+int4Read);
			int4Read = (int4Read+1) % ARRAY_SIZE;
		}
		while(int5Read != int5Write) {
			send(int5Timestamps+int5Read);
			int5Read = (int5Read+1) % ARRAY_SIZE;
		}
	}
}
