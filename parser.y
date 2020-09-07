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

%left '^' '|' '~'
%left '<' '>' OPERATOR_EQ OPERATOR_DIF OPERATOR_GE OPERATOR_LE
%left '+' '-'
%left '*' '/'

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

global_var: TK_IDENTIFIER '=' type ':' init_value
          | TK_IDENTIFIER '=' type '[' vector_size ']' init_vector
          ;

type: KW_BOOL
    | KW_CHAR
    | KW_INT
    | KW_FLOAT
    ;

init_value: LIT_INTEGER
          | LIT_FLOAT
          | LIT_CHAR
          | LIT_TRUE
          | LIT_FALSE
          ;

vector_size: LIT_INTEGER
           ;

init_vector: ':' vector_values
           |
           ;

vector_values: init_value
             | init_value vector_values
             ;

/* Funções */

func: TK_IDENTIFIER '(' parameters_list ')' '=' type block
    ;

parameters_list: parameters
               |
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
   | if_cmd
   | while_cmd
   | loop_cmd
   | block
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
        | elements ',' element
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
    | expr '~' expr
    ;

operand: TK_IDENTIFIER
       | TK_IDENTIFIER '[' LIT_INTEGER ']'
       | LIT_INTEGER
       | LIT_FLOAT
       | LIT_CHAR
       | func_call
       ;

func_call: TK_IDENTIFIER '(' arguments_list ')'
         ;

arguments_list: arguments
              |
              ;

arguments: argument
         | argument ',' arguments
         ;

argument: expr
        ;

/* Comandos de Controle de Fluxo */

if_cmd: KW_IF '(' expr ')' KW_THEN cmd
      | KW_IF '(' expr ')' KW_THEN cmd KW_ELSE cmd
      ;

while_cmd: KW_WHILE '(' expr ')' cmd
         ;

loop_cmd: KW_LOOP '(' TK_IDENTIFIER ':' expr ',' expr ',' expr ')' cmd
        ;

%%

int yyerror(char *s) {
	fprintf(stderr, "Line %d: Syntax Error.\n", getLineNumber());
	exit(3);
}