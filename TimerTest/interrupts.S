
/*
 * interrupts.S
 * External interrupt vectors. Sets fields of the current timestamp,
 * and increments the index value. The absolute time is created with the number
 * of overflows as the high order bytes, and the current timer value as the lower.
 *
 * Created: 4/6/2017 2:22:16 PM
 *  Author: Michael
 */
 #include <avr/io.h>

.section .text
.global __vector_3
.global __vector_4
.global __vector_5
.global __vector_6

__vector_3:
	push r20
	in r20, 0x09 //in PIND
	push r21
	in r21, 0x3f
	push r21
	andi r20, 0x04 //bit 2
	push r26
	push r27
	lds r26, tsIndex
	mov r21, r26
	add r21, r21
	add r21, r26
	add r21, r21
	inc r26
	sts tsIndex, r26
	ldi r26, lo8(timestamps)
	ldi r27, hi8(timestamps)
	add r26, r21
	adc r27, r1
	st X+, r20
	ldi r21, 19 //pin 19 on arduino board
	st X+, r21
	lds r20, TCNT4L
	lds r21, TCNT4H
	st X+, r20
	st X+, r21
	lds r20, TCNT5L
	lds r21, TCNT5H
	st X+, r20
	st X, r21
	pop r27
	pop r26
	pop r21
	out 0x3f, r21
	pop r21
	pop r20
	reti

__vector_4:
	push r20
	in r20, 0x09 //in PIND
	push r21
	in r21, 0x3f
	push r21
	andi r20, 0x08 //bit 3
	push r26
	push r27
	lds r26, tsIndex
	mov r21, r26
	add r21, r21
	add r21, r26
	add r21, r21
	inc r26
	sts tsIndex, r26
	ldi r26, lo8(timestamps)
	ldi r27, hi8(timestamps)
	add r26, r21
	adc r27, r1
	st X+, r20
	ldi r21, 18 //pin 18 on arduino board
	st X+, r21
	lds r20, TCNT4L
	lds r21, TCNT4H
	st X+, r20
	st X+, r21
	lds r20, TCNT5L
	lds r21, TCNT5H
	st X+, r20
	st X, r21
	pop r27
	pop r26
	pop r21
	out 0x3f, r21
	pop r21
	pop r20
	reti

__vector_5:
	push r20
	in r20, 0x0C //in PINE
	push r21
	in r21, 0x3f
	push r21
	andi r20, 0x10 //bit 4
	push r26
	push r27
	lds r26, tsIndex
	mov r21, r26
	add r21, r21
	add r21, r26
	add r21, r21
	inc r26
	sts tsIndex, r26
	ldi r26, lo8(timestamps)
	ldi r27, hi8(timestamps)
	add r26, r21
	adc r27, r1
	st X+, r20
	ldi r21, 2 //pin 2 on arduino board
	st X+, r21
	lds r20, TCNT4L
	lds r21, TCNT4H
	st X+, r20
	st X+, r21
	lds r20, TCNT5L
	lds r21, TCNT5H
	st X+, r20
	st X, r21
	pop r27
	pop r26
	pop r21
	out 0x3f, r21
	pop r21
	pop r20
	reti

__vector_6: //59 cycles
	push r20
	in r20, 0x0C //in PINE
	push r21
	in r21, 0x3f
	push r21
	andi r20, 0x20 //bit 5
	push r26
	push r27
	lds r26, tsIndex
	mov r21, r26
	add r21, r21
	add r21, r26
	add r21, r21
	inc r26
	sts tsIndex, r26
	ldi r26, lo8(timestamps)
	ldi r27, hi8(timestamps)
	add r26, r21
	adc r27, r1
	st X+, r20
	ldi r21, 3 //pin 3 on arduino board
	st X+, r21
	lds r20, TCNT4L
	lds r21, TCNT4H
	st X+, r20
	st X+, r21
	lds r20, TCNT5L
	lds r21, TCNT5H
	st X+, r20
	st X, r21
	pop r27
	pop r26
	pop r21
	out 0x3f, r21
	pop r21
	pop r20
	reti