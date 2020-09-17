/* Trabalho de Compiladores 2020/1
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ast.h"


// Insere nodo na AST dando seu tipo, o símbolo (que é o próprio nodo na tabela hash) e os nodos filhos
AST_NODE *astInsert(int type, HASH_NODE *symbol, AST_NODE *son0, AST_NODE *son1, AST_NODE *son2, AST_NODE *son3) {
	// Faz alocação em memória do nodo
	AST_NODE *node = (AST_NODE*) calloc(1, sizeof(AST_NODE));
	// Coloca dentro do nodo alocado os seus respectivos valores
	node->type = type;            // o tipo definido em ast.h
	node->symbol = symbol;        // o símbolo (ponteiro para um nodo na tabela hash)
	// os filhos
	node->son[0] = son0;
	node->son[1] = son1;
	node->son[2] = son2;
	node->son[3] = son3;
	// retorna o nodo para ser atribuído para $$ no parser, para depois ser atribuído a outro $$ em outra regra gramatical
	return node;
}


// Imprime toda a AST
void astPrint(AST_NODE *node) {
	printf("\nABSTRACT SYNTAX TREE:\n\n");
	// chama a função recursiva para a impressão da árvore abstrata
	astShow(node, 0, 0, 0);
	printf("\n");
}


// Função auxiliar para formatar a árvore durante a impressão da AST
void astFormat(int spaces, int bar, int last) {
	// Se não faz parte da primeira coluna (caso onde teria espaços para printar)
	if(spaces > 0) {
		int i, j, aux;
		// primeira linha a ser impressa
		for(i=0; i<=spaces; i++) {
			// vai pegando as potências de 2 (pra ver se um nodo ainda tem filhos)
			aux = 1 << (i);
			// se 1 na posição i, é porque precisa imprimir '|' pois ainda tem filhos mais para baixo na árvore
			if((bar & aux) == aux)
				printf("   \u2502");     // imprime '    |'
			else
				printf("    ");          // imprime '     '
		}

		// caso especial em que o nodo é o último do pai na árvore
		if(last) {
			printf("\b\b\b\b\b\u2502\n");
		}
		else {
			printf("\n");
		}

		// segunda linha a ser impressa
		for(i=0; i<spaces; i++) {
			// vai pegando as potências de 2 (pra ver se um nodo ainda tem filhos)
			aux = 1 << (i);
			// se 1 na posição i, é porque precisa imprimir '|' pois ainda tem filhos mais para baixo na árvore
			if((bar & aux) == aux)
				printf("   \u2502");      // imprime '    |'
			else
				printf("    ");          // imprime '     '
		}
		// por fim, chegou no nodo e imprime '-- ', com o tipo do nodo vindo a seguir
		printf("\b\u2514\u2500\u2500 ");
	}
}


// Faz check se o nodo possui outro filho além do índice 'i' passado (um nodo pode ter no máximo 4 filhos)
int hasAnotherSon(AST_NODE *node, int i) {
	int j;
	// percorre os filhos restantes para além do índice do filho atual passado como parâmetro
	for(j=i; j<MAX_SONS; j++) {
		if(node->son[j] != NULL)
			return 1;
	}
	return 0;
}


// Função auxiliar para fazer a impressão (esta é a função recursiva de impressão que é chamada por astPrint)
void astShow(AST_NODE *node, int spaces, int bar, int last) {

	// imprime os espaços e as barras
	astFormat(spaces, bar, last);

	// imprime o tipo do nodo
	switch(node->type) {
		case AST_SYMBOL:   printf("AST_SYMBOL: ");     break;
		case AST_ADD:      printf("AST_ADD\n");        break;
		case AST_SUB:      printf("AST_SUB\n");        break;
		case AST_MUL:      printf("AST_MUL\n");        break;
		case AST_DIV:      printf("AST_DIV\n");        break;
		case AST_LT:       printf("AST_LT\n");         break;
		case AST_GT:       printf("AST_GT\n");         break;
		case AST_LE:       printf("AST_LE\n");         break;
		case AST_GE:       printf("AST_GE\n");         break;
		case AST_EQ:       printf("AST_EQ\n");         break;
		case AST_DIF:      printf("AST_DIF\n");        break;
		case AST_XOR:      printf("AST_XOR\n");        break;
		case AST_OR:       printf("AST_OR\n");         break;
		case AST_NOT:      printf("AST_NOT\n");        break;
		case AST_FUNCALL:  printf("AST_FUNCALL: ");    break;    // tem também símbolo na tabela hash (nome da função)
		case AST_ARG:      printf("AST_ARG\n");        break;
		case AST_RETURN:   printf("AST_RETURN\n");     break;
		default:           printf("AST_UNKNOWN\n");    break;
	}

	// se é um símbolo na tabela hash (tabela de símbolos), imprime ainda seu conteúdo texto
	if(node-> symbol != NULL) {
		printf("%s\n", node->symbol->text);
	}

	int i;
	// percorre todos os filhos do nodo atual
	for(i=0; i<MAX_SONS; i++) {
		// se filho neste índice existe
		if(node->son[i] != NULL) {
			if(hasAnotherSon(node, i+1)) {
				// codifica em potências de 2 que nodo atual ainda tem filho além desse (para continuar a imprimir as barras)
				bar += pow(2, spaces);
				// chama o print para filho
				astShow(node->son[i], spaces+1, bar, 0);
			}
			else {
				// chama o print para o último filho (1 no último argumento é o parâmetro 'last')
				astShow(node->son[i], spaces+1, bar, 1);
			}
			// desfaz a codificação para este nodo (pois o próximo pode ser o último)
			bar -= pow(2, spaces);
		}
	}
}

/*  Como funciona a impressão da árvore?
 *
 * spaces -> controlam quanto de espaço precisa printar na mesma linha (o nível na árvore)
 * bar -> é a codificação em potências de 2.
 *
 * Se bar = 13 (1101) significa que as barras printadas são | x | | (x indica ausência de |)
 * Assim, dentro do astFormat, é possível saber, quando desce, quais barras precisa printar acessando
 * os bits.
 *
 * O last serve como correção de 'off-by-one-error', já que seria o último filho e o parâmetro
 * bar não seria atualizado somente para aquele nodo específico, não imprimindo nenhuma barra
 * acima do nodo final.
 */