/* Trabalho de Compiladores 2020/ERE
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ast.h"

extern int getLineNumber(void);

// Insere nodo na AST dando seu tipo, o símbolo (que é o próprio nodo na tabela hash) e os nodos filhos
AST_NODE *astInsert(int type, HASH_NODE *symbol, AST_NODE *son0, AST_NODE *son1, AST_NODE *son2, AST_NODE *son3) {
	// Faz alocação em memória do nodo
	AST_NODE *node = (AST_NODE*) calloc(1, sizeof(AST_NODE));
	// Coloca dentro do nodo alocado os seus respectivos valores
	node->type = type;                    // o tipo definido em ast.h
	node->symbol = symbol;                // o símbolo (ponteiro para um nodo na tabela hash)
	node->lineNumber = getLineNumber();   // a linha em que o símbolo ocorre
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
	printf("\nAbstract Syntax Tree:\n\n");
	// chama a função que imprime nodo a nodo recursivamente a AST (se tiver algum nodo)
	if(node != NULL) {
		astPrintNode(node, 0, 0, 0);
	}
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
void astPrintNode(AST_NODE *node, int spaces, int bar, int last) {

	// imprime os espaços e as barras
	astFormat(spaces, bar, last);

	// imprime o tipo do nodo
	switch(node->type) {
		case AST_SYMBOL:       printf("AST_SYMBOL: ");        break;  // tem símbolo na tabela hash (nome do identificador)
		case AST_VEC:          printf("AST_VEC: ");           break;  // tem também símbolo na tabela hash (nome do identificador do vetor)
		case AST_ADD:          printf("AST_ADD\n");           break;
		case AST_SUB:          printf("AST_SUB\n");           break;
		case AST_MUL:          printf("AST_MUL\n");           break;
		case AST_DIV:          printf("AST_DIV\n");           break;
		case AST_LT:           printf("AST_LT\n");            break;
		case AST_GT:           printf("AST_GT\n");            break;
		case AST_LE:           printf("AST_LE\n");            break;
		case AST_GE:           printf("AST_GE\n");            break;
		case AST_EQ:           printf("AST_EQ\n");            break;
		case AST_DIF:          printf("AST_DIF\n");           break;
		case AST_XOR:          printf("AST_XOR\n");           break;
		case AST_OR:           printf("AST_OR\n");            break;
		case AST_NOT:          printf("AST_NOT\n");           break;
		case AST_PARENTHESIS:  printf("AST_PARENTHESIS\n");   break;
		case AST_FUNCALL:      printf("AST_FUNCALL: ");       break;    // tem também símbolo na tabela hash (nome da função)
		case AST_ARGS:         printf("AST_ARGS\n");          break;
		case AST_ATTR:         printf("AST_ATTR: ");          break;    // tem também símbolo na tabela hash (nome do identificador do lado esquerdo da atribuição)
		case AST_ATTR_VEC:     printf("AST_ATTR_VEC: ");      break;    // tem também símbolo na tabela hash (nome do identificador do vetor do lado esquerdo da atribuição)
		case AST_CMD:          printf("AST_CMD\n");           break;
		case AST_READ:         printf("AST_READ: ");          break;    // tem também símbolo na tabela hash (identificador para onde será lido o valor)
		case AST_PRINT:        printf("AST_PRINT\n");         break;
		case AST_PRINT_LIST:   printf("AST_PRINT_LIST\n");    break;
		case AST_RETURN:       printf("AST_RETURN\n");        break;
		case AST_IF:           printf("AST_IF\n");            break;
		case AST_IF_ELSE:      printf("AST_IF_ELSE\n");       break;
		case AST_WHILE:        printf("AST_WHILE\n");         break;
		case AST_LOOP:         printf("AST_LOOP: ");          break;    // tem também símbolo na tabela hash (identificador do controle do laço)
		case AST_BLOCK:        printf("AST_BLOCK\n");         break;
		case AST_PARAMS:       printf("AST_PARAMS\n");        break;
		case AST_PARAM:        printf("AST_PARAM: ");         break;    // tem também símbolo na tabela hash (identificador que é o nome do parâmetro)
		case AST_DECL_FUNC:    printf("AST_DECL_FUNC: ");     break;    // tem também símbolo na tabela hash (identificador que é o nome da função)
		case AST_BOOL:         printf("AST_BOOL\n");          break;
		case AST_CHAR:         printf("AST_CHAR\n");          break;
		case AST_INT:          printf("AST_INT\n");           break;
		case AST_FLOAT:        printf("AST_FLOAT\n");         break;
		case AST_DECL_VAR:     printf("AST_DECL_VAR: ");      break;    // tem também símbolo na tabela hash (identificador que é o nome da variável declarada)
		case AST_DECL_VAR_VEC: printf("AST_DECL_VAR_VEC: ");  break;    // tem também símbolo na tabela hash (identificador que é o nome do vetor declarado)
		case AST_VEC_VAL:      printf("AST_VEC_VAL\n");       break;
		case AST_DECL:         printf("AST_DECL\n");          break;
		default:               printf("AST_UNKNOWN\n");       break;
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
				astPrintNode(node->son[i], spaces+1, bar, 0);
			}
			else {
				// chama o print para o último filho (1 no último argumento é o parâmetro 'last')
				astPrintNode(node->son[i], spaces+1, bar, 1);
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


// Função para avaliação da etapa3, que descompila a AST em código novamente
void decompile(AST_NODE *node) {
	// escreve no arquivo de saída meta informação como comentário
	fprintf(outputFile, "/* Arquivo gerado como resultado\n * da descompilação da AST\n * do programa original.\n */\n\n");
	// faz a descompilação recursivamente nodo a nodo da AST (se tiver algum nodo)
	if(node != NULL) {
		decompileNode(node);
	}
	// escreve no arquivo de saída comentário indicando que descompilação terminou com sucesso
	fprintf(outputFile, "\n/* Descompilação terminada com sucesso. */\n");
}


