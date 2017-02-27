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

volatile unsigned int overflows = 0;  //The number of overflows of timer 4 that have occurred.
volatile timestamp timestamps[9]; //Only capturing 8 timestamps for this test
volatile unsigned char i = 0; //index for the timestamp array

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

	TIMSK4 = 1; //Enable overflow interrupt.

	EICRA = 1; //Any edge on INT0 pin (21) triggers interrupt.
	
	sei(); //Enable global interrupts.

	TCCR4B = 1; //Set clock source to no prescaling

	while(1) {
		EIMSK |= 1; //Enable INT0. Having this down here removed the extra capture
		
		PORTB |= 0x80;
		_delay_ms(250);
		PORTB &= 0x7F;
		_delay_ms(1000);
		
		if(i == 8) {
			EIMSK &= 0xFE; //Apparently this interrupt disable isn't working.
			i = 0;
			sendTimestamps();
		}
	}
}

//Increment the overflows value when they occur.
ISR(TIMER4_OVF_vect) {overflows++;}

/*
 * External interrupt 0 vector. Sets fields of the current timestamp,
 * and increments the index value. The absolute time is created with the number
 * of overflows as the high order bytes, and the current timer value as the lower.
 */
ISR(INT0_vect) {
	unsigned int timerValue = TCNT4;
	timestamps[i].absTime = ((unsigned long)(overflows) << 16) + timerValue;
	timestamps[i].stateChange = PIND & 1 ? HIGH : LOW;
	timestamps[i].pinNo = 21;
	i++;
}
