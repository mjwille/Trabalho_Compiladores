/* Trabalho de Compiladores 2020/ERE
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul
*/

#ifndef TAC_HEADER
#define TAC_HEADER

#include "hash.h"
#include "ast.h"

// Definição das operações (opcodes possíveis) para um nodo de "Three Address Code" (TAC)
#define TAC_SYMBOL 1
#define TAC_ADD    2
#define TAC_SUB    3
#define TAC_MUL    4
#define TAC_DIV    5
#define TAC_LT     6
#define TAC_GT     7
#define TAC_LE     8
#define TAC_GE     9
#define TAC_EQ    10
#define TAC_DIF   11
#define TAC_XOR   12
#define TAC_OR    13
#define TAC_NOT   14
#define TAC_COPY  15
#define TAC_JF    16
#define TAC_JMP   17
#define TAC_LABEL 18

typedef struct tac_node
{
   int opcode;
   HASH_NODE *res;
   HASH_NODE *op1;
   HASH_NODE *op2;
   struct tac_node *prev;
   struct tac_node *next;
} TAC_NODE;

// Percorre recursivamente a AST para gerar os códigos TAC
TAC_NODE* tacCodeGenerate(AST_NODE *node);

// Cria nodo do tipo TAC e coloca 'prev' e 'next' para NULL
TAC_NODE* tacCreate(int opcode, HASH_NODE *res, HASH_NODE *op1, HASH_NODE *op2);

// Imprime as TACs da frente para trás
void tacPrint(TAC_NODE *tac);

// Imprime um nodo do tipo TAC
void tacPrintNode(TAC_NODE *tac);

// Cria uma TAC para operadores binários
TAC_NODE* tacBinaryOperation(int opcode, TAC_NODE *son0, TAC_NODE *son1);

// Cria uma TAC para operadores unários
TAC_NODE* tacUnaryOperation(int opcode, TAC_NODE *son0);

// Cria uma TAC para o if/then
TAC_NODE* tacIfThen(TAC_NODE *son0, TAC_NODE *son1);

// Cria uma TAC para o if/then/else
TAC_NODE* tacIfThenElse(TAC_NODE *son0, TAC_NODE *son1, TAC_NODE *son2);

// Junta 2 TACs em uma TAC única maior
TAC_NODE* tacJoin(TAC_NODE *tac1, TAC_NODE *tac2);

#endif