// Função que joga no arquivo de saída a decompilação nodo a nodo da AST gerada durante a compilação
void decompileNode(AST_NODE *node) {
	int i;
	// para cada tipo de nodo na AST, faz caminho reverso e gera o código de acordo com a linguagem
	switch(node->type) {
		case AST_SYMBOL:
			fprintf(outputFile, "%s", node->symbol->text);
			break;
		case AST_VEC:
			fprintf(outputFile, "%s", node->symbol->text);
			fprintf(outputFile, "[");
			decompileNode(node->son[0]);    // expr
			fprintf(outputFile, "]");
			break;
		case AST_ADD:
			decompileNode(node->son[0]);    // expr
			fprintf(outputFile, " + ");
			decompileNode(node->son[1]);    // expr
			break;
		case AST_SUB:
			decompileNode(node->son[0]);    // expr
			fprintf(outputFile, " - ");
			decompileNode(node->son[1]);    // expr
			break;
		case AST_MUL:
			decompileNode(node->son[0]);    // expr
			fprintf(outputFile, " * ");
			decompileNode(node->son[1]);    // expr
			break;
		case AST_DIV:
			decompileNode(node->son[0]);    // expr
			fprintf(outputFile, " / ");
			decompileNode(node->son[1]);    // expr
			break;
		case AST_LT:
			decompileNode(node->son[0]);    // expr
			fprintf(outputFile, " < ");
			decompileNode(node->son[1]);    // expr
			break;
		case AST_GT:
			decompileNode(node->son[0]);    // expr
			fprintf(outputFile, " > ");
			decompileNode(node->son[1]);    // expr
			break;
		case AST_LE:
			decompileNode(node->son[0]);    // expr
			fprintf(outputFile, " <= ");
			decompileNode(node->son[1]);    // expr
			break;
		case AST_GE:
			decompileNode(node->son[0]);    // expr
			fprintf(outputFile, " >= ");
			decompileNode(node->son[1]);    // expr
			break;
		case AST_EQ:
			decompileNode(node->son[0]);    // expr
			fprintf(outputFile, " == ");
			decompileNode(node->son[1]);    // expr
			break;
		case AST_DIF:
			decompileNode(node->son[0]);    // expr
			fprintf(outputFile, " != ");
			decompileNode(node->son[1]);    // expr
			break;
		case AST_XOR:
			decompileNode(node->son[0]);    // expr
			fprintf(outputFile, " ^ ");
			decompileNode(node->son[1]);    // expr
			break;
		case AST_OR:
			decompileNode(node->son[0]);    // expr
			fprintf(outputFile, " | ");
			decompileNode(node->son[1]);    // expr
			break;
		case AST_NOT:
			fprintf(outputFile, "~ ");
			decompileNode(node->son[0]);    // expr
			break;
		case AST_PARENTHESIS:
			fprintf(outputFile, "(");
			decompileNode(node->son[0]);    // expr
			fprintf(outputFile, ")");
			break;
		case AST_FUNCALL:
			fprintf(outputFile, "%s", node->symbol->text);
			fprintf(outputFile, "(");
			// verifica se a função tem argumentos passados na chamada
			if(node->son[0] != NULL) {
				decompileNode(node->son[0]);    // arguments_list
			}
			fprintf(outputFile, ")");
			break;
		case AST_ARGS:
			for(i=0; i<MAX_SONS; i++) {
				if(node->son[i] != NULL) {
					if(hasAnotherSon(node, i+1)) {
						decompileNode(node->son[i]);     // expr
						fprintf(outputFile, ", ");
					}
					else {
						decompileNode(node->son[i]);     // expr
					}
				}
			}
			break;
		case AST_ATTR:
			fprintf(outputFile, "%s", node->symbol->text);
			fprintf(outputFile, " = ");
			decompileNode(node->son[0]);     // expr
			break;
		case AST_ATTR_VEC:
			fprintf(outputFile, "%s", node->symbol->text);
			fprintf(outputFile, "[");
			decompileNode(node->son[0]);     // expr
			fprintf(outputFile, "] = ");
			decompileNode(node->son[1]);     // expr
			break;
		case AST_CMD:
			if(node->son[0] != NULL) {
				decompileNode(node->son[0]);     // attr_cmd, read_cmd, print_cmd, ..., block
				fprintf(outputFile, "\n");
			}
			if(node->son[1] != NULL) {
				decompileNode(node->son[1]);     // attr_cmd, read_cmd, print_cmd, ..., block
			}
			break;
		case AST_READ:
			fprintf(outputFile, "read %s", node->symbol->text);
			break;
		case AST_PRINT:
			fprintf(outputFile, "print ");
			decompileNode(node->son[0]);     // elements
			break;
		case AST_PRINT_LIST:
			for(i=0; i<MAX_SONS; i++) {
				if(node->son[i] != NULL) {
					if(hasAnotherSon(node, i+1)) {
						decompileNode(node->son[i]);     // LIT_STRING, expr
						fprintf(outputFile, ", ");
					}
					else {
						decompileNode(node->son[i]);     // LIT_STRING, expr
					}
				}
			}
			break;
		case AST_RETURN:
			fprintf(outputFile, "return ");
			decompileNode(node->son[0]);     // expr
			break;
		case AST_IF:
			fprintf(outputFile, "if(");
			decompileNode(node->son[0]);     // expr
			fprintf(outputFile, ") then ");
			decompileNode(node->son[1]);     // cmd
			break;
		case AST_IF_ELSE:
			fprintf(outputFile, "if(");
			decompileNode(node->son[0]);     // expr
			fprintf(outputFile, ") then ");
			decompileNode(node->son[1]);     // cmd
			fprintf(outputFile, "\nelse ");
			decompileNode(node->son[2]);     // cmd
			break;
		case AST_WHILE:
			fprintf(outputFile, "while(");
			decompileNode(node->son[0]);     // expr
			fprintf(outputFile, ") ");
			decompileNode(node->son[1]);     // cmd
			break;
		case AST_LOOP:
			fprintf(outputFile, "loop(%s : ", node->symbol->text);
			decompileNode(node->son[0]);     // expr
			fprintf(outputFile, ", ");
			decompileNode(node->son[1]);     // expr
			fprintf(outputFile, ", ");
			decompileNode(node->son[2]);     // expr
			fprintf(outputFile, ") ");
			decompileNode(node->son[3]);     // cmd
			break;
		case AST_BLOCK:
			fprintf(outputFile, "{\n");
			if(node->son[0] != NULL) {
				decompileNode(node->son[0]);     // commands
			}
			fprintf(outputFile, "\n}");
			break;
		case AST_PARAMS:
			for(i=0; i<MAX_SONS; i++) {
				if(node->son[i] != NULL) {
					if(hasAnotherSon(node, i+1)) {
						decompileNode(node->son[i]);     // parameter
						fprintf(outputFile, ", ");
					}
					else {
						decompileNode(node->son[i]);     // parameter
					}
				}
			}
			break;
		case AST_PARAM:
			fprintf(outputFile, "%s = ", node->symbol->text);
			decompileNode(node->son[0]);     // type
			break;
		case AST_DECL_FUNC:
			fprintf(outputFile, "%s(", node->symbol->text);
			if(node->son[0] != NULL) {
			decompileNode(node->son[0]);     // parameters_list
			}
			fprintf(outputFile, ") = ");
			decompileNode(node->son[1]);     // type
			fprintf(outputFile, " ");
			decompileNode(node->son[2]);     // block
			fprintf(outputFile, ";\n");
			break;
		case AST_BOOL:
			fprintf(outputFile, "bool");
			break;
		case AST_CHAR:
			fprintf(outputFile, "char");
			break;
		case AST_INT:
			fprintf(outputFile, "int");
			break;
		case AST_FLOAT:
			fprintf(outputFile, "float");
			break;
		case AST_DECL_VAR:
			fprintf(outputFile, "%s = ", node->symbol->text);
			decompileNode(node->son[0]);     // type
			fprintf(outputFile, " : ");
			decompileNode(node->son[1]);     // init_value (literais)
			fprintf(outputFile, ";\n");
			break;
		case AST_DECL_VAR_VEC:
			fprintf(outputFile, "%s = ", node->symbol->text);
			decompileNode(node->son[0]);         // type
			fprintf(outputFile, "[");
			decompileNode(node->son[1]);         // vector_size (LIT_INTEGER)
			fprintf(outputFile, "]");
			if(node->son[2] != NULL) {
				fprintf(outputFile, " : ");
				decompileNode(node->son[2]);     // init_vector
			}
			fprintf(outputFile, ";\n");
			break;
		case AST_VEC_VAL:
			for(i=0; i<MAX_SONS; i++) {
				if(node->son[i] != NULL) {
					if(hasAnotherSon(node, i+1)) {
						decompileNode(node->son[i]);     // init_value (literais)
						fprintf(outputFile, " ");
					}
					else {
						decompileNode(node->son[i]);     // init_value (literais)
					}
				}
			}
			break;
		case AST_DECL:
			for(i=0; i<MAX_SONS; i++) {
				if(node->son[i] != NULL) {
					decompileNode(node->son[i]);     // dec (declaração)
				}
			}
			break;

		// caso o nodo seja uma declaração do programa
		default:
			fprintf(outputFile, "ERROR! Decompilation of AST was not possible.\n");
			break;
	}
}