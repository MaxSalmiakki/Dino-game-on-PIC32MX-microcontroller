  # labwork.S
  # Written 2015-2017 by F Lundevall
  # Skeleton file for IS1200/IS1500 lab 1.
  # The macros PUSH and POP are in the public domain.
  # Please add your own code at the end of the file.

  #
  # Please keep the two macros PUSH and POP unchanged
  #
.macro  PUSH reg
 addi  $sp,$sp,-4
 sw  \reg,0 ($sp)
.endm
 
.macro  POP reg
 lw  \reg,0($sp)
 addi  $sp,$sp,4
.endm
  #
  # Please add your own code below this line
  #
.global delay
.global time2string
.global enable_interrupt

.text 
hexasc:

li  $t1, 9
ble $a0, $t1,case1
nop

li     $t0,0x30
add    $v0,$t0,$a0
addi   $v0,$v0, 7
jr $ra
nop

	case1:
		li            $t0,0x30
		add           $v0,$t0,$a0
		jr $ra 
		nop
		break

	
delay:

# Set value of $t0 to the amount of milliseconds (ms) we need wait

		# Set the value of i to 0
		li $t4, 0
		
		#start running the while loop
		j while
		nop
		
		
	loop2:
		addi $t4, $t4, 1 #i++
		
		#Go back to while if equal to 4710
		beq $t4, 6999, while
		nop
		#Re-access loop 2 if $t1 is less than 4711
		blt $t4, 7000, loop2
		nop
	
	
	while:
		
		# Decrease ms by 1
		addi $a0, $a0, -1
		
		li $t4, 0
		#Access the loop
		bgt $a0, 0, loop2
		nop
		jr $ra
		nop



time2string:

PUSH $s0
PUSH $s1
PUSH $ra
move $s0,$a0
move $s1,$a1 

 # push pop s0 and s1
 # store a1 and a0 in s register 

    # copys content of a0 to s0
   

andi $a0,$s1,0xf   # extracts 4 lsb and stores to t2
jal hexasc 
nop
sb $v0, 4($s0) 

andi $a0,$s1,0xf0   # extracts bit 7-4 bits and stores to t2
srl $a0, $a0, 4
jal hexasc 
nop
sb $v0,3($s0) 

li $a0, 10
jal case1  # put in the colon (0x3a)
nop
sb $v0, 2($s0)


andi $a0,$s1,0xf00   # extracts 11- 8 bits and stores to t2
srl $a0, $a0, 8
jal hexasc 
nop
sb $v0, 1($s0)


andi $a0,$s1,0xf000
srl $a0, $a0, 12
jal hexasc 
nop
sb $v0, 0($s0)

li $t0, 0x00
sb $t0, 5($s0)

move $a0, $s0


POP $ra
POP $s1
POP $s0

jr $ra
nop


enable_interrupt:
PUSH $ra
EI 
POP $ra
jr $ra
nop



