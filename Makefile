# Trabalho de Compiladores 2020/1
# Nome: Marcelo Jantsch Wille
# Universidade Federal do Rio Grande do Sul


etapa2: lex.yy.c
	gcc -o etapa3 lex.yy.c

lex.yy.c: scanner.l y.tab.c
	flex scanner.l

y.tab.c: parser.y
	yacc -d parser.y

clean:
	rm etapa3 lex.yy.c y.tab.c y.tab.h