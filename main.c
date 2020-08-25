// Verifica se o número de argumentos do programa é correto, abortando se não forem.
void check_arguments(int argc) {
	if(argc < 2) {
		fprintf(stderr, "ERROR!\nUsage: ./etapa1 <filename>\n");
		exit(1);
	}
}

// Verifica se o arquivo pode ser aberto, abortando caso não conseguir (yyin é variável do lex para o arquivo)
void set_yyin(char *filename) {
	yyin = fopen(filename, "r");
	if(yyin == 0) {
		fprintf(stderr, "ERROR!\nFile %s could not be opened.\n", filename);
		exit(2);
	}
}


int main(int argc, char *argv[])
{
	// Faz verificações de número de argumentos e abertura correta do arquivo
	check_arguments(argc);
	set_yyin(argv[1]);

	// Inicializa a tabela hash com valores default NULL
	hashInit();

	int tok;

	while(isRunning())
	{
		// Pede um token para o analisador léxico
		tok = yylex();
		switch(tok)
		{
			case 256: printf("KW_CHAR");        printf(" na linha %d.\n", getLineNumber()); break;
			case 257: printf("KW_INT");         printf(" na linha %d.\n", getLineNumber()); break;
			case 259: printf("KW_FLOAT");       printf(" na linha %d.\n", getLineNumber()); break;
			case 260: printf("KW_BOOL");        printf(" na linha %d.\n", getLineNumber()); break;
			case 261: printf("KW_IF");          printf(" na linha %d.\n", getLineNumber()); break;
			case 262: printf("KW_THEN");        printf(" na linha %d.\n", getLineNumber()); break;
			case 263: printf("KW_ELSE");        printf(" na linha %d.\n", getLineNumber()); break;
			case 264: printf("KW_WHILE");       printf(" na linha %d.\n", getLineNumber()); break;
			case 265: printf("KW_LOOP");        printf(" na linha %d.\n", getLineNumber()); break;
			case 266: printf("KW_READ");        printf(" na linha %d.\n", getLineNumber()); break;
			case 267: printf("KW_PRINT");       printf(" na linha %d.\n", getLineNumber()); break;
			case 268: printf("KW_RETURN");      printf(" na linha %d.\n", getLineNumber()); break;
			case 270: printf("OP_LE");          printf(" na linha %d.\n", getLineNumber()); break;
			case 271: printf("OP_GE");          printf(" na linha %d.\n", getLineNumber()); break;
			case 272: printf("OP_EQ");          printf(" na linha %d.\n", getLineNumber()); break;
			case 273: printf("OP_DF");          printf(" na linha %d.\n", getLineNumber()); break;
			case 280: printf("TK_IDENTIFIER");  printf(" na linha %d.\n", getLineNumber()); break;
			case 281: printf("LIT_INT");        printf(" na linha %d.\n", getLineNumber()); break;
			case 282: printf("LIT_FLOAT");      printf(" na linha %d.\n", getLineNumber()); break;
			case 283: printf("LIT_TRUE");       printf(" na linha %d.\n", getLineNumber()); break;
			case 284: printf("LIT_FALSE");      printf(" na linha %d.\n", getLineNumber()); break;
			case 285: printf("LIT_CHAR");       printf(" na linha %d.\n", getLineNumber()); break;
			case 286: printf("LIT_STR");        printf(" na linha %d.\n", getLineNumber()); break;
			case 290: printf("TK_ERROR");       printf(" na linha %d.\n", getLineNumber()); break;
			default:  printf("%c", yytext[0]);  printf(" na linha %d.\n", getLineNumber()); break;
		}
	}

	hashPrint();

	return 0;
}