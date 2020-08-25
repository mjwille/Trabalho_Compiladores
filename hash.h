/* Trabalho de Compiladores 2020/1
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul
*/

#include <stdio.h>
#include <string.h>

typedef struct hash_node
{
	int type;
	char *text;
	struct hash_node *next;
} HASH_NODE;

// Definição do tamanho do array da tabela hash
#define HASH_SIZE 997
// Definição da tabela hash
HASH_NODE *HASH_TABLE[HASH_SIZE];

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
	return NULL;   //TODO
}

// Insere elemento dentro da tabela hash
HASH_NODE* hashInsert(char *text)
{
	// Calcula o índice (endereço) deste novo nodo para poder inserí-lo dentro da tabela hash
	int addr = hashAddress(text);

	HASH_NODE *node = (HASH_NODE*) calloc(1, sizeof(HASH_NODE));

	// Coloca dentro do nodo alocado os seus respectivos valores
	node->type = 1;
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

	printf("-------------------------- SYMBOL TABLE BEGIN --------------------------\n");

	for(i=0; i<HASH_SIZE; i++) {
		if(HASH_TABLE[i] != NULL)
		{
			printf("[%d]", i);
			for(node=HASH_TABLE[i]; node; node=node->next)
				printf(" <- ( Type: %d | Text: %s )", node->type, node->text);
			printf("\n");
		}
	}

	printf("--------------------------- SYMBOL TABLE END ---------------------------\n");
}