/* Trabalho de Compiladores 2020/ERE
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul
*/

#ifndef HASH_HEADER
#define HASH_HEADER

#define DATATYPE_NULL 0

/* ---------- Símbolos marcados pelo scanner ---------- */

// Literais
#define SYMBOL_LIT_INTEGER 1
#define SYMBOL_LIT_FLOAT 2
#define SYMBOL_LIT_TRUE 3
#define SYMBOL_LIT_FALSE 4
#define SYMBOL_LIT_CHAR 5
#define SYMBOL_LIT_STRING 6
// Identificador
#define SYMBOL_IDENTIFIER 7

/* ---------- Símbolos marcadores pelo analisador semântico ---------- */

// Semântica dos Identificadores (redefinido pela análise semântica)
#define SYMBOL_SCALAR 8
#define SYMBOL_VECTOR 9
#define SYMBOL_FUNCTION 10
// Tipos de dados dos identificadores (variável escalar é tipo int, variável escalar float, função que retorna bool, ...)
#define DATATYPE_BOOL  11
#define DATATYPE_CHAR  12
#define DATATYPE_INT   13
#define DATATYPE_FLOAT 14

// Símbolo extra para reconhecer labels das TACs na hash table
#define SYMBOL_LABEL 15

typedef struct hash_node
{
	int type;
	char *text;
	int dataType;
	char *strName;
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
HASH_NODE* hashInsert(char *text, int type, int dataType, char *strName);

// Imprime todo elemento que a tabela hash contém dentro dela (índice a índice)
void hashPrint();

// Função auxiliar que retorna o tipo ou dataType em uma melhor formatação para a impressão em hashPrint
char* formatType(int type);

// Função auxiliar que retorna o nome da string a ser impresso na tabela de símbolos
char* formatStr(char *strName);

// Cria um nodo com um temporário para a geração de TACs
HASH_NODE* makeTemp();

// Cria um nodo com um label para a geração de TACs
HASH_NODE* makeLabel();

// Cria nome de uma string para poder referenciar no assembly e fazer print
char* createStr(char *strName);

#endif