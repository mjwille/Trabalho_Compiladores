/* Trabalho de Compiladores 2020/1
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul
*/

#ifndef AST_HEADER
#define AST_HEADER

#include "hash.h"

#define AST_SYMBOL 1
#define AST_ADD 2
#define AST_SUB 3
#define AST_MUL 4
#define AST_DIV 5

#define MAX_SONS 4

typedef struct AST_NODE
{
	int type;
	HASH_NODE *symbol;
	struct AST_NODE *son[MAX_SONS];
} AST_NODE;

AST_NODE *astInsert(int type, HASH_NODE *symbol, AST_NODE *son0, AST_NODE *son1, AST_NODE *son2, AST_NODE *son3);
void astPrint(AST_NODE *node);
void astFormat(int spaces, int bar, int last);
int hasAnotherSon(AST_NODE *node, int i);
void astShow(AST_NODE *node, int spaces, int bar, int last);

#endif