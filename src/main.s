main:
	addi a0, zero, 4
	jal ra, facto


inf:
	jal ra, inf

# s = a * b       (a,b >= 0)
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

# s = a!
#	a -> a0
#	s -> a0
facto:
	addi sp, sp, -4
	sw ra, 0(sp)

	addi t0, a0, 0
	addi t1, zero, 1
	beq a0, zero, 2f
1:
	# set a0, a1
	addi a0, t0, 0
	addi a1, t1, 0
	# call _mulp
	addi sp, sp, -8
	sw t0, 0(sp)
	sw t1, 4(sp)
	jal ra, _mulp
	lw t1, 4(sp)
	lw t0, 0(sp)
	addi sp, sp, 8

	addi t0, t0, -1
	addi t1, a0, 0
	bne t0, zero, 1b
2:
	# set result
	addi a0, t1, 0

	lw ra, 0(sp)
	addi sp, sp, 4
	jalr zero, 0(ra)
	
