main:
	addi a0, zero, 2
	addi a1, zero, 4
	jal ra, _mulp


inf:
	jal ra, inf


# s = a * b
#	a -> a0
#	b -> a1
#	s -> a0
_mulp:
	addi sp, sp, -4
	sw ra, 0(sp)

	addi t0, zero, 0
	beq a0, zero, 2f
	beq a1, zero, 2f

1:
	add t0, t0, a0
	addi a1, a1, -1
	bne a1, zero, 1b
2:
	addi a0, t0, 0

	lw ra, 0(sp)
	addi sp, sp, 4
	jalr zero, 0(ra)
