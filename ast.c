/* Trabalho de Compiladores 2020/1
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

AST_NODE *astInsert(int type, HASH_NODE *symbol, AST_NODE *son0, AST_NODE *son1, AST_NODE *son2, AST_NODE *son3) {
	AST_NODE *node = (AST_NODE*) calloc(1, sizeof(AST_NODE));

	node->type = type;
	node->symbol = symbol;
	node->son[0] = son0;
	node->son[1] = son1;
	node->son[2] = son2;
	node->son[3] = son3;

	return node;
}

void astFormat(int spaces) {
	if(spaces > 0) {
		int i;
		for(i=0; i<spaces-1; i++)
			printf("    ");
		printf("\u2502\n");
		for(i=0; i<spaces-1; i++)
			printf("    ");
		printf("\u2514\u2500\u2500 ");
	}
}

void astPrint(AST_NODE *node) {
	printf("\nABSTRACT SYNTAX TREE:\n\n");
	astShow(node, 0);
	printf("\n");
}

void astShow(AST_NODE *node, int spaces) {

	astFormat(spaces);

	switch(node->type) {
		case AST_SYMBOL: printf("AST_SYMBOL: ");     break;
		case AST_ADD:    printf("AST_ADD\n");        break;
		case AST_SUB:    printf("AST_SUB\n");        break;
		case AST_MUL:    printf("AST_MUL\n");        break;
		case AST_DIV:    printf("AST_DIV\n");        break;
		default:         printf("AST_UNKNOWN\n");    break;
	}

	if(node-> symbol != NULL) {
		printf("%s\n", node->symbol->text);
	}

	int i;
	for(i=0; i<MAX_SONS; i++) {
		if(node->son[i] != NULL)
			astShow(node->son[i], spaces+1);
	}
}