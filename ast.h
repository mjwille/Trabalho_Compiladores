/* Trabalho de Compiladores 2020/1
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul
*/

#ifndef AST_HEADER
#define AST_HEADER

#include "hash.h"

// Definição dos tipos que um AST_NODE pode ter
#define AST_SYMBOL      1
#define AST_VEC         2
#define AST_ADD         3
#define AST_SUB         4
#define AST_MUL         5
#define AST_DIV         6
#define AST_LT          7
#define AST_GT          8
#define AST_LE          9
#define AST_GE         10
#define AST_EQ         11
#define AST_DIF        12
#define AST_XOR        13
#define AST_OR         14
#define AST_NOT        15
#define AST_FUNCALL    16
#define AST_ARGS       17
#define AST_ATTR       18
#define AST_ATTR_VEC   19
#define AST_READ       20
#define AST_PRINT      21
#define AST_PRINT_LIST 22
#define AST_RETURN     23
#define AST_IF         24
#define AST_IF_ELSE    25
#define AST_WHILE      26
#define AST_LOOP       27

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