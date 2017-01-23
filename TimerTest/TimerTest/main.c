/*
 * TimerTest.c
 *
 * Created: 1/23/2017 4:13:31 PM
 * Author : zangamj
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

unsigned int overflows = 0, prevOverflows = 0, prevTimerValue = 0;

int main(void) {
	UCSR0A = 0x2; //U2X enable
	UBRR0 = 16; //Set baud rate to 115.2K
	UCSR0B = 0x18; //Enable transmitter and receiver
	
	PORTL = 1; //Enable PL0's pull up resistor

	TIMSK4 = 0x21; //Enable input capture and overflow interrupts.
	//Timers 4 and 5 are the only ones for which the input capture pin maps to a pin on the arduino boards.
	sei();
	TCCR4B = 1; //Set clock source to no prescaling
	
	while(1);
}

void send(char * line) {
	while(*line != '\0') {
		while(!(UCSR0A & 0x20));
		UDR0 = *line;
		line++;
	}
}

ISR(TIMER4_OVF_vect) {overflows++;}

ISR(TIMER4_CAPT_vect) {
	unsigned int timerValue = ICR4;
	unsigned int diffTimerValue = timerValue-prevTimerValue;
	unsigned int diffOverflows = overflows-prevOverflows;
	double seconds = (double)(diffOverflows*65536 + diffTimerValue) / F_CPU;
	char buf[40];
	sprintf(buf, "%f seconds\n", seconds);
	prevOverflows = overflows;
	prevTimerValue = timerValue;
	send(buf);
}



