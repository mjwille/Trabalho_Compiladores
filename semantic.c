/* Trabalho de Compiladores 2020/ERE
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.h"

int SEMANTIC_ERRORS = 0;

// Função principal que chama todos os passos da análise semântica e possivelmente reporta erro semântico no fim
void semanticAnalysis(AST_NODE *node) {
	// Verifica se identificadores foram declarados, colocando o datatype do identificador caso sim.
	checkDeclarations(node);
	// Verifica se erros semânticos foram encontrados. Caso sim, retorna 4 conforme especificação do trabalho
	if(SEMANTIC_ERRORS > 0) {
		exit(4);
	}
}

// Verifica se identificadores foram declarados, colocando o datatype do identificador caso sim
void checkDeclarations(AST_NODE *node) {
	// Se nodo for um dos nodos de identificadores
	if(node->type == AST_DECL_FUNC) {
		// É um identificador mas não tem tipo SYMBOL_IDENTIFIER, significa que já encontrou o identificador e alterou
		// o tipo dele na hash para um tipo que diz mais sobre o símbolo (SCALAR, VECTOR, FUNCTION) e portanto redeclarou.
		if(node->symbol->type != SYMBOL_IDENTIFIER) {
			fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
			fprintf(stderr, "-------> Redeclaration of symbol '%s'.\n", node->symbol->text);
			SEMANTIC_ERRORS++;
		}
		else {
			// Altera símbolo do identificador para algo que melhor represente a semântica dele no código
			node->symbol->type = SYMBOL_FUNCTION;
			// Verifica na AST o seu tipo de dado (ie. se variável é inteira, se função retorna float, ...)
			if(node->son[1]->type == AST_BOOL)
				node->symbol->datatype = DATATYPE_BOOL;
			if(node->son[1]->type == AST_CHAR)
				node->symbol->datatype = DATATYPE_CHAR;
			if(node->son[1]->type == AST_INT)
				node->symbol->datatype = DATATYPE_INT;
			if(node->son[1]->type == AST_FLOAT)
				node->symbol->datatype = DATATYPE_FLOAT;
		}
	}
	// Verifica os nodos filhos
	int i;
	for(i=0; i<MAX_SONS; i++) {
		if(node->son[i] != NULL) {
			checkDeclarations(node->son[i]);
		}
	}
}