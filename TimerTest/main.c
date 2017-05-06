/*
 * TimerTest.c
 *
 * Created: 1/23/2017 4:13:31 PM
 * Author : zangamj
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/twi.h>

#define ARRAY_SIZE 256

typedef struct {
	unsigned char stateChange; //The detected state
	unsigned long absTime; //Absolute time since the capture was started
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
//Send pin number and detected state in the same byte
void send(volatile timestamp * t, char pin) {
	char * c = (char *)t;
	while(!(UCSR0A & 0x20));
	UDR0 = *c | pin;
	c++;
	for(char i = 1; i < sizeof(timestamp); i++) {
		while(!(UCSR0A & 0x20));
		UDR0 = *c;
		c++;
	}
}

//Setup I/O registers, interrupts, and timers, set pin numbers in timestamp arrays,
//and then toggle pin 5 0.5s period 50% duty cycle.
int main(void) {
	UCSR0A = 0x2; //U2X enable
	UBRR0 = 1; //Set baud rate to 1M
	UCSR0B = 0x98; //Enable transmitter and receiver

	DDRB |= 0xC0; //Set port B pin 6 as output
	DDRH |= 0x8; //Set port H bit 3 (OC4A) for output

	TCCR4A = 0xC2; //Set OC4A to set on match, and half of fast PWM mode
	TCCR4B = 0x18; //Other half of fast PWM mode
	OCR4A = 0x7FFF; //Match on half max timer value
	ICR4 = 0xFFFF; //Timer's top is max value
	
	//Generate square wave that isn't affected by interrupts
	DDRE = 8; //Arduino pin 5, timer3 oca
	PORTE = 8; //Turn on initially
	TCCR3A = 0x82; //Fast PWM mode, clear oca on match
	TCCR3B = 0x18;
	//ICR3 = 31250; //500ms period
	//OCR3A = 15625; //250ms match time
	//TCCR3B |= 4; //256 prescalar
	ICR3 = 159;
	OCR3A = 79;
	TCCR3B |= 1;

	sei(); //Enable global interrupts.

	TCCR5B = 0x6; //Clock on falling edge
	TCCR4B |= 1; //Set clock source to no prescaling
	
	while(1) {
		//pin values 0, 1, 2, 3 are interrupts 2, 3, 4, 5
		while(int2Read != int2Write) {
			send(int2Timestamps+int2Read, 0);
			int2Read = (int2Read+1) % ARRAY_SIZE;
		}
		while(int3Read != int3Write) {
			send(int3Timestamps+int3Read, 1);
			int3Read = (int3Read+1) % ARRAY_SIZE;
		}
		while(int4Read != int4Write) {
			send(int4Timestamps+int4Read, 2);
			int4Read = (int4Read+1) % ARRAY_SIZE;
		}
		while(int5Read != int5Write) {
			send(int5Timestamps+int5Read, 3);
			int5Read = (int5Read+1) % ARRAY_SIZE;
		}
	}
}

ISR(USART0_RX_vect) {
	unsigned char in = UDR0;
	if(in & 0xC0) EICRA = in;
	else if(in & 0x03) EICRB = in;
	else EIMSK = in;
}