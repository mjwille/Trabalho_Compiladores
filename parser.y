/* Trabalho de Compiladores 2020/ERE
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul
*/

%{
        #include <stdio.h>
        #include <stdlib.h>
        #include "hash.h"
        #include "ast.h"
        #include "semantic.h"
        #include "tac.h"
        #include "asm.h"

        int yylex();
        int getLineNumber(void);
        int yyerror(char *s);
%}

%union
{
        HASH_NODE *symbol;
        AST_NODE *ast;
}

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
%token<symbol> TK_IDENTIFIER

%token<symbol> LIT_INTEGER
%token<symbol> LIT_FLOAT
%token<symbol> LIT_TRUE
%token<symbol> LIT_FALSE
%token<symbol> LIT_CHAR
%token<symbol> LIT_STRING

%token TOKEN_ERROR

%type<ast> programa
%type<ast> decl
%type<ast> dec
%type<ast> global_var
%type<ast> init_value
%type<ast> vector_size
%type<ast> init_vector
%type<ast> vector_values
%type<ast> type
%type<ast> func
%type<ast> parameters_list
%type<ast> parameters
%type<ast> parameter
%type<ast> block
%type<ast> commands
%type<ast> cmd
%type<ast> attr_cmd
%type<ast> read_cmd
%type<ast> print_cmd
%type<ast> elements
%type<ast> element
%type<ast> return_cmd
%type<ast> if_cmd
%type<ast> while_cmd
%type<ast> loop_cmd
%type<ast> expr
%type<ast> operand
%type<ast> func_call
%type<ast> arguments_list
%type<ast> arguments
%type<ast> argument

%left '^' '|' '~'
%left '<' '>' OPERATOR_EQ OPERATOR_DIF OPERATOR_GE OPERATOR_LE
%left '+' '-'
%left '*' '/'

%start programa

%%

programa: decl         { $$ = $1; semanticAnalysis($$); /* astPrint($$); */  generateAsm(tacReverse(tacCodeGenerate($$))); }
        ;

decl: dec ';' decl     { $$ = astInsert(AST_DECL,  NULL, $1, $3, NULL, NULL); }
    |                  { $$ = NULL; }
    ;

dec: global_var        { $$ = $1; }
   | func              { $$ = $1; }
   ;

/* Variáveis Globais */

global_var: TK_IDENTIFIER '=' type ':' init_value                         { $$ = astInsert(AST_DECL_VAR,      $1, $3, $5, NULL, NULL); }
          | TK_IDENTIFIER '=' type '[' vector_size ']' init_vector        { $$ = astInsert(AST_DECL_VAR_VEC,  $1, $3, $5,   $7, NULL); }
          ;

type: KW_BOOL                             { $$ = astInsert(AST_BOOL,  NULL, NULL, NULL, NULL, NULL); }
    | KW_CHAR                             { $$ = astInsert(AST_CHAR,  NULL, NULL, NULL, NULL, NULL); }
    | KW_INT                              { $$ = astInsert(AST_INT,   NULL, NULL, NULL, NULL, NULL); }
    | KW_FLOAT                            { $$ = astInsert(AST_FLOAT, NULL, NULL, NULL, NULL, NULL); }
    ;

init_value: LIT_INTEGER                   { $$ = astInsert(AST_SYMBOL,  $1, NULL, NULL, NULL, NULL); }
          | LIT_FLOAT                     { $$ = astInsert(AST_SYMBOL,  $1, NULL, NULL, NULL, NULL); }
          | LIT_CHAR                      { $$ = astInsert(AST_SYMBOL,  $1, NULL, NULL, NULL, NULL); }
          | LIT_TRUE                      { $$ = astInsert(AST_SYMBOL,  $1, NULL, NULL, NULL, NULL); }
          | LIT_FALSE                     { $$ = astInsert(AST_SYMBOL,  $1, NULL, NULL, NULL, NULL); }
          ;

vector_size: LIT_INTEGER                  { $$ = astInsert(AST_SYMBOL,  $1, NULL, NULL, NULL, NULL); }
           ;

init_vector: ':' vector_values            { $$ =   $2; }
           |                              { $$ = NULL; }
           ;

vector_values: init_value                          { $$ = $1; }
             | init_value vector_values            { $$ = astInsert(AST_VEC_VAL, NULL, $1, $2, NULL, NULL); }
             ;

/* Funções */

func: TK_IDENTIFIER '(' parameters_list ')' '=' type block   { $$ = astInsert(AST_DECL_FUNC, $1, $3, $6, $7, NULL); }
    ;

parameters_list: parameters                      { $$ =   $1; }
               |                                 { $$ = NULL; }
               ;

parameters: parameter                            { $$ = $1; }
          | parameter ',' parameters             { $$ = astInsert(AST_PARAMS, NULL, $1, $3, NULL, NULL); }
          ;

parameter: TK_IDENTIFIER '=' type                { $$ = astInsert(AST_PARAM, $1, $3, NULL, NULL, NULL); }
         ;

block: '{' commands '}'                          { $$ = astInsert(AST_BLOCK, NULL, $2, NULL, NULL, NULL); }
     ;

/* Comandos */

commands: cmd                                    { $$ = $1; }
        | cmd commands                           { $$ = astInsert(AST_CMD, NULL, $1, $2, NULL, NULL); }
        ;

cmd: attr_cmd                                    { $$ =   $1; }
   | read_cmd                                    { $$ =   $1; }
   | print_cmd                                   { $$ =   $1; }
   | return_cmd                                  { $$ =   $1; }
   | if_cmd                                      { $$ =   $1; }
   | while_cmd                                   { $$ =   $1; }
   | loop_cmd                                    { $$ =   $1; }
   | block                                       { $$ =   $1; }
   |                                             { $$ = NULL; }
   ;

