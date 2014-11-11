.text
.globl main
main:
	pushl %ebp
	movl %esp, %ebp

	movl $0xabcdef,-4(%ebp);
	movl $0xabcdef,-8(%ebp);
	movl $0xabcdef,-12(%ebp);
	movl $0xabcdef,-16(%ebp);
	movl $0xabcdef,-20(%ebp);
	subl $20,%esp;
	movl $0xab,%eax
	
	movl %ebp, %esp
	popl %ebp
	
	ret

