/* Trabalho de Compiladores 2020/1
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
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

void astPrint(AST_NODE *node) {
	printf("\nABSTRACT SYNTAX TREE:\n\n");
	astShow(node, 0, 0, 0);
	printf("\n");
}

void astFormat(int spaces, int bar, int last) {
	if(spaces > 0) {
		int i, j, aux;
		for(i=0; i<=spaces; i++) {
			aux = 1 << (i);
			if((bar & aux) == aux)
				printf("   \u2502");
			else
				printf("    ");
		}
		if(last)
			printf("\b\b\b\b\b\u2502\n");
		else
			printf("\n");
		for(i=0; i<spaces; i++) {
			aux = 1 << (i);
			if((bar & aux) == aux)
				printf("   \u2502");
			else
				printf("    ");
		}
		printf("\b\u2514\u2500\u2500 ");
	}
}

int hasAnotherSon(AST_NODE *node, int i) {
	int j;
	for(j=i; j<MAX_SONS; j++) {
		if(node->son[j] != NULL)
			return 1;
	}
	return 0;
}

void astShow(AST_NODE *node, int spaces, int bar, int last) {

	astFormat(spaces, bar, last);

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
		if(node->son[i] != NULL) {
			if(hasAnotherSon(node, i+1)) {
				bar += pow(2, spaces);
				astShow(node->son[i], spaces+1, bar, 0);
			}
			else {
				astShow(node->son[i], spaces+1, bar, 1);
			}
			bar -= pow(2, spaces);
		}
	}
}