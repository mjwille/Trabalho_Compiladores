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
TAC_NODE* codeGenerate(AST_NODE *node);

// Cria nodo do tipo TAC e coloca 'prev' e 'next' para NULL
TAC_NODE* tacCreate(int opcode, HASH_NODE *res, HASH_NODE *op1, HASH_NODE *op2);

// Imprime as TACs da frente para trás
void tacPrint(TAC_NODE *tac);

// Imprime um nodo do tipo TAC
void tacPrintNode(TAC_NODE *tac);

#endif