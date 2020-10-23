/* Trabalho de Compiladores 2020/ERE
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul
*/

#include <stdio.h>
#include <stdlib.h>
#include "hash.h"

extern FILE *yyin;               // from scanner.l
extern void initMe();            // from scanner.l
extern int yyparse();            // from parser.y

int main(int argc, char *argv[])
{
	// Faz verificações de número de argumentos
	if(argc < 2) {
		fprintf(stderr, "ERROR!\nUsage: ./etapa4 <input_filename>\n");
		exit(1);
	}

	// Faz verificação de abertura correta do arquivo de input
	yyin = fopen(argv[1], "r");
	if(yyin == 0) {
		fprintf(stderr, "ERROR!\nFile %s could not be opened.\n", argv[1]);
		exit(2);
	}

	// Faz as inicializações necessárias (tabela hash)
	initMe();

	// Chama o analisador sintático
	yyparse();

	// Imprime a tabela de símbolos
	hashPrint();

	// Imprime mensagem de compilação bem feita até o fim
	printf("\nCompilation was a success.\n\n");

	exit(0);
}