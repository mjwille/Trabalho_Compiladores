# Trabalho de Compiladores 2020/ERE
# Nome: Marcelo Jantsch Wille
# Universidade Federal do Rio Grande do Sul


etapa5: main.o y.tab.o lex.yy.o hash.o ast.o semantic.o tac.o
	gcc main.o y.tab.o lex.yy.o hash.o ast.o semantic.o tac.o -o etapa5

main.o: main.c
	gcc -c main.c

hash.o: hash.c
	gcc -c hash.c

ast.o: ast.c
	gcc -c ast.c

semantic.o: semantic.c
	gcc -c semantic.c

tac.o: tac.c
	gcc -c tac.c

y.yab.o: y.tab.c
	gcc -c y.tab.c

lex.yy.o: lex.yy.c
	gcc -c lex.yy.c

lex.yy.c: scanner.l y.tab.c
	flex scanner.l

y.tab.c: parser.y
	yacc -d parser.y

clean:
	rm etapa5 *.o lex.yy.c y.tab.c y.tab.h