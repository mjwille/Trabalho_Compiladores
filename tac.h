/* Trabalho de Compiladores 2020/ERE
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul
*/

#ifndef TAC_HEADER
#define TAC_HEADER

#include "hash.h"
#include "ast.h"

// Definição das operações (opcodes possíveis) para um nodo de "Three Address Code" (TAC)
#define TAC_SYMBOL    1
#define TAC_ADD       2
#define TAC_SUB       3
#define TAC_MUL       4
#define TAC_DIV       5
#define TAC_LT        6
#define TAC_GT        7
#define TAC_LE        8
#define TAC_GE        9
#define TAC_EQ       10
#define TAC_DIF      11
#define TAC_XOR      12
#define TAC_OR       13
#define TAC_NOT      14
#define TAC_COPY     15
#define TAC_JF       16
#define TAC_JMP      17
#define TAC_LABEL    18
#define TAC_BEGINFUN 19
#define TAC_ENDFUN   20
#define TAC_READ     21
#define TAC_RET      22
#define TAC_CALL     23
#define TAC_VECREAD  24
#define TAC_VECCOPY  25

typedef struct tac_node
{
   int opcode;
   HASH_NODE *res;
   HASH_NODE *op1;
   HASH_NODE *op2;
   struct tac_node *prev;
   struct tac_node *next;
} TAC_NODE;

// Percorre recursivamente a AST para gerar os códigos TAC
TAC_NODE* tacCodeGenerate(AST_NODE *node);

// Cria nodo do tipo TAC e coloca 'prev' e 'next' para NULL
TAC_NODE* tacCreate(int opcode, HASH_NODE *res, HASH_NODE *op1, HASH_NODE *op2);

// Função principal chamada pelo parser.y para imprimir as TACs (chama a função de impressão recursiva)
void tacPrint(TAC_NODE *tac);

// Imprime as TACs da frente para trás de forma recursiva
void tacPrintRecursive(TAC_NODE *tac);

// Imprime um nodo do tipo TAC
void tacPrintNode(TAC_NODE *tac);

// Junta 2 TACs em uma TAC única maior
TAC_NODE* tacJoin(TAC_NODE *tac1, TAC_NODE *tac2);

/* ------------------------- Funções Auxiliares ------------------------- */

// Cria TAC para operadores binários
TAC_NODE* makeTacBinaryOperation(int opcode, TAC_NODE *son0, TAC_NODE *son1);

// Cria TAC para operadores unários
TAC_NODE* makeTacUnaryOperation(int opcode, TAC_NODE *son0);

// Cria TAC para o if/then
TAC_NODE* makeTacIfThen(TAC_NODE *son0, TAC_NODE *son1);

// Cria TAC para o if/then/else
TAC_NODE* makeTacIfThenElse(TAC_NODE *son0, TAC_NODE *son1, TAC_NODE *son2);

// Cria TAC para o loop
TAC_NODE* makeTacLoop(TAC_NODE *son0, TAC_NODE *son1, TAC_NODE *son2, TAC_NODE *son3);

// Cria TAC para o while
TAC_NODE* makeTacWhile(TAC_NODE *son0, TAC_NODE *son1);

// Cria TACs para declarações de funções
TAC_NODE* makeTacDefFun(AST_NODE *node, TAC_NODE *son0, TAC_NODE *son1);

// Cria TAC para comando return
TAC_NODE* makeTacRet(TAC_NODE *son0);

// Cria TAC para leitura de posição do vetor
TAC_NODE* makeTacVecRead(AST_NODE *node, TAC_NODE *son0);

// Cria TAC para mover valor para posição do vetor
TAC_NODE* makeTacVecCopy(AST_NODE *node, TAC_NODE *son0, TAC_NODE *son1);

// Função usada para verificar se a TAC realmente possui o campo na struct diferente de NULL e evitar segfaults
HASH_NODE* tacResCheck(TAC_NODE *tac);

#endif