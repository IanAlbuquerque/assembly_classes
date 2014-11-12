compile:
	clear
	gcc -Wall -Wa,--execstack -m32 -o mincc main.c geracod.c
	
run: compile
	clear
	./mincc meu_programa_minimo.min
	
asm:
	clear
	gcc -Wall -c -m32 asm.s
	objdump -d asm.o
	rm asm.o
	
