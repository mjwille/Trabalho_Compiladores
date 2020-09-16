/* Trabalho de Compiladores 2020/1
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

AST_NODE *astInsert(int type, HASH_NODE *symbol, AST_NODE *son0, AST_NODE *son1, AST_NODE *son2, AST_NODE *son3) {
	AST_NODE *node;

	node->type = type;
	node->symbol = symbol;
	node->son[0] = son0;
	node->son[1] = son1;
	node->son[2] = son2;
	node->son[3] = son3;

	return node;
}

void astPrint(AST_NODE *node) {
	switch(node->type) {
		case AST_SYMBOL: printf("AST_SYMBOL!\n"); break;
		case AST_ADD:    printf("AST_ADD!\n");    break;
		case AST_SUB:    printf("AST_SUB!\n");    break;
		case AST_MUL:    printf("AST_MUL!\n");    break;
		case AST_DIV:    printf("AST_DIV!\n");    break;
		default:         printf("AST_UNKN!\n");   break;
	}

	if(node-> symbol != NULL) {
		printf("%s\n", node->symbol->text);
	}
	else {
		printf("NULL\n");
	}

	int i;
	for(i=0; i<MAX_SONS; i++) {
		astPrint(node->son[i]);
	}
}