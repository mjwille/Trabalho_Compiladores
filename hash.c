/* Trabalho de Compiladores 2020/ERE
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hash.h"


// Inicializa todas as posições da tabela hash com o valor default NULL
void hashInit()
{
	int i;
	for(i=0; i<HASH_SIZE; i++)
	{
		HASH_TABLE[i] = NULL;
	}
}


// Calcula o endereço (índice) de um dado element (dado seu campo texto) dentro do array da tabela hash
int hashAddress(char *text)
{
	int i, addr = 1;
	for(i=0; i<strlen(text); i++)
	{
		// Função Hash (entre 1 e 997)
		addr = (addr * text[i]) % HASH_SIZE + 1;
	}
	return addr-1;   // Corrige para ficar entre 0 e 996
}


// Verifica se um nodo existe na tabela. Caso sim, retorna este nodo. Caso contrário, retorna NULL
HASH_NODE* hashFind(char *text)
{
	// pega qual seria o índice na tabela hash caso *text estivesse nela
	int addr = hashAddress(text);

	HASH_NODE *node;

	// verifica a lista encadeada deste índice para ver se está lá
	for(node=HASH_TABLE[addr]; node; node=node->next) {
		// se está na lista encadeada
		if(strcmp(node->text, text) == 0)
			return node;
	}

	// se não está na lista encadeada
	return NULL;
}


// Insere elemento dentro da tabela hash
HASH_NODE* hashInsert(char *text, int type, int dataType)
{
	HASH_NODE *node;

	// se nodo já está na hash, retorna o nodo sem inserir ele na tabela de símbolos novamente
	node = hashFind(text);
	if(node != NULL) {
		return node;
	}

	// senão, insere ele na tabela e depois retorna o novo nodo:

	// Calcula o índice (endereço) deste novo nodo para poder inserí-lo dentro da tabela hash
	int addr = hashAddress(text);

	// aloca espaço para novo nodo na memória
	node = (HASH_NODE*) calloc(1, sizeof(HASH_NODE));

	// Coloca dentro do nodo alocado os seus respectivos valores
	node->type = type;
	node->dataType = DATATYPE_NULL;
	node->text = (char*) calloc(strlen(text)+1, sizeof(char));
	strcpy(node->text, text);

	// Faz o encadeamento no índice da tabela hash (o último inserido é o primeiro a ser apontado pelo ponteiro do índice)
	node->next = HASH_TABLE[addr];
	HASH_TABLE[addr] = node;

	return node;
}


// Imprime todo elemento que a tabela hash contém dentro dela (índice a índice)
void hashPrint()
{
	int i;
	HASH_NODE *node;

	printf("----------------------------- SYMBOL TABLE BEGIN -----------------------------\n");

	for(i=0; i<HASH_SIZE; i++) {
		if(HASH_TABLE[i] != NULL)
		{
			printf("[%d]", i);
			for(node=HASH_TABLE[i]; node; node=node->next)
				printf(" <- [ Type: %s | Text: %s | Datatype: %s ]", formatType(node->type), node->text, formatType(node->dataType));
			printf("\n");
		}
	}

	printf("------------------------------ SYMBOL TABLE END ------------------------------\n");
}


// Função auxiliar que retorna o tipo ou dataType em uma melhor formatação para a impressão em hashPrint
char* formatType(int type)
{
	switch(type) {
		case DATATYPE_NULL:        return "NULL";
		case SYMBOL_LIT_INTEGER:   return "LIT_INT";
		case SYMBOL_LIT_FLOAT:     return "LIT_FLOAT";
		case SYMBOL_LIT_TRUE:      return "LIT_TRUE";
		case SYMBOL_LIT_FALSE:     return "LIT_FALSE";
		case SYMBOL_LIT_CHAR:      return "LIT_CHAR";
		case SYMBOL_LIT_STRING:    return "LIT_STRING";
		case SYMBOL_IDENTIFIER:    return "IDENTIFIER";
		case SYMBOL_SCALAR:        return "SCALAR";
		case SYMBOL_VECTOR:        return "VECTOR";
		case SYMBOL_FUNCTION:      return "FUNCTION";
		case DATATYPE_BOOL:        return "BOOL";
		case DATATYPE_CHAR:        return "CHAR";
		case DATATYPE_INT:         return "INT";
		case DATATYPE_FLOAT:       return "FLOAT";
		default:                   return "UNKNOWN";
	}
}