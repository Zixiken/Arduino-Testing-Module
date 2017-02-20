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

typedef enum {
	HIGH,
	LOW
} state;

typedef struct {
	state stateChange; //The detected state
	unsigned char pinNo; //The pin the change occurred on (always 21 in this case)
	double timePassed; //How much time passed since the previous capture
} timestamp;

volatile unsigned int overflows = 0;  //The number of overflows of timer 4 that have occurred.
volatile unsigned long prevValue = 0; //The previous time a capture occurred at
volatile timestamp timestamps[8]; //Only capturing 8 timestamps for this test
volatile unsigned char i = 0; //index for the timestamp array

//Setup I/O registers, and then flash PB6 with 1.25s period, duty cycle 20%
//Timers' input capture cannot work on both edges, so I switched to using an external interrupt.
int main(void) {
	UCSR0A = 0x2; //U2X enable
	UBRR0 = 16; //Set baud rate to 115.2K
	UCSR0B = 0x18; //Enable transmitter and receiver
	
	DDRB |= 0x40; //Set port B pin 6 as output

	TIMSK4 = 1; //Enable overflow interrupt.
	
	EICRA = 1; //Any edge on INT0 pin (21) triggers interrupt.
	EIMSK = 1; //Enable external interrupt 0
	
	sei(); //Enable global interrupts.
	
	TCCR4B = 1; //Set clock source to no prescaling
	
	while(1) {
		PORTB |= 0x40;
		_delay_ms(250);
		PORTB &= 0xBF;
		_delay_ms(1000);
	}
}

//Very basic serial send function
void send(char * line) {
	while(*line != '\0') {
		while(!(UCSR0A & 0x20));
		UDR0 = *line;
		line++;
	}
}

//Function to write the contents of the timestamp array to the serial port.
//Eventually will have it just send the data instead of parsing a string.
void sendTimestamps(void) {
	char buf[80];
	for(int i = 0; i < 8; i++) {
		sprintf(buf, "%d: change to %d on pin %d, %f seconds since last capture\n",
		i, timestamps[i].stateChange, timestamps[i].pinNo, timestamps[i].timePassed);
		send(buf);
	}
}

//Increment the overflows value whtn they occur.
ISR(TIMER4_OVF_vect) {overflows++;}

/*
 * External interrupt 0 vector. Now (I think) correctly determines the amount of time that passed
 * since the last capture. I changed it to treat the overflows and the timer value together as a
 * 32-bit value, making the time difference easier to calculate. Also will set the other fields of
 * the current timestamp, and if the end of the array has been reached, disables itself and sends
 * the timestamps over the serial port. Otherwise, it increments the index value. That will eventually
 * not happen in this vector, but for now it allows some testing.
 */
ISR(INT0_vect) {
	unsigned int timerValue = TCNT4;
	unsigned long currentValue = ((unsigned long)(overflows) << 16) + timerValue;
	timestamps[i].timePassed = (double)(currentValue-prevValue) / F_CPU;
	prevValue = currentValue;
	timestamps[i].stateChange = PIND & 1 ? HIGH : LOW;
	timestamps[i].pinNo = 21;
	if(i == 7) {
		EIMSK &= 0xFE;
		sendTimestamps();
	} else i++;
}



