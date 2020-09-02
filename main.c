/* Trabalho de Compiladores 2020/1
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul
*/

int main(int argc, char *argv[])
{
	// Faz verificações de número de argumentos
	if(argc < 2) {
		fprintf(stderr, "ERROR!\nUsage: ./etapa1 <filename>\n");
		exit(1);
	}

	// Faz verificação de abertura correta do arquivo
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

	printf("Compilation was a success.\n");
	exit(0);
}