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



	movl -4(%ebp),%eax;
	movl -8(%ebp),%eax;
	movl -12(%ebp),%eax;
	movl -16(%ebp),%eax;
	movl -20(%ebp),%eax;

	movl $0xabcdef,%eax;

	addl $0xabcdef,%eax;

	movl %eax,-4(%ebp);
	movl %eax,-8(%ebp);
	movl %eax,-12(%ebp);
	movl %eax,-16(%ebp);
	movl %eax,-20(%ebp);

	subl $0xabcdef,%eax;
	imull $0xabcdef,%eax;

	movl -4(%ebp),%eax;
	movl -8(%ebp),%eax;
	movl -12(%ebp),%eax;
	movl -16(%ebp),%eax;
	movl -20(%ebp),%eax;

	movl $0,-4(%ebp);
	movl $0,-8(%ebp);
	movl $0,-12(%ebp);
	movl $0,-16(%ebp);
	movl $0,-20(%ebp);

	addl -4(%ebp),%eax;
	addl -8(%ebp),%eax;
	addl -12(%ebp),%eax;
	addl -16(%ebp),%eax;
	addl -20(%ebp),%eax;

	TESTE: subl -4(%ebp),%eax;
	subl -8(%ebp),%eax;
	subl -12(%ebp),%eax;
	subl -16(%ebp),%eax;
	subl -20(%ebp),%eax;

	imull -4(%ebp),%eax;
	imull -8(%ebp),%eax;
	imull -12(%ebp),%eax;
	imull -16(%ebp),%eax;
	imull -20(%ebp),%eax;

	movl 12(%ebp),%eax;

	movl 12(%ebp),%eax;
	cmp %eax,-16(%ebp);
	je TESTE;
	