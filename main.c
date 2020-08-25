void check_arguments(int argc) {
	if(argc < 2) {
		fprintf(stderr, "ERROR!\nUsage: ./etapa1 <filename>\n");
		exit(1);
	}
}


void set_yyin(char *filename) {
	yyin = fopen(filename, "r");
	if(yyin == 0) {
		fprintf(stderr, "ERROR!\nFile %s could not be opened.\n", filename);
		exit(2);
	}
}


int main(int argc, char *argv[])
{
	int tok;

	check_arguments(argc);
	set_yyin(argv[1]);

	while(isRunning())
	{
		tok = yylex();
		switch(tok)
		{
			case 256: printf("KW_CHAR");        printf(" -> %d\n", lineNumber); break;
			case 257: printf("KW_INT");         printf(" -> %d\n", lineNumber); break;
			case 259: printf("KW_FLOAT");       printf(" -> %d\n", lineNumber); break;
			case 260: printf("KW_BOOL");        printf(" -> %d\n", lineNumber); break;
			case 261: printf("KW_IF");          printf(" -> %d\n", lineNumber); break;
			case 262: printf("KW_THEN");        printf(" -> %d\n", lineNumber); break;
			case 263: printf("KW_ELSE");        printf(" -> %d\n", lineNumber); break;
			case 264: printf("KW_WHILE");       printf(" -> %d\n", lineNumber); break;
			case 265: printf("KW_LOOP");        printf(" -> %d\n", lineNumber); break;
			case 266: printf("KW_READ");        printf(" -> %d\n", lineNumber); break;
			case 267: printf("KW_PRINT");       printf(" -> %d\n", lineNumber); break;
			case 268: printf("KW_RETURN");      printf(" -> %d\n", lineNumber); break;
			case 270: printf("OP_LE");          printf(" -> %d\n", lineNumber); break;
			case 271: printf("OP_GE");          printf(" -> %d\n", lineNumber); break;
			case 272: printf("OP_EQ");          printf(" -> %d\n", lineNumber); break;
			case 273: printf("OP_DF");          printf(" -> %d\n", lineNumber); break;
			case 280: printf("TK_IDENTIFIER");  printf(" -> %d\n", lineNumber); break;
			case 281: printf("LIT_INT");        printf(" -> %d\n", lineNumber); break;
			case 282: printf("LIT_FLOAT");      printf(" -> %d\n", lineNumber); break;
			case 283: printf("LIT_TRUE");       printf(" -> %d\n", lineNumber); break;
			case 284: printf("LIT_FALSE");      printf(" -> %d\n", lineNumber); break;
			case 285: printf("LIT_CHAR");       printf(" -> %d\n", lineNumber); break;
			case 286: printf("LIT_STR");        printf(" -> %d\n", lineNumber); break;
			case 290: printf("TK_ERROR");       printf(" -> %d\n", lineNumber); break;
			default:  printf("%c", yytext[0]);  printf(" -> %d\n", lineNumber); break;
		}
	}
}