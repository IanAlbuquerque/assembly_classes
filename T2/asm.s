.text
.globl main
main:
	movl 12(%ebp),%eax;
TESTE:
	cmpl $0xabcdef,-16(%ebp)
	cmpl $0xabcdef,-20(%ebp)
GO:
	cmpl $0xabcdef,-12(%ebp)
	cmpl -16(%ebp),%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	je TESTE;
	je main;
	je GO;
	je GO2;
	jne GO2;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
GO2:
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;
	cmpl $0xabcdef,%eax;

	movl %eax,-20(%ebp);

	movl -20(%ebp),%eax;
	movl %ebp, %esp
	popl %ebp
	ret

	subl $0xabcdef,%esp
	
