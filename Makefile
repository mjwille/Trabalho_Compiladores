# Trabalho de Compiladores 2020/1
# Nome: Marcelo Jantsch Wille
# Universidade Federal do Rio Grande do Sul


etapa1: lex.yy.c
	gcc -o etapa1 lex.yy.c

lex.yy.c: scanner.l
	flex scanner.l

clean:
	rm etapa1 lex.yy.c