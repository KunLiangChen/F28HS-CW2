@ Task: compute the Hamming distance on 2 words
@       Given:  2 integer arrays xs and ys, where len(xs)==len(ys)
@       Return: the number of positions where the two arrays differ
@               i.e. | { i | i <- 0..len(xs), xs[i]!=ys[i] } |
	
@ Follows ARM subroutine calling conventions
	
	@ Entry point (Callable from C): 
	.global hamming
	
hamming:		  @ Input: 2 ptrs to int arrays in R0 and R1, length in R2
			  @ don't forget to push relevant registers here
	@ @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
	@ ... COMPLETE THIS CODE ...
	mov r3, #0
	cmp r2, #0
	ble	done
	push {r4, r5}
	
loop:
	cmp r2,#0
	ble done
	ldr r4, [r0], #4
	ldr r5, [r1], #4
	
	cmp r4,r5
	it ne
	addne r3,r3,#1

	subs r2,r2,#1
	b loop
	@ @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
			  @ don't forget to pop relevant registers here
done:
	mov r0,r3
	pop {r4,r5}	
	BX   LR

@ Test data	
.data
.equ VAL1, 1
.equ VAL2, 2	

@ Indicate to the linker that the code in this file does not need the stack
@ to be executable. (Recent versions of GNU ld warn if this is not present.)
.section .note.GNU-stack,"",%progbits
	
