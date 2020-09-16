# Trabalho de Compiladores 2020/1
# Nome: Marcelo Jantsch Wille
# Universidade Federal do Rio Grande do Sul


etapa3: main.o y.tab.o lex.yy.o hash.o
	gcc main.o y.tab.o lex.yy.o hash.o -o etapa3

main.o: main.c
	gcc -c main.c

hash.o: hash.c
	gcc -c hash.c

y.yab.o: y.tab.c
	gcc -c y.tab.c

lex.yy.o: lex.yy.c
	gcc -c lex.yy.c

lex.yy.c: scanner.l y.tab.c
	flex scanner.l

y.tab.c: parser.y
	yacc -d parser.y

clean:
	rm etapa3 *.o lex.yy.c y.tab.c y.tab.h