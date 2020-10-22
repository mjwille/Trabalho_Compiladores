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
	// Especifica semanticamente o tipo do identificador nas declarações de variáveis e funções, e define seu tipo (datatype)
	redefineIdentifiers(node);
	// Verifica se algum identificador usado no código não foi declarado
	// checkUndeclared(node);
	// Verifica se o uso dos identificadores está compatível com sua declaração
	// checkUsage(node);
	// Verifica se erros semânticos foram encontrados. Caso sim, retorna 4 conforme especificação do trabalho
	if(SEMANTIC_ERRORS > 0) {
		exit(4);
	}
}


// Verifica se o nodo é um dos nodos que declaram variável ou função
int isDeclaration(AST_NODE *node) {
	if(node->type == AST_DECL_FUNC)
		return 1;
	if(node->type == AST_DECL_VAR)
		return 1;
	if(node->type == AST_DECL_VAR_VEC)
		return 1;
	return 0;
}


// Especifica semanticamente o tipo do identificador nas declarações de variáveis e funções, e define seu tipo (datatype)
void redefineIdentifiers(AST_NODE *node) {
	// Se nodo for um dos nodos de declaração
	if(isDeclaration(node)) {
		// e tipo dele é ainda o tipo genético "identifier" (sem semântica nenhuma associada a ele)
		if(node->symbol->type == SYMBOL_IDENTIFIER) {
			// Altera tipo do identificador para algo que melhor represente a semântica dele no código e pega da AST o datatype
			if(node->type == AST_DECL_FUNC) {
				node->symbol->type = SYMBOL_FUNCTION;
				switch(node->son[1]->type) {
					case AST_BOOL:  node->symbol->dataType = DATATYPE_BOOL;   break;
					case AST_CHAR:  node->symbol->dataType = DATATYPE_CHAR;   break;
					case AST_INT:   node->symbol->dataType = DATATYPE_INT;    break;
					case AST_FLOAT: node->symbol->dataType = DATATYPE_FLOAT;  break;
					// TODO: verificar qtd de parâmetros pra comparar depois na chamada, colocando eles pra SCALAR e ver se não tem iguais
				}
			}
			if(node->type == AST_DECL_VAR) {
				node->symbol->type = SYMBOL_SCALAR;
				switch(node->son[0]->type) {
					case AST_BOOL:  node->symbol->dataType = DATATYPE_BOOL;   break;
					case AST_CHAR:  node->symbol->dataType = DATATYPE_CHAR;   break;
					case AST_INT:   node->symbol->dataType = DATATYPE_INT;    break;
					case AST_FLOAT: node->symbol->dataType = DATATYPE_FLOAT;  break;
				}
			}
			if(node->type == AST_DECL_VAR_VEC) {
				node->symbol->type = SYMBOL_VECTOR;
				switch(node->son[0]->type) {
					case AST_BOOL:  node->symbol->dataType = DATATYPE_BOOL;   break;
					case AST_CHAR:  node->symbol->dataType = DATATYPE_CHAR;   break;
					case AST_INT:   node->symbol->dataType = DATATYPE_INT;    break;
					case AST_FLOAT: node->symbol->dataType = DATATYPE_FLOAT;  break;
				}
			}
		}
		// Senão, achou uma declaração com tipo diferente de "identifier". Significa que já encontrou esse identificador
		// antes em uma declaração e o tipo dele já foi mudado. Portanto, houve uma redeclaração do mesmo símbolo.
		else {
			fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
			fprintf(stderr, "-------> Redeclaration of symbol '%s'.\n", node->symbol->text);
			SEMANTIC_ERRORS++;
		}
	}

	// Verifica os nodos filhos
	int i;
	for(i=0; i<MAX_SONS; i++) {
		if(node->son[i] != NULL) {
			redefineIdentifiers(node->son[i]);
		}
	}
}


// Verifica se algum identificador usado no código não foi declarado
void checkUndeclared(AST_NODE *node) {
	/*
	 * Se sobrar algum identificador na tabela hash com tipo SYMBOL_IDENTIFIER, não foi declarado pois
	 * a redefinição de tipos dos identificadores de declarações feito anteriormente deveria ter alterado.
	 */

	// se possue ponteiro para tabela de símbolos
	if(node->symbol != NULL) {
		// e símbolo é SYMBOL_IDENTIFIER
		if(node->symbol->type == SYMBOL_IDENTIFIER) {
			fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
			fprintf(stderr, "-------> '%s' was not declared.\n", node->symbol->text);
			SEMANTIC_ERRORS++;
		}
	}
	// Verifica os nodos filhos
	int i;
	for(i=0; i<MAX_SONS; i++) {
		if(node->son[i] != NULL) {
			checkUndeclared(node->son[i]);
		}
	}
}


// Verifica se o uso dos identificadores está compatível com sua declaração
void checkUsage(AST_NODE *node) {
	// Vetor usado como vetor
	if(node->type == AST_SYMBOL) {
		if(node->symbol->type == SYMBOL_VECTOR) {
			fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
			fprintf(stderr, "-------> Vector '%s' used as a scalar.\n", node->symbol->text);
			SEMANTIC_ERRORS++;
		}
		if(node->symbol->type == SYMBOL_FUNCTION) {
			fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
			fprintf(stderr, "-------> Function '%s' used as a scalar.\n", node->symbol->text);
			SEMANTIC_ERRORS++;
		}
	}
	// Vetor usado como vetor
	if(node->type == AST_VEC) {
		if(node->symbol->type == SYMBOL_SCALAR) {
			fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
			fprintf(stderr, "-------> Scalar '%s' used as a vector.\n", node->symbol->text);
			SEMANTIC_ERRORS++;
		}
		if(node->symbol->type == SYMBOL_FUNCTION) {
			fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
			fprintf(stderr, "-------> Function '%s' used as a vector.\n", node->symbol->text);
			SEMANTIC_ERRORS++;
		}
	}
	// Função usada como função
	if(node->type == AST_FUNCALL) {
		if(node->symbol->type == SYMBOL_SCALAR) {
			fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
			fprintf(stderr, "-------> Scalar '%s' used as a function.\n", node->symbol->text);
			SEMANTIC_ERRORS++;
		}
		if(node->symbol->type == SYMBOL_VECTOR) {
			fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
			fprintf(stderr, "-------> Vector '%s' used as a function.\n", node->symbol->text);
			SEMANTIC_ERRORS++;
		}
	}

	// TODO: Atribuições... (escalar e vetor)

	// Verifica os nodos filhos
	int i;
	for(i=0; i<MAX_SONS; i++) {
		if(node->son[i] != NULL) {
			checkUsage(node->son[i]);
		}
	}
}