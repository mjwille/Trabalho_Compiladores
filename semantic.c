/* Trabalho de Compiladores 2020/ERE
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.h"

AST_NODE *ROOT;

int SEMANTIC_ERRORS = 0;

// Função principal que chama todos os passos da análise semântica e possivelmente reporta erro semântico no fim
void semanticAnalysis(AST_NODE *node) {
	ROOT = node;
	if(node != NULL) {
		// Especifica semanticamente o tipo do identificador nas declarações de variáveis e funções, e define seu tipo (datatype)
		checkAndSetDeclarations(node);
		// Verifica se algum identificador usado no código não foi declarado
		checkUndeclared(node);
		// Verifica os tipos de dados para expressões, percorrendo recursivamente as operações de baixo pra cima e anotando valor nos nodos
		checkExprTypes(node);
		// Verifica se o uso dos identificadores está compatível com sua declaração
		checkUsage(node);
		// Verifica se o uso das funções está correto (valor de retorno, quantidade de parâmetros e tipos dos parâmetros)
		checkFunctions(node);

		// Verifica se erros semânticos foram encontrados. Caso sim, retorna 4 conforme especificação do trabalho
		if(SEMANTIC_ERRORS > 0) {
			exit(4);
		}
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

	// Verifica de forma básica (não estava no enunciado) os parâmetros da funções
	// (sem comparar diferentes funções, assumindo que o programador não vai repetir nome dos parâmetros entre elas)
	if(node->type == AST_PARAM) {
		// Se parâmetro já não é mais um identifier, foi também redeclarado
		if(node->symbol->type != SYMBOL_IDENTIFIER) {
			fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
			fprintf(stderr, "-------> Redeclaration of symbol '%s'.\n", node->symbol->text);
			SEMANTIC_ERRORS++;
		}
		// Senão, coloca ele pra escalar (unico tipo que um parâmetro pode assumir) e define o dataType
		else {
			node->symbol->type = SYMBOL_SCALAR;
			switch(node->son[0]->type) {
				case AST_BOOL:  node->symbol->dataType = DATATYPE_BOOL;   break;
				case AST_CHAR:  node->symbol->dataType = DATATYPE_CHAR;   break;
				case AST_INT:   node->symbol->dataType = DATATYPE_INT;    break;
				case AST_FLOAT: node->symbol->dataType = DATATYPE_FLOAT;  break;
			}
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
		checkBooleanSon(jumpParenthesis(node->son[0]), "\b", 1);        // verifica único filho do NOT
	}
	// Se for nodo de operador booleano ou numérico (EQ, DIF)
	else if(isBooleanOperator(node) && isNumericOperator(node)) {
		// os 2 nodos filhos podem ser qualquer tipo, contanto que os 2 sejam de tipos compatíveis entre sí (iguais em tipo)
		int leftSon  = checkBooleanSon(jumpParenthesis(node->son[0]), "left",   0);     // verifica filho da esquerda
		int rightSon = checkBooleanSon(jumpParenthesis(node->son[1]), "right",  0);     // verifica filho da direita
		if(leftSon != rightSon) {
			fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
			fprintf(stderr, "-------> Incompatible types in operation.\n");
			SEMANTIC_ERRORS++;
		}
	}
	// Se for nodo exclusivamente de operadores booleano binário (XOR, OR)
	else if(isBooleanOperator(node)) {
		// os 2 nodos filhos precisam ser de tipo booleano ou de operador que resulta em um valor booleano
		checkBooleanSon(jumpParenthesis(node->son[0]), "left",  1);     // verifica filho da esquerda
		checkBooleanSon(jumpParenthesis(node->son[1]), "right", 1);     // verifica filho da direita
	}
	// Se for nodo exclusivamente de operadores exclusivamente numérico (ADD, SUB, MUL, DIV, LT, GT, LE, GE)
	else if(isNumericOperator(node)) {
		// os 2 nodos filhos precisam ser de tipo numérico ou de operador que resulta em um valor numérico
		checkNumericSon(jumpParenthesis(node->son[0]), "left",  1);     // verifica filho da esquerda
		checkNumericSon(jumpParenthesis(node->son[1]), "right", 1);     // verifica filho da direita
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
	// Escalar usado como escalar
	if(node->type == AST_SYMBOL) {
		// Se usado como vetor
		if(node->symbol->type == SYMBOL_VECTOR) {
			fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
			fprintf(stderr, "-------> Vector '%s' used as a scalar.\n", node->symbol->text);
			SEMANTIC_ERRORS++;
		}
		// Se usado como função
		if(node->symbol->type == SYMBOL_FUNCTION) {
			fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
			fprintf(stderr, "-------> Function '%s' used as a scalar.\n", node->symbol->text);
			SEMANTIC_ERRORS++;
		}
	}

	// Vetor usado como vetor
	if(node->type == AST_VEC) {
		// Se usado como escalar
		if(node->symbol->type == SYMBOL_SCALAR) {
			fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
			fprintf(stderr, "-------> Scalar '%s' used as a vector.\n", node->symbol->text);
			SEMANTIC_ERRORS++;
		}
		// Se usado como função
		else if(node->symbol->type == SYMBOL_FUNCTION) {
			fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
			fprintf(stderr, "-------> Function '%s' used as a vector.\n", node->symbol->text);
			SEMANTIC_ERRORS++;
		}
		// Se foi usado corretamente (ie. é um vetor)
		else {
			// Verifica se o índice é um inteiro ou uma expressão que resulta em um inteiro
			checkIntegerIndex(node->son[0]);
		}
	}

	// Função usada como função
	if(node->type == AST_FUNCALL) {
		// Se usado como escalar
		if(node->symbol->type == SYMBOL_SCALAR) {
			fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
			fprintf(stderr, "-------> Scalar '%s' used as a function.\n", node->symbol->text);
			SEMANTIC_ERRORS++;
		}
		// Se usado como vetor
		if(node->symbol->type == SYMBOL_VECTOR) {
			fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
			fprintf(stderr, "-------> Vector '%s' used as a function.\n", node->symbol->text);
			SEMANTIC_ERRORS++;
		}
	}

	// Inicialização de escalar na declaração
	if(node->type == AST_DECL_VAR) {
		// Se variável é tipo numérico (CHAR, INT, FLOAT)
		if(node->symbol->dataType == DATATYPE_CHAR || node->symbol->dataType == DATATYPE_INT || node->symbol->dataType == DATATYPE_FLOAT) {
			if(!isNumericLiteral(node->son[1])) {
				fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
				fprintf(stderr, "-------> Numeric variable '%s' not initialized with numeric value.\n", node->symbol->text);
				SEMANTIC_ERRORS++;
			}
		}
		// Atribuição com tipo booleano (BOOL)
		if(node->symbol->dataType == DATATYPE_BOOL) {
			if(!isBooleanLiteral(node->son[1])) {
				fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
				fprintf(stderr, "-------> Boolean variable '%s' not initialized with boolean value.\n", node->symbol->text);
				SEMANTIC_ERRORS++;
			}
		}
	}

	// Inicialização de vetor na declaração
	if(node->type == AST_DECL_VAR_VEC) {
		// Se declaração do vetor possui valores com inicialização
		if(node->son[2] != NULL) {
			// Verifica se valores correspondem ao datatype da variável e retorna a quantidade de valores inicializados
			int initValues = checkVectorInitValues(node->son[2], node->symbol->dataType);
			// Verifica se total de valores na inicialização corresponde ao tamanho do vetor
			if(initValues != atoi(node->son[1]->symbol->text)) {
				fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
				fprintf(stderr, "-------> In vector '%s': quantity of init values differs from vector size.\n", node->symbol->text);
				SEMANTIC_ERRORS++;
			}
		}
	}

	// Atribuição de escalar
	if(node->type == AST_ATTR) {
		// Se escalar for do tipo numérico
		if(isNumericDataType(node)) {
			// Verifica se o que está tentando atribuir é compatível com numérico
			int isNumeric = checkNumericSon(node->son[0], "\b", 0);
			if(!isNumeric) {
				fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
				fprintf(stderr, "-------> Boolean assignment to numeric variable '%s'.\n", node->symbol->text);
				SEMANTIC_ERRORS++;
			}
		}
		// Se escalar for do tipo booleano
		if(isBooleanDataType(node)) {
			// Verifica se o que está tentando atribuir é compatível com booleano
			int isBoolean = checkBooleanSon(node->son[0], "\b", 0);
			if(!isBoolean) {
				fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
				fprintf(stderr, "-------> Numeric assignment to boolean variable '%s'.\n", node->symbol->text);
				SEMANTIC_ERRORS++;
			}
		}
	}

	// Atribuição de vetor
	if(node->type == AST_ATTR_VEC) {
		// Se vetor for do tipo numérico
		if(isNumericDataType(node)) {
			// Verifica se o que está tentando atribuir é compatível com numérico
			int isNumeric = checkNumericSon(node->son[1], "\b", 0);
			if(!isNumeric) {
				fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
				fprintf(stderr, "-------> Boolean assignment to numeric vector variable '%s'.\n", node->symbol->text);
				SEMANTIC_ERRORS++;
			}
		}
		// Se escalar for do tipo booleano
		if(isBooleanDataType(node)) {
			// Verifica se o que está tentando atribuir é compatível com booleano
			int isBoolean = checkBooleanSon(node->son[1], "\b", 0);
			if(!isBoolean) {
				fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
				fprintf(stderr, "-------> Numeric assignment to boolean vector variable '%s'.\n", node->symbol->text);
				SEMANTIC_ERRORS++;
			}
		}

		// Verifica também se a expressão para acesso ao índice do vetor é inteira
		checkIntegerIndex(node->son[0]);
	}

	// Verifica se condição do if é booleano
	if(node->type == AST_IF || node->type == AST_IF_ELSE) {
		// Verifica se a condição entre parênteses é booleana
		int isBoolean = checkBooleanSon(node->son[0], "\b", 0);
		if(!isBoolean) {
			fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
			fprintf(stderr, "-------> If expression should be boolean.\n");
			SEMANTIC_ERRORS++;
		}
	}

	// Verifica os nodos filhos
	int i;
	for(i=0; i<MAX_SONS; i++) {
		if(node->son[i] != NULL) {
			checkUsage(node->son[i]);
		}
	}
}


// Função que analisa uso de tipo correto dos filhos de operadores aritméticos
int checkNumericSon(AST_NODE *node, char *sonSide, int displayErrorMessage) {
	// Se não for outro operador que retorna um numérico
	if(!isNumericResultantOperator(node)) {
		// Se não for um literal numérico compatível (CHAR, INTEGER, FLOAT)
		if(!isNumericLiteral(node)) {
			// Se não for identificador de tipo numérico compatível (função, scalar e vetor)
			if(!isNumericIdentifier(node)) {
				if(displayErrorMessage) {
					fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
					fprintf(stderr, "-------> Invalid %s operand for arithmetic operator.\n", sonSide);
					SEMANTIC_ERRORS++;
				}
				return 0;
			}
		}
	}
	return 1;
}


// Função que analisa uso de tipo correto dos filhos de operadores booleanos
int checkBooleanSon(AST_NODE *node, char *sonSide, int displayErrorMessage) {
	// Se não for outro operador que retorna um booleano
	if(!isBooleanResultantOperator(node)) {
		// Se não for um literal booleano compatível (TRUE, FALSE)
		if(!isBooleanLiteral(node)) {
			// Se não for identificador de tipo numérico compatível (função, scalar e vetor)
			if(!isBooleanIdentifier(node)) {
				if(displayErrorMessage) {
					fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
					fprintf(stderr, "-------> Invalid %s operand for boolean operator.\n", sonSide);
					SEMANTIC_ERRORS++;
				}
				return 0;
			}
		}
	}
	return 1;
}


// Verifica se o uso das funções está correto (valor de retorno, quantidade de parâmetros e tipos dos parâmetros)
void checkFunctions(AST_NODE *node) {
	// Se for uma declaração de função, verifica se o valor retornado é o mesmo tipo da função (datatype)
	if(node->type == AST_DECL_FUNC) {
		checkReturnNode(node, node);
	}

	// Se for uma chamda de função, compara com a declaração da função
	if(node->type == AST_FUNCALL) {
		AST_NODE *funcDecl = getFuncDecl(ROOT, node->symbol->text);
		// Se encontrou a declaração da função
		if(funcDecl != NULL) {
			// Pega total de argumentos e parâmetros
			int totalArgs   = getTotalArgs(node->son[0]);
			int totalParams = getTotalParams(funcDecl->son[0]);
			// Se diferentes é um erro semântico
			if(totalArgs != totalParams) {
				fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
				fprintf(stderr, "-------> Invalid number of arguments in function call '%s' .\n", node->symbol->text);
				SEMANTIC_ERRORS++;
			}
			// Se iguais, compara tipos dos parâmetros com tipos dos argumentos
			else {
				compareFuncTypes(funcDecl->son[0], node->son[0]);
			}
		}
	}

	// Verifica os nodos filhos
	int i;
	for(i=0; i<MAX_SONS; i++) {
		if(node->son[i] != NULL) {
			checkFunctions(node->son[i]);
		}
	}
}


// Função para verificar se os retornos dados dentro da declaração da função são do mesmo tipo da função (datatype)
void checkReturnNode(AST_NODE *node, AST_NODE *functionNode) {
	// Se encontrou um nodo de return
	if(node->type == AST_RETURN) {
		// Se datatype da função sendo declarada é numérico
		if(isNumericDataType(functionNode)) {
			// Verifica se o que está tentando retornar é compatível com numérico
			int isNumeric = checkNumericSon(node->son[0], "\b", 0);
			if(!isNumeric) {
				fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
				fprintf(stderr, "-------> Return expression inside function '%s' is not numeric.\n", functionNode->symbol->text);
				SEMANTIC_ERRORS++;
			}
		}
		// Se datatype da função sendo declarada é booleano
		else {
			// Verifica se o que está tentando retornar é compatível com booleano
			int isBoolean = checkBooleanSon(node->son[0], "\b", 0);
			if(!isBoolean) {
				fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
				fprintf(stderr, "-------> Return expression inside function '%s' is not boolean.\n", functionNode->symbol->text);
				SEMANTIC_ERRORS++;
			}
		}
	}

	// Verifica os nodos filhos
	int i;
	for(i=0; i<MAX_SONS; i++) {
		if(node->son[i] != NULL) {
			checkReturnNode(node->son[i], functionNode);
		}
	}
}


// Função que verifica se operações e valores são inteiros para acesso a um vetor
void checkIntegerIndex(AST_NODE *node) {
	// Se for símbolo
	if(node->type == AST_SYMBOL) {
		// Ele deve ser inteiro ou char, senão é erro (índice não pode ser CHAR nem FLOAT)
		if(node->symbol->dataType != DATATYPE_INT && node->symbol->type != SYMBOL_LIT_INTEGER && node->symbol->dataType != DATATYPE_CHAR && node->symbol->type != SYMBOL_LIT_CHAR) {
			fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
			fprintf(stderr, "-------> Invalid vector index expression.\n");
			SEMANTIC_ERRORS++;
		}
	}
	// Se não for uma operação que resulta em inteiro, também é erro
	else if(!isNumericResultantOperator(node)) {
		fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
		fprintf(stderr, "-------> Invalid vector index expression.\n");
		SEMANTIC_ERRORS++;
	}

	// Verifica os nodos filhos
	int i;
	for(i=0; i<MAX_SONS; i++) {
		if(node->son[i] != NULL) {
			checkIntegerIndex(node->son[i]);
		}
	}
}


// Compara valores do vetor com tipo de inicialização do vetor e retorna o total de valores inicializados
int checkVectorInitValues(AST_NODE *node, int dataType) {
	// Condição de parada da recursão onde não achou um valor de inicialização
	if(node == NULL) {
		// Retorna 0 indicando que não encontrou valor (função retorna tatal de valore encontrados)
		return 0;
	}

	// Condição de parada da recursão onde achou um valor de inicialização
	if(node->type == AST_SYMBOL) {
		// Inicialização de vetor com tipo numérico (CHAR, INT, FLOAT)
		if(dataType == DATATYPE_CHAR || dataType == DATATYPE_INT || dataType == DATATYPE_FLOAT) {
			// Mas não é valor numérico
			if(!isNumericLiteral(node)) {
				fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
				fprintf(stderr, "-------> Numeric vector not initialized with numeric value.\n");
				SEMANTIC_ERRORS++;
			}
		}
		// Inicialização de vetor com tipo booleano (BOOL)
		if(dataType == DATATYPE_BOOL) {
			// Mas não é valor booleano
			if(!isBooleanLiteral(node)) {
				fprintf(stderr, "Line %d: Semantic Error.\n", node->lineNumber);
				fprintf(stderr, "-------> Boolean vector not initialized with boolean value.\n");
				SEMANTIC_ERRORS++;
			}
		}
		// Retorna 1 indicando que encontrou valor (função retorna tatal de valore encontrados)
		return 1;
	}

	return checkVectorInitValues(node->son[0], dataType) + checkVectorInitValues(node->son[1], dataType);
}


// Verifica se o nodo é um dos nodos que representam operações com operandos numéricos
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
	if(node->type == AST_EQ)
		return 1;
	if(node->type == AST_DIF)
		return 1;
	return 0;
}


// Verifica se o nodo é um dos nodos que representam operações com operandos booleanos
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


// Verifica se o nodo é um dos nodos que representam operações com resultados numéricos
int isNumericResultantOperator(AST_NODE *node) {
	if(node->type == AST_ADD)
		return 1;
	if(node->type == AST_SUB)
		return 1;
	if(node->type == AST_MUL)
		return 1;
	if(node->type == AST_DIV)
		return 1;
	return 0;
}


// Verifica se o nodo é um dos nodos que representam operações com resultados booleanos
int isBooleanResultantOperator(AST_NODE *node) {
	if(node->type == AST_LT)
		return 1;
	if(node->type == AST_GT)
		return 1;
	if(node->type == AST_LE)
		return 1;
	if(node->type == AST_GE)
		return 1;
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


// Verifica se o nodo é de um datatype numérico compatível (CHAR, INT, FLOAT)
int isNumericDataType(AST_NODE *node) {
	if(node->symbol->dataType == DATATYPE_CHAR)
		return 1;
	if(node->symbol->dataType == DATATYPE_INT)
		return 1;
	if(node->symbol->dataType == DATATYPE_FLOAT)
		return 1;
	return 0;
}


// Verifica se o nodo é de um datatype booleano compatível (BOOL)
int isBooleanDataType(AST_NODE *node) {
	if(node->symbol->dataType == DATATYPE_BOOL)
		return 1;
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
	// Não precisa testar nodo NULL pois, pela gramática, a expressão dentro dos parênteses não pode ser vazio
	if(node->type != AST_PARENTHESIS) {
		return node;
	}
	else {
		return jumpParenthesis(node->son[0]);
	}
}

// Função que percorre a AST buscando a declaração da função com mesmo nome
AST_NODE* getFuncDecl(AST_NODE *node, char *funcName) {

	AST_NODE *funcDecl;

	// Se for uma declaração de função
	if(node->type == AST_DECL_FUNC) {
		// Com mesmo nome do que na chamada da função
		if(!strcmp(funcName, node->symbol->text)) {
			return node;
		}
	}
	// Verifica os nodos filhos
	int i;
	for(i=0; i<MAX_SONS; i++) {
		if(node->son[i] != NULL) {
			funcDecl = getFuncDecl(node->son[i], funcName);
			// Se achou a declaração na recursão, retorna
			if(funcDecl != NULL)
				return funcDecl;
		}
	}

	// Se não é declaração de função e já passou por todos os filhos
	return NULL;
}


// Função que pega o total de argumentos passados na chamada de função
int getTotalArgs(AST_NODE *node) {
	if(node == NULL) {
		return 0;
	}
	if(node->type != AST_ARGS) {
		return 1;
	}
	return getTotalArgs(node->son[0]) + getTotalArgs(node->son[1]);
}


// Função que pega o total de parâmetros da declaração de uma função
int getTotalParams(AST_NODE *node) {
	if(node == NULL) {
		return 0;
	}
	if(node->type != AST_PARAMS) {
		return 1;
	}
	return getTotalParams(node->son[0]) + getTotalParams(node->son[1]);
}


// Função que compara tipos dos parâmetros da definição da função com os tipos dos argumentos da chamada da função
void compareFuncTypes(AST_NODE *funcdefNode, AST_NODE *funcallNode) {
	if(funcallNode == NULL) {
		return;
	}
	if(funcallNode->type != AST_ARGS) {
		// Tanto o parâmetro como o argumento precisam ser iguais em tipos compatíveis
		AST_NODE *paramNode = funcdefNode->son[0];
		// Se parâmetro é booleano
		if(paramNode->type == AST_BOOL) {
			// Verifica se argumento é booleano também
			int arg   = checkBooleanSon(jumpParenthesis(funcallNode), "",   0);
			if(!arg) {
				fprintf(stderr, "Line %d: Semantic Error.\n", funcallNode->lineNumber);
				fprintf(stderr, "-------> Invalid argument type in function call.\n");
				SEMANTIC_ERRORS++;
			}
		}
		// Se parâmetro é numérico
		else {
			// Verifica se argumento é numérico também
			int arg   = checkNumericSon(jumpParenthesis(funcallNode), "",   0);
			if(!arg) {
				fprintf(stderr, "Line %d: Semantic Error.\n", funcallNode->lineNumber);
				fprintf(stderr, "-------> Invalid argument type in function call.\n");
				SEMANTIC_ERRORS++;
			}
		}
		// Retorna, não indo para os filhos pois não tem nada
		return;
	}

	// Vai andando junto na AST dos argumentos e na AST dos parâmetros
	compareFuncTypes(funcdefNode->son[0], funcallNode->son[0]);
	compareFuncTypes(funcdefNode->son[1], funcallNode->son[1]);
}