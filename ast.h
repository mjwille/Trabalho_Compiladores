/* Trabalho de Compiladores 2020/ERE
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul
*/

#ifndef AST_HEADER
#define AST_HEADER

#include "hash.h"

FILE *outputFile;

// Definição dos tipos que um AST_NODE pode ter
#define AST_SYMBOL         1
#define AST_VEC            2
#define AST_ADD            3
#define AST_SUB            4
#define AST_MUL            5
#define AST_DIV            6
#define AST_LT             7
#define AST_GT             8
#define AST_LE             9
#define AST_GE            10
#define AST_EQ            11
#define AST_DIF           12
#define AST_XOR           13
#define AST_OR            14
#define AST_NOT           15
#define AST_PARENTHESIS   16
#define AST_FUNCALL       17
#define AST_ARGS          18
#define AST_CMD           19
#define AST_ATTR          20
#define AST_ATTR_VEC      21
#define AST_READ          22
#define AST_PRINT         23
#define AST_PRINT_LIST    24
#define AST_RETURN        25
#define AST_IF            26
#define AST_IF_ELSE       27
#define AST_WHILE         28
#define AST_LOOP          29
#define AST_BLOCK         30
#define AST_PARAMS        31
#define AST_PARAM         32
#define AST_DECL_FUNC     33
#define AST_BOOL          34
#define AST_CHAR          35
#define AST_INT           36
#define AST_FLOAT         37
#define AST_DECL_VAR      38
#define AST_DECL_VAR_VEC  39
#define AST_VEC_VAL       40
#define AST_DECL          41

// Máximo de filhos que um AST_NODE pode ter
#define MAX_SONS 4

// Definição da AST
typedef struct AST_NODE
{
	int type;
	HASH_NODE *symbol;
	int lineNumber;
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
void astPrintNode(AST_NODE *node, int spaces, int bar, int last);

// Função para avaliação da etapa3, que descompila a AST em código novamente
void decompile(AST_NODE *node);

// Função que joga no arquivo de saída a decompilação nodo a nodo da AST gerada durante a compilação
void decompileNode(AST_NODE *node);

#endif