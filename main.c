/* Trabalho de Compiladores 2020/ERE
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul
*/

#include <stdio.h>
#include <stdlib.h>
#include "hash.h"

extern FILE *yyin;               // from scanner.l
extern FILE *outputFile;         // from ast.h  (para descompilação da AST)
extern void initMe();            // from scanner.l
extern int yyparse();            // from parser.y

int main(int argc, char *argv[])
{
	// Faz verificações de número de argumentos
	if(argc < 3) {
		fprintf(stderr, "Error.\nUsage: ./etapa5 <input_filename> <output_filename>\n");
		exit(1);
	}

	// Faz verificação de abertura correta do arquivo de input
	yyin = fopen(argv[1], "r");
	if(yyin == 0) {
		fprintf(stderr, "Error.\nInput file %s could not be opened.\n", argv[1]);
		exit(2);
	}

	// Faz verificação de abertura correta do arquivo de output
	outputFile = fopen(argv[2], "w");
	if(outputFile == 0) {
		fprintf(stderr, "Error.\nOutput file %s could not be opened.\n", argv[2]);
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

	// Fecha arquivo de output (de ast.h) e encerra programa com código de sucesso
	fclose(outputFile);

	exit(0);
}