/* Comando de Atribuição */

attr_cmd: TK_IDENTIFIER '=' expr                 { $$ = astInsert(AST_ATTR,     $1, $3, NULL, NULL, NULL); }
        | TK_IDENTIFIER '[' expr ']' '=' expr    { $$ = astInsert(AST_ATTR_VEC, $1, $3,   $6, NULL, NULL); }
        ;

/* Comando de Leitura (read) */

read_cmd: KW_READ TK_IDENTIFIER                  { $$ = astInsert(AST_READ, $2, NULL, NULL, NULL, NULL); }
        ;

/* Comando de Impressão (print) */

print_cmd: KW_PRINT elements                     { $$ = astInsert(AST_PRINT, NULL, $2, NULL, NULL, NULL); }
         ;

elements: element                                { $$ = $1; }
        | element ',' elements                   { $$ = astInsert(AST_PRINT_LIST, NULL, $1, $3,   NULL, NULL); }
        ;

element: LIT_STRING                              { $$ = astInsert(AST_SYMBOL, $1, NULL, NULL, NULL, NULL); }
       | expr                                    { $$ = $1; }
       ;

/* Comando de Retorno (return) */

return_cmd: KW_RETURN expr                        { $$ = astInsert(AST_RETURN, NULL, $2, NULL, NULL, NULL); }
          ;

/* Expressão */

expr: operand                                     { $$ = $1; }
    | '(' expr ')'                                { $$ = astInsert(AST_PARENTHESIS, NULL, $2, NULL, NULL, NULL); }
    | expr '+' expr                               { $$ = astInsert(AST_ADD,         NULL, $1,   $3, NULL, NULL); }
    | expr '-' expr                               { $$ = astInsert(AST_SUB,         NULL, $1,   $3, NULL, NULL); }
    | expr '*' expr                               { $$ = astInsert(AST_MUL,         NULL, $1,   $3, NULL, NULL); }
    | expr '/' expr                               { $$ = astInsert(AST_DIV,         NULL, $1,   $3, NULL, NULL); }
    | expr '<' expr                               { $$ = astInsert(AST_LT,          NULL, $1,   $3, NULL, NULL); }
    | expr '>' expr                               { $$ = astInsert(AST_GT,          NULL, $1,   $3, NULL, NULL); }
    | expr OPERATOR_LE expr                       { $$ = astInsert(AST_LE,          NULL, $1,   $3, NULL, NULL); }
    | expr OPERATOR_GE expr                       { $$ = astInsert(AST_GE,          NULL, $1,   $3, NULL, NULL); }
    | expr OPERATOR_EQ expr                       { $$ = astInsert(AST_EQ,          NULL, $1,   $3, NULL, NULL); }
    | expr OPERATOR_DIF expr                      { $$ = astInsert(AST_DIF,         NULL, $1,   $3, NULL, NULL); }
    | expr '^' expr                               { $$ = astInsert(AST_XOR,         NULL, $1,   $3, NULL, NULL); }
    | expr '|' expr                               { $$ = astInsert(AST_OR,          NULL, $1,   $3, NULL, NULL); }
    | '~' expr                                    { $$ = astInsert(AST_NOT,         NULL, $2, NULL, NULL, NULL); }
    ;

operand: TK_IDENTIFIER                            { $$ = astInsert(AST_SYMBOL, $1, NULL, NULL, NULL, NULL); }
       | TK_IDENTIFIER '[' expr ']'               { $$ = astInsert(   AST_VEC, $1,   $3, NULL, NULL, NULL); }
       | LIT_INTEGER                              { $$ = astInsert(AST_SYMBOL, $1, NULL, NULL, NULL, NULL); }
       | LIT_FLOAT                                { $$ = astInsert(AST_SYMBOL, $1, NULL, NULL, NULL, NULL); }
       | LIT_CHAR                                 { $$ = astInsert(AST_SYMBOL, $1, NULL, NULL, NULL, NULL); }
       | func_call                                { $$ = $1; }
       ;

func_call: TK_IDENTIFIER '(' arguments_list ')'   { $$ = astInsert(AST_FUNCALL, $1, $3, NULL, NULL, NULL); }
         ;

arguments_list: arguments                         { $$ =   $1; }
              |                                   { $$ = NULL; }
              ;

arguments: argument                               { $$ = $1; }
         | argument ',' arguments                 { $$ = astInsert(AST_ARGS, NULL, $1, $3, NULL, NULL);   }
         ;

argument: expr                                    { $$ = $1; }
        ;

/* Comandos de Controle de Fluxo */

if_cmd: KW_IF '(' expr ')' KW_THEN cmd                   { $$ = astInsert(     AST_IF, NULL, $3, $6, NULL, NULL); }
      | KW_IF '(' expr ')' KW_THEN cmd KW_ELSE cmd       { $$ = astInsert(AST_IF_ELSE, NULL, $3, $6,   $8, NULL); }
      ;

while_cmd: KW_WHILE '(' expr ')' cmd                     { $$ = astInsert(  AST_WHILE, NULL, $3, $5, NULL, NULL); }
         ;

loop_cmd: KW_LOOP '(' TK_IDENTIFIER ':' expr ',' expr ',' expr ')' cmd      { $$ = astInsert(AST_LOOP, $3, $5, $7, $9, $11); }
        ;

%%

int yyerror(char *s) {
	fprintf(stderr, "Line %d: Syntax Error.\n", getLineNumber());
	exit(3);
}