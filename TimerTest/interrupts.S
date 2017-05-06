
/*
 * interrupts.S
 * External interrupt vectors. Sets fields of the current timestamp,
 * and increments the index value. The absolute time is created with the number
 * of overflows as the high order bytes, and the current timer value as the lower.
 * These take a total of 71 cycles to complete, including 5 cycles to enter the
 * vector table and an rjmp to one of these vectors. This is 4.4375 us.
 *
 * Created: 4/6/2017 2:22:16 PM
 *  Author: Michael
 */
 #include <avr/io.h>
 #define ARRAY_SIZE 255

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
	push r22
	push r26
	push r27
	lds r26, int2Write
	mov r21, r26
	clr r22
	lsl r21
	rol r22
	lsl r21
	rol r22
	add r21, r26
	adc r22, r1
	inc r26
	andi r26, ARRAY_SIZE
	sts int2Write, r26
	ldi r26, lo8(int2Timestamps)
	ldi r27, hi8(int2Timestamps)
	add r26, r21
	adc r27, r22
	st X+, r20
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
	pop r22
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
	push r22
	push r26
	push r27
	lds r26, int3Write
	mov r21, r26
	clr r22
	lsl r21
	rol r22
	lsl r21
	rol r22
	add r21, r26
	adc r22, r1
	inc r26
	andi r26, ARRAY_SIZE
	sts int3Write, r26
	ldi r26, lo8(int3Timestamps)
	ldi r27, hi8(int3Timestamps)
	add r26, r21
	adc r27, r22
	st X+, r20
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
	pop r22
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
	push r22
	push r26
	push r27
	lds r26, int4Write
	mov r21, r26
	clr r22
	lsl r21
	rol r22
	lsl r21
	rol r22
	add r21, r26
	adc r22, r1
	inc r26
	andi r26, ARRAY_SIZE
	sts int4Write, r26
	ldi r26, lo8(int4Timestamps)
	ldi r27, hi8(int4Timestamps)
	add r26, r21
	adc r27, r22
	st X+, r20
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
	pop r22
	pop r21
	out 0x3f, r21
	pop r21
	pop r20
	reti

__vector_6:
	push r20
	in r20, 0x0C //in PINE
	push r21
	in r21, 0x3f
	push r21
	andi r20, 0x20 //bit 5
	push r22
	push r26
	push r27
	lds r26, int5Write
	mov r21, r26
	clr r22
	lsl r21
	rol r22
	lsl r21
	rol r22
	add r21, r26
	adc r22, r1
	inc r26
	andi r26, ARRAY_SIZE
	sts int5Write, r26
	ldi r26, lo8(int5Timestamps)
	ldi r27, hi8(int5Timestamps)
	add r26, r21
	adc r27, r22
	st X+, r20
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
	pop r22
	pop r21
	out 0x3f, r21
	pop r21
	pop r20
	reti