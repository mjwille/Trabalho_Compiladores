/* Trabalho de Compiladores 2020/ERE
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul

   INSTRUÇÕES DE USO

   Em uma máquina Linux, com arquitetura x86-64:

   1. Executar este programa com uma entrada válida da linguagem
   ./etapa6 my_program.txt

   2. Usar a saída do comando anterior (asm_code.s) com gcc
   gcc asm_code.s

   3. Executar o arquivo
   ./a.out
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
	if(argc < 2) {
		fprintf(stderr, "Error.\nUsage: ./etapa6 <input_filename>\n");
		exit(1);
	}

	// Faz verificação de abertura correta do arquivo de input
	yyin = fopen(argv[1], "r");
	if(yyin == 0) {
		fprintf(stderr, "Error.\nInput file %s could not be opened.\n", argv[1]);
		exit(2);
	}

	// Faz as inicializações necessárias (tabela hash)
	initMe();

	// Chama o analisador sintático
	yyparse();

	// Imprime a tabela de símbolos
	hashPrint();

	// Imprime mensagem de compilação bem feita até o fim
	printf("==> Compilation was a success.\n\n");

	// Fecha arquivo de output (de ast.h) e encerra programa com código de sucesso
	fclose(outputFile);

	exit(0);
}