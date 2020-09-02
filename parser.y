%{
int yyerror(char *s);
%}

%token KW_CHAR
%token KW_INT
%token KW_FLOAT
%token KW_BOOL

%token KW_IF
%token KW_THEN
%token KW_ELSE
%token KW_WHILE
%token KW_LOOP
%token KW_READ
%token KW_PRINT
%token KW_RETURN

%token OPERATOR_LE
%token OPERATOR_GE
%token OPERATOR_EQ
%token OPERATOR_DIF
%token TK_IDENTIFIER

%token LIT_INTEGER
%token LIT_FLOAT
%token LIT_TRUE
%token LIT_FALSE
%token LIT_CHAR
%token LIT_STRING

%token TOKEN_ERROR

%%

programa: decl
        ;

decl: dec ',' decl
    |
    ;

dec: KW_INT TK_IDENTIFIER
   | KW_INT TK_IDENTIFIER '(' ')' '{' '}'
   ;

%%

int yyerror(char *s) {
	fprintf(stderr, "Line %d: Syntax Error.\n", getLineNumber());
	exit(3);
}