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
	checkAndSetDeclarations(node);
	// Verifica se algum identificador usado no código não foi declarado
	checkUndeclared(node);
	// Verifica os tipos de dados para expressões, percorrendo recursivamente as operações de baixo pra cima e anotando valor nos nodos
	checkExprTypes(node);
	// Verifica se o uso dos identificadores está compatível com sua declaração
	checkUsage(node);

	// Verifica se erros semânticos foram encontrados. Caso sim, retorna 4 conforme especificação do trabalho
	if(SEMANTIC_ERRORS > 0) {
		exit(4);
	}
}


// Especifica semanticamente o tipo do identificador nas declarações de variáveis e funções, e define seu tipo (datatype)
void checkAndSetDeclarations(AST_NODE *node) {
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
			checkAndSetDeclarations(node->son[i]);
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


// Verifica os tipos de dados para expressões, percorrendo recursivamente as operações de baixo pra cima e anotando valor nos nodos
void checkExprTypes(AST_NODE *node) {
	// Se for operador booleano unário (só tem o NOT)
	if(node->type == AST_NOT) {
		checkBooleanSon(jumpParenthesis(node->son[0]), "\b");        // verifica único filho do NOT
	}
	// Se for operador booleano binário (todos eles vão ter a mesma avaliação de expressões)
	else if(isBooleanOperator(node)) {
		checkBooleanSon(jumpParenthesis(node->son[0]), "left");     // verifica filho da esquerda
		checkBooleanSon(jumpParenthesis(node->son[1]), "right");    // verifica filho da direita
	}
	// Se for um nodo de operador numérico (todos eles vão ter a mesma avaliação de expressões)
	else if(isNumericOperator(node)) {
		checkNumericSon(jumpParenthesis(node->son[0]), "left");     // verifica filho da esquerda
		checkNumericSon(jumpParenthesis(node->son[1]), "right");    // verifica filho da direita
	}

	// Verifica os nodos filhos
	int i;
	for(i=0; i<MAX_SONS; i++) {
		if(node->son[i] != NULL) {
			checkExprTypes(node->son[i]);
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

	// TODO: verificação do índice do vetor como inteiro tanto na atribuição como no uso em expressões
	// TODO: Atribuições... (escalar e vetor)

	// Verifica os nodos filhos
	int i;
	for(i=0; i<MAX_SONS; i++) {
		if(node->son[i] != NULL) {
			checkUsage(node->son[i]);
		}
	}
}


// Função que analisa uso de tipo correto dos filhos de operadores aritméticos
void checkNumericSon(AST_NODE *node, char *sonSide) {
	// Se não for outro operador numérico
	if(!isNumericOperator(node)) {
		// Se não for um literal numérico compatível (CHAR, INTEGER, FLOAT)
		if(!isNumericLiteral(node)) {
			// Se não for identificador de tipo numérico compatível (função, scalar e vetor)
			if(!isNumericIdentifier(node)) {
					fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
					fprintf(stderr, "-------> Invalid %s operand for arithmetic operator.\n", sonSide);
					SEMANTIC_ERRORS++;
			}
		}
	}
}


// Função que analisa uso de tipo correto dos filhos de operadores booleanos
void checkBooleanSon(AST_NODE *node, char *sonSide) {
	// Se não for outro operador booleano
	if(!isBooleanOperator(node)) {
		// Se não for um literal booleano compatível (TRUE, FALSE)
		if(!isBooleanLiteral(node)) {
			// Se não for identificador de tipo numérico compatível (função, scalar e vetor)
			if(!isBooleanIdentifier(node)) {
					fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
					fprintf(stderr, "-------> Invalid %s operand for boolean operator.\n", sonSide);
					SEMANTIC_ERRORS++;
			}
		}
	}
}


// Verifica se o nodo é um dos nodos que representam operações numéricas
int isNumericOperator(AST_NODE *node) {
	if(node->type == AST_ADD)
		return 1;
	if(node->type == AST_SUB)
		return 1;
	if(node->type == AST_MUL)
		return 1;
	if(node->type == AST_DIV)
		return 1;
	if(node->type == AST_LT)
		return 1;
	if(node->type == AST_GT)
		return 1;
	if(node->type == AST_LE)
		return 1;
	if(node->type == AST_GE)
		return 1;
	return 0;
}


// Verifica se o nodo é um dos nodos que representam operações booleanas
int isBooleanOperator(AST_NODE *node) {
	if(node->type == AST_EQ)
		return 1;
	if(node->type == AST_DIF)
		return 1;
	if(node->type == AST_XOR)
		return 1;
	if(node->type == AST_OR)
		return 1;
	if(node->type == AST_NOT)
		return 1;
	return 0;
}


// Verifica se o nodo é um literal numérico compatível (CHAR, INTEGER, FLOAT)
int isNumericLiteral(AST_NODE *node) {
	if(node->symbol != NULL) {
		if(node->symbol->type == SYMBOL_LIT_CHAR)
			return 1;
		if(node->symbol->type == SYMBOL_LIT_INTEGER)
			return 1;
		if(node->symbol->type == SYMBOL_LIT_FLOAT)
			return 1;
	}
	return 0;
}


// Verifica se o nodo é um literal booleano compatível (TRUE, FALSE)
int isBooleanLiteral(AST_NODE *node) {
	if(node->symbol != NULL) {
		if(node->symbol->type == SYMBOL_LIT_TRUE)
			return 1;
		if(node->symbol->type == SYMBOL_LIT_FALSE)
			return 1;
	}
	return 0;
}


// Verifica se o nodo é um identificador (chamada de função, variável escalar ou vetor) com tipo numérico compatível
int isNumericIdentifier(AST_NODE *node) {
	if(node->symbol != NULL) {
		if(node->type == AST_FUNCALL || node->type == AST_SYMBOL || node->type == AST_VEC) {
			if(node->symbol->dataType == DATATYPE_CHAR)
				return 1;
			if(node->symbol->dataType == DATATYPE_INT)
				return 1;
			if(node->symbol->dataType == DATATYPE_FLOAT)
				return 1;
		}
	}
	return 0;
}


// Verifica se o nodo é um identificador (chamada de função, variável escalar ou vetor) com tipo booleano compatível
int isBooleanIdentifier(AST_NODE *node) {
	if(node->symbol != NULL) {
		if(node->type == AST_FUNCALL || node->type == AST_SYMBOL || node->type == AST_VEC) {
			if(node->symbol->dataType == DATATYPE_BOOL)
				return 1;
		}
	}
	return 0;
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


// Função para pegar o primeiro filho dos operadores que não é um parênthesis
AST_NODE* jumpParenthesis(AST_NODE *node) {
	if(node->type != AST_PARENTHESIS) {
		return node;
	}
	else {
		return jumpParenthesis(node->son[0]);
	}
}