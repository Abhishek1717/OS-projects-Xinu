
	
	.globl zfunction 


zfunction:
     
	pushl %ebp
	movl %esp, %ebp
	movl 8(%ebp), %ebx
	sall $0x04 ,%ebx           //shift the elements by two 
	andl $0xf803ffff, %ebx     // and to make the bits 0
	movl %ebx, %eax
	popl %ebp

	ret

