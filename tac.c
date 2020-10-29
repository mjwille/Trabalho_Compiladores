/* Trabalho de Compiladores 2020/ERE
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tac.h"

// Percorre recursivamente a AST para gerar os códigos TAC
TAC_NODE* codeGenerate(AST_NODE *node) {
   return NULL;
}


// Cria nodo do tipo TAC e coloca 'prev' e 'next' para NULL
TAC_NODE* tacCreate(int opcode, HASH_NODE *res, HASH_NODE *op1, HASH_NODE *op2) {
   // Aloca espaço para novo nodo na memória
   TAC_NODE *newTac = (TAC_NODE*) calloc(1, sizeof(TAC_NODE));
   // Inicializa os campos no nodo passados como parâmetro
   newTac->opcode = opcode;
   newTac->res = res;
   newTac->op1 = op1;
   newTac->op2 = op2;
   // TAC seguinte e anterior estão indefinidas nesse ponto
   newTac->prev = NULL;
   newTac->next = NULL;
   // Retorna a nova tac criada
   return newTac;
}


// Imprime as TACs da frente para trás
void tacPrint(TAC_NODE *tac) {
   // Condição de saída do percorrimento das TACs
   if(tac == NULL)
      return;

   // Pra imprimir do início pro fim, vai indo nos prévios até chegar a raíz
   tacPrint(tac->prev);
   // E então desce nas TACs fazendo a impressão dos nodos
   tacPrintNode(tac);
}


// Imprime um nodo do tipo TAC
void tacPrintNode(TAC_NODE *tac) {
   // Não imprime nodo vazio
   if(tac == NULL)
      return;

   // Faz a impressão do nodo baseado no seu opcode

   printf("TAC(");

   switch(tac->opcode) {
      case TAC_SYMBOL:  printf("TAC_SYMBOL");   break;
      case TAC_ADD:     printf("TAC_ADD");      break;
      case TAC_SUB:     printf("TAC_SUB");      break;
      case TAC_MUL:     printf("TAC_MUL");      break;
      case TAC_DIV:     printf("TAC_DIV");      break;
      default:          printf("TAC_UNKNOWN");  break;
   }

   // Impressão dos campos de resposta e operandos dos nodos (caso tenha um ponteiro para a tabela hash)
   printf(",%s", (tac->res) ? tac->res->text : "0");
   printf(",%s", (tac->op1) ? tac->op1->text : "0");
   printf(",%s", (tac->op2) ? tac->op2->text : "0");
   printf(");\n");
}