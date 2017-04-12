
/*
 * interrupts.S
 *
 * Created: 4/6/2017 2:22:16 PM
 *  Author: Michael
 */
 #include <avr/io.h>

.section .text
.global __vector_3

/*
 * External interrupt 2 vector. Sets fields of the current timestamp,
 * and increments the index value. The absolute time is created with the number
 * of overflows as the high order bytes, and the current timer value as the lower.
 */
__vector_3:
	/*sbi 5, 7
	ldi r24, 250
	ldi r25, 0
	ldi  r18, 21
    ldi  r19, 75
    ldi  r20, 188
1:  dec  r20
    brne 1b
    dec  r19
    brne 1b
    dec  r18
    brne 1b
    nop
	cbi 5, 7
	ldi  r18, 21
    ldi  r19, 75
    ldi  r20, 188
1:  dec  r20
    brne 1b
    dec  r19
    brne 1b
    dec  r18
    brne 1b
    nop
	rjmp __vector_3
	*/
	push r22
	push r23
	lds r22, TCNT4L
	lds r23, TCNT4H
	push r24
	push r25
	lds r24, TCNT5L
	lds r25, TCNT5H
	push r20
	in r20, 0x3f
	push r20
	in r20, 0x09 //in PIND
	andi r20, 4 //bit 4
	push r21
	push r30
	push r31
	lds r30, tsIndex
	mov r21, r30
	add r21, r21
	add r21, r30
	add r21, r21
	inc r30
	sts tsIndex, r30
	ldi r30, lo8(timestamps)
	ldi r31, hi8(timestamps)
	add r30, r21
	adc r31, r1
	st Z+, r20
	ldi r21, 19 //pin 19 on arduino board
	st Z+, r21
	st Z+, r22
	st Z+, r23
	st Z+, r24
	st Z, r25
	pop r31
	pop r30
	pop r21
	pop r20
	out 0x3f, r20
	pop r20
	pop r25
	pop r24
	pop r23
	pop r22
	reti