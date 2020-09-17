/* Trabalho de Compiladores 2020/1
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul
*/

#ifndef AST_HEADER
#define AST_HEADER

#include "hash.h"

// Definição dos tipos que um AST_NODE pode ter
#define AST_SYMBOL 1
#define AST_ADD 2
#define AST_SUB 3
#define AST_MUL 4
#define AST_DIV 5
#define AST_LT 6
#define AST_GT 7
#define AST_LE 8
#define AST_GE 9
#define AST_EQ 10
#define AST_DIF 11
#define AST_XOR 12
#define AST_OR 13
#define AST_NOT 14
#define AST_RETURN 15

// Máximo de filhos que um AST_NODE pode ter
#define MAX_SONS 4

// Definição da AST
typedef struct AST_NODE
{
	int type;
	HASH_NODE *symbol;
	struct AST_NODE *son[MAX_SONS];
} AST_NODE;

// Insere nodo na AST dando seu tipo, o símbolo (que é o próprio nodo na tabela hash) e os nodos filhos
AST_NODE *astInsert(int type, HASH_NODE *symbol, AST_NODE *son0, AST_NODE *son1, AST_NODE *son2, AST_NODE *son3);

// Imprime toda a AST
void astPrint(AST_NODE *node);

// Função auxiliar para formatar a árvore durante a impressão da AST
void astFormat(int spaces, int bar, int last);

// Faz check se o nodo possui outro filho além do índice 'i' passado (um nodo pode ter no máximo 4 filhos)
int hasAnotherSon(AST_NODE *node, int i);

// Função auxiliar para fazer a impressão (esta é a função recursiva de impressão que é chamada por astPrint)
void astShow(AST_NODE *node, int spaces, int bar, int last);

#endif