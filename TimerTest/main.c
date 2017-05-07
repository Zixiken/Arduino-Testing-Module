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

typedef struct {
	unsigned char stateChange; //The detected state
	unsigned long absTime; //Absolute time since the capture was started
} timestamp;

//Timestamp arrays
volatile timestamp int2Timestamps[256];
volatile timestamp int3Timestamps[256];
volatile timestamp int4Timestamps[256];
volatile timestamp int5Timestamps[256];
//Next write index for the timestamp arrays
volatile unsigned char int2Write = 0;
volatile unsigned char int3Write = 0;
volatile unsigned char int4Write = 0;
volatile unsigned char int5Write = 0;
//Next read index for the timestamp arrays
volatile unsigned char int2Read = 0;
volatile unsigned char int3Read = 0;
volatile unsigned char int4Read = 0;
volatile unsigned char int5Read = 0;
//Counters for number of captures sent on each pin
unsigned char int2Count = 0, int2NumCounts = 0;
unsigned char int3Count = 0, int3NumCounts = 0;
unsigned char int4Count = 0, int4NumCounts = 0;
unsigned char int5Count = 0, int5NumCounts = 0;

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

void int2Limited() {
	if(int2Count < int2NumCounts && int2Read != int2Write) {
		send(int2Timestamps+int2Read, 0);
		int2Count++;
		int2Read++;
	}
}

void int2Unlimited() {
	while(int2Read != int2Write) {
		send(int2Timestamps+int2Read, 0);
		int2Read++;
	}
}

void int3Limited() {
	if(int3Count < int3NumCounts && int3Read != int3Write) {
		send(int3Timestamps+int3Read, 1);
		int3Count++;
		int3Read++;
	}
}

void int3Unlimited() {
	while(int3Read != int3Write) {
		send(int3Timestamps+int3Read, 0);
		int3Read++;
	}
}

void int4Limited() {
	if(int4Count < int4NumCounts && int4Read != int4Write) {
		send(int4Timestamps+int4Read, 2);
		int4Count++;
		int4Read++;
	}
}

void int4Unlimited() {
	while(int4Read != int4Write) {
		send(int4Timestamps+int4Read, 0);
		int4Read++;
	}
}

void int5Limited() {
	if(int5Count < int5NumCounts && int5Read != int5Write) {
		send(int5Timestamps+int5Read, 3);
		int5Count++;
		int5Read++;
	}
}

void int5Unlimited() {
	while(int5Read != int5Write) {
		send(int5Timestamps+int5Read, 0);
		int5Read++;
	}
}

//Setup I/O registers, interrupts, and timers, set pin numbers in timestamp arrays,
//and then toggle pin 5 0.5s period 50% duty cycle.
int main(void) {
	void (* int2ReadFunc)(void) = int2Unlimited;
	void (* int3ReadFunc)(void) = int3Unlimited;
	void (* int4ReadFunc)(void) = int4Unlimited;
	void (* int5ReadFunc)(void) = int5Unlimited;
	
	UCSR0A = 0x2; //U2X enable
	UBRR0 = 1; //Set baud rate to 1M
	UCSR0B = 0x18; //Enable transmitter and receiver

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
		if(UCSR0A & 0x80) {
			unsigned char in = UDR0;
			while(!(UCSR0A & 0x80));
			if(in == 0) EICRA = UDR0;
			else if(in == 1) EICRB = UDR0;
			else if(in == 2) EIMSK = UDR0;
			else if(in == 3) {
				int2NumCounts = UDR0;
				int2Count = int2Read = int2Write = 0;
				if(int2NumCounts == 0) int2ReadFunc = int2Unlimited;
				else int2ReadFunc = int2Limited;
			} else if(in == 4) {
				int3NumCounts = UDR0;
				int3Count = int3Read = int3Write = 0;
				if(int3NumCounts == 0) int3ReadFunc = int3Unlimited;
				else int3ReadFunc = int3Limited;
			} else if(in == 5) {
				int4NumCounts = UDR0;
				int4Count = int4Read = int4Write = 0;
				if(int4NumCounts == 0) int4ReadFunc = int4Unlimited;
				else int4ReadFunc = int4Limited;
			} else {
				int5NumCounts = UDR0;
				int5Count = int5Read = int5Write = 0;
				if(int5NumCounts == 0) int5ReadFunc = int5Unlimited;
				else int5ReadFunc = int5Limited;
			}
		}

		//pin values 0, 1, 2, 3 are interrupts 2, 3, 4, 5
		int2ReadFunc();
		int3ReadFunc();
		int4ReadFunc();
		int5ReadFunc();
	}
}