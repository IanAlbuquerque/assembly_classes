.text
.globl main
main:
	pushl %ebp
	movl %esp, %ebp
	
	movl $0xffff2222, %eax
	movl $0xaaaaaaaa, %ecx
	
	subl %eax,%ecx
	
	push %ecx
	movl %ebp, %esp
	popl %ebp
	
	ret

