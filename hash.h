/* Trabalho de Compiladores 2020/1
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul
*/

#ifndef HASH_HEADER
#define HASH_HEADER

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
void hashInit();

// Calcula o endereço (índice) de um dado element (dado seu campo texto) dentro do array da tabela hash
int hashAddress(char *text);

// Verifica se um nodo existe na tabela. Caso sim, retorna este nodo. Caso contrário, retorna NULL
HASH_NODE* hashFind(char *text);

// Insere elemento dentro da tabela hash
HASH_NODE* hashInsert(char *text, int type);

// Imprime todo elemento que a tabela hash contém dentro dela (índice a índice)
void hashPrint();

#endif