/* Trabalho de Compiladores 2020/1
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul
*/

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

decl: dec ';' decl
    |
    ;

dec: global_var
   | func
   ;

/* Variáveis Globais */

global_var: TK_IDENTIFIER '=' type ':' global_init_value
          | TK_IDENTIFIER '=' type '[' global_vector_size ']' ':' global_init_value
          ;

type: KW_BOOL
    | KW_CHAR
    | KW_INT
    | KW_FLOAT
    ;

global_init_value: LIT_INTEGER
                 | LIT_FLOAT
                 | LIT_CHAR
                 ;

global_vector_size: LIT_INTEGER
                  |
                  ;

/* Funções */

func: TK_IDENTIFIER '(' parameters ')' '=' type block
    ;

parameters: parameter
          | parameter ',' parameters
          ;

parameter: TK_IDENTIFIER '=' type
         ;

block: '{' commands '}'
     ;

/* Comandos */

commands: cmd
        | cmd commands
        ;

cmd: attr_cmd
   | read_cmd
   | print_cmd
   | return_cmd
   |
   ;

/* Comando de Atribuição */

attr_cmd: TK_IDENTIFIER '=' expr
        | TK_IDENTIFIER '[' expr ']' '=' expr
        ;

/* Comando de Leitura (read) */

read_cmd: KW_READ TK_IDENTIFIER
        ;

/* Comando de Impressão (print) */

print_cmd: KW_PRINT elements
         ;

elements: element
        | element elements
        ;

element: LIT_STRING
       | expr
       ;

/* Comando de Retorno (return) */

return_cmd: KW_RETURN expr
          ;

/* Expressão */

expr: operand
    | '(' expr ')'
    | expr '+' expr
    | expr '-' expr
    | expr '*' expr
    | expr '/' expr
    | expr '<' expr
    | expr '>' expr
    | expr OPERATOR_LE expr
    | expr OPERATOR_GE expr
    | expr OPERATOR_EQ expr
    | expr OPERATOR_DIF expr
    | expr '^' expr
    | expr '|' expr
    | '~' expr
    ;

operand: TK_IDENTIFIER
       | LIT_INTEGER
       | LIT_FLOAT
       | LIT_CHAR
       | func_call
       ;

func_call: TK_IDENTIFIER '(' arguments ')'
         ;

arguments: argument
         | argument ',' arguments
         ;

argument: expr
        ;

%%

int yyerror(char *s) {
	fprintf(stderr, "Line %d: Syntax Error.\n", getLineNumber());
	exit(3);
}