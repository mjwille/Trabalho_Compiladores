/* Trabalho de Compiladores 2020/ERE
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tac.h"


// Percorre recursivamente a AST para gerar os códigos TAC
TAC_NODE* tacCodeGenerate(AST_NODE *node) {
   // Caso nodo exista (programa vazio e nodos filhos)
   if(node == NULL)
      return NULL;

   TAC_NODE *tac = NULL;
   TAC_NODE *tacSon[MAX_SONS];

   // Processa primeiro seus nodos filhos da AST (pra processar de baixo para cima na árvore)
	int i;
	for(i=0; i<MAX_SONS; i++) {
		tacSon[i] = tacCodeGenerate(node->son[i]);
	}

   // Processa então o nodo atual
   switch(node->type) {
      case AST_SYMBOL:   tac = tacCreate(TAC_SYMBOL, node->symbol, NULL, NULL);                             break;
      case AST_ADD:      tac = tacBinaryOperation(TAC_ADD, tacSon[0], tacSon[1]);                           break;
      case AST_SUB:      tac = tacBinaryOperation(TAC_SUB, tacSon[0], tacSon[1]);                           break;
      case AST_MUL:      tac = tacBinaryOperation(TAC_MUL, tacSon[0], tacSon[1]);                           break;
      case AST_DIV:      tac = tacBinaryOperation(TAC_DIV, tacSon[0], tacSon[1]);                           break;
      case AST_LT:       tac = tacBinaryOperation(TAC_LT,  tacSon[0], tacSon[1]);                           break;
      case AST_GT:       tac = tacBinaryOperation(TAC_GT,  tacSon[0], tacSon[1]);                           break;
      case AST_LE:       tac = tacBinaryOperation(TAC_LE,  tacSon[0], tacSon[1]);                           break;
      case AST_GE:       tac = tacBinaryOperation(TAC_GE,  tacSon[0], tacSon[1]);                           break;
      case AST_EQ:       tac = tacBinaryOperation(TAC_EQ,  tacSon[0], tacSon[1]);                           break;
      case AST_DIF:      tac = tacBinaryOperation(TAC_DIF, tacSon[0], tacSon[1]);                           break;
      case AST_XOR:      tac = tacBinaryOperation(TAC_XOR, tacSon[0], tacSon[1]);                           break;
      case AST_OR:       tac = tacBinaryOperation(TAC_OR,  tacSon[0], tacSon[1]);                           break;
      case AST_NOT:      tac = tacUnaryOperation(TAC_NOT,  tacSon[0]);                                      break;
      case AST_ATTR:     tac = tacJoin(tacSon[0], tacCreate(TAC_COPY, node->symbol, tacSon[0]->res, NULL)); break;
      case AST_IF:       tac = tacIfThen(tacSon[0], tacSon[1]);                                             break;
      case AST_IF_ELSE:  tac = tacIfThenElse(tacSon[0], tacSon[1], tacSon[2]);                              break;
      case AST_LOOP:     tac = tacLoop(tacSon[0], tacSon[1], tacSon[2], tacSon[3]);                         break;
      case AST_WHILE:    tac = tacWhile(tacSon[0], tacSon[1]);                                              break;

      // Caso nodo da AST não tenha opcode TAC, junta os TACs dos filhos na AST de forma unificada
      default: tac = tacJoin(tacSon[0], tacJoin(tacSon[1], tacJoin(tacSon[2], tacSon[3])));               break;
   }

   return tac;
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
   // Retorna a nova TAC criada
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
   // Não imprime nodo vazio ou nodo que seja símbolo
   if(tac == NULL || tac->opcode == TAC_SYMBOL)
      return;

   // Faz a impressão do nodo baseado no seu opcode

   printf("TAC(");

   switch(tac->opcode) {
      case TAC_ADD:     printf("TAC_ADD");      break;
      case TAC_SUB:     printf("TAC_SUB");      break;
      case TAC_MUL:     printf("TAC_MUL");      break;
      case TAC_DIV:     printf("TAC_DIV");      break;
      case TAC_LT:      printf("TAC_LT");       break;
      case TAC_GT:      printf("TAC_GT");       break;
      case TAC_LE:      printf("TAC_LE");       break;
      case TAC_GE:      printf("TAC_GE");       break;
      case TAC_EQ:      printf("TAC_EQ");       break;
      case TAC_DIF:     printf("TAC_DIF");      break;
      case TAC_XOR:     printf("TAC_XOR");      break;
      case TAC_OR:      printf("TAC_OR");       break;
      case TAC_NOT:     printf("TAC_NOT");      break;
      case TAC_COPY:    printf("TAC_COPY");     break;
      case TAC_JF:      printf("TAC_JF");       break;
      case TAC_JMP:     printf("TAC_JMP");      break;
      case TAC_LABEL:   printf("TAC_LABEL");    break;
      default:          printf("TAC_UNKNOWN");  break;
   }

   // Impressão dos campos de resposta e operandos dos nodos (caso tenha um ponteiro para a tabela hash)
   printf(", %s", (tac->res) ? tac->res->text : "0");
   printf(", %s", (tac->op1) ? tac->op1->text : "0");
   printf(", %s", (tac->op2) ? tac->op2->text : "0");
   printf(");\n");
}


// Junta 2 TACs em uma TAC única maior
TAC_NODE* tacJoin(TAC_NODE *tac1, TAC_NODE *tac2) {
   // Se não tem a primeira TAC, retorna a segunda TAC
   if(tac1 == NULL)
      return tac2;

   // Se não tem a segunda TAC, retorna a primeira TAC
   if(tac2 == NULL)
      return tac1;

   TAC_NODE *aux;
   // Passa por todos os TAC prévios de tac2 para realmente colocar tac1 no início de tac2
   for(aux = tac2; aux->prev != NULL; aux = aux->prev);
   // Coloca tac1 no início de tac2
   aux->prev = tac1;

   /* Agora as duas devem estar assim:
    *
    * [a -> b -> c] tac1 -> [x -> y -> z] tac2
    *
    * onde a, b, c, ..., x, y, z eram as tacs dentro de tac1 e tac2, e z, y, x foi o que
    * percorremos com o 'prev' para que tac1 apontasse para o primeiro tac de tac2 (eg. x)
    */

   return tac2;
}


// Cria uma TAC para operadores binários
TAC_NODE* tacBinaryOperation(int opcode, TAC_NODE *son0, TAC_NODE *son1) {
   // Cria um temporário na hash table para onde o resultado da operação será colocado
   HASH_NODE *temp = makeTemp();
   // Cria o código TAC da operação, que tem como operandos os resultados dos códigos dos filhos
   HASH_NODE *op1 = (son0->res) ? son0->res : 0;
   HASH_NODE *op2 = (son1->res) ? son1->res : 0;
   TAC_NODE *tacOperation = tacCreate(opcode, temp, op1, op2);
   // Junta o código dos filhos para os quais a operação binária será aplicada
   TAC_NODE *tacSons = tacJoin(son0, son1);
   // Junta por fim o código dos filhos (que vem antes da operação) com o código da própria operação
   return tacJoin(tacSons, tacOperation);
}


// Cria uma TAC para operadores unários
TAC_NODE* tacUnaryOperation(int opcode, TAC_NODE *son0) {
   // Cria um temporário na hash table para onde o resultado da operação será colocado
   HASH_NODE *temp = makeTemp();
   // Cria o código TAC da operação, que tem como operandos os resultados dos códigos dos filhos
   HASH_NODE *op1 = (son0->res) ? son0->res : 0;
   TAC_NODE *tacOperation = tacCreate(opcode, temp, op1, NULL);
   // Junta por fim o código do filho (que vem antes da operação) com o código da própria operação
   return tacJoin(son0, tacOperation);
}


// Cria uma TAC para o if/then
TAC_NODE* tacIfThen(TAC_NODE *son0, TAC_NODE *son1) {
   // Cria TAC e label na hash para montar a lógica do If/Then
   HASH_NODE *label = makeLabel();
   TAC_NODE *tacJf  = tacCreate(TAC_JF, label, son0->res, NULL);
   // Cria TAC_LABEL que vai ficar antes do código pra dar jump se for falso (JF - Jump If False)
   TAC_NODE *tacLabel = tacCreate(TAC_LABEL, label, NULL, NULL);
   /* Faz os joins para que o If fique na forma:
    *
    * expr (código de son0)
    * TAC_JF -------------------
    * expr (código de son1)    |
    * TAC_LABEL <--------------
    * ...
    *
    * Tudo isso precisa ser juntado quando chegar no nodo da AST que representa o If/Then
    */
   return tacJoin(son0, tacJoin(tacJf, tacJoin(son1, tacLabel)));
}


// Cria uma TAC para o if/then/else
TAC_NODE* tacIfThenElse(TAC_NODE *son0, TAC_NODE *son1, TAC_NODE *son2) {
   // Cria TACs e labels na hash para montar a lógica do If/Then/Else
   HASH_NODE *label1 = makeLabel();
   HASH_NODE *label2 = makeLabel();
   TAC_NODE *tacJf   = tacCreate(TAC_JF,  label1, son0->res, NULL);
   TAC_NODE *tacJmp  = tacCreate(TAC_JMP, label2, NULL, NULL);
   // Cria TAC_LABELs utilizados para dar os jumps do If/Then/Else
   TAC_NODE *tacLabel1 = tacCreate(TAC_LABEL, label1, NULL, NULL);
   TAC_NODE *tacLabel2 = tacCreate(TAC_LABEL, label2, NULL, NULL);
   /* Faz os joins para que o If/Then/Else fique na forma:
    *
    *               expr (código de son0)
    *               TAC_JF -------------------
    *               expr (código de son1)    |
    *          ---- TAC_JMP                  |
    *          |    TAC_LABEL1 <-------------
    *          |    expr (código de son2)
    *          ---> TAC_LABEL2
    *               ...
    *
    * Tudo isso precisa ser juntado quando chegar no nodo da AST que representa o If/Then/Else
    */
   return tacJoin(son0, tacJoin(tacJf, tacJoin(son1, tacJoin(tacJmp, tacJoin(tacLabel1, tacJoin(son2, tacLabel2))))));
}


// Cria uma TAC para o loop
TAC_NODE* tacLoop(TAC_NODE *son0, TAC_NODE *son1, TAC_NODE *son2, TAC_NODE *son3) {
   // Cria TACs e labels na hash para montar a lógica do loop
   HASH_NODE *label1 = makeLabel();
   HASH_NODE *label2 = makeLabel();
   TAC_NODE *tacJf   = tacCreate(TAC_JF,  label2, son1->res, NULL);
   TAC_NODE *tacJmp  = tacCreate(TAC_JMP, label1, NULL, NULL);
   // Cria TAC_LABELs utilizados para dar os jumps do loop
   TAC_NODE *tacLabel1 = tacCreate(TAC_LABEL, label1, NULL, NULL);
   TAC_NODE *tacLabel2 = tacCreate(TAC_LABEL, label2, NULL, NULL);
   /* Faz os joins para que o loop fique na forma:
    *
    *                expr (código de son0)                    *inicialização do loop
    *           ---> TAC_LABEL1
    *          |     expr (código de son1)                    *condição de parada do loop
    *          |     TAC_JF -------------------
    *          |     expr (código de son3)    |               *corpo do loop
    *          |     expr (código de son2)    |               *expressão de update do loop
    *          ---- TAC_JMP                   |
    *               TAC_LABEL2 <-------------
    *               ...
    *
    * Tudo isso precisa ser juntado quando chegar no nodo da AST que representa o loop
    */
   return tacJoin(son0, tacJoin(tacLabel1, tacJoin(son1, tacJoin(tacJf, tacJoin(son3, tacJoin(son2, tacJoin(tacJmp, tacLabel2)))))));
}


// Cria uma TAC para o while
TAC_NODE* tacWhile(TAC_NODE *son0, TAC_NODE *son1) {
   // Cria TACs e labels na hash para montar a lógica do while
   HASH_NODE *label1 = makeLabel();
   HASH_NODE *label2 = makeLabel();
   TAC_NODE *tacJf   = tacCreate(TAC_JF,  label2, son0->res, NULL);
   TAC_NODE *tacJmp  = tacCreate(TAC_JMP, label1, NULL, NULL);
   // Cria TAC_LABELs utilizados para dar os jumps do while
   TAC_NODE *tacLabel1 = tacCreate(TAC_LABEL, label1, NULL, NULL);
   TAC_NODE *tacLabel2 = tacCreate(TAC_LABEL, label2, NULL, NULL);
   /* Faz os joins para que o loop fique na forma:
    *
    *           ---> TAC_LABEL1
    *          |     expr (código de son0)                    *condição do while
    *          |     TAC_JF -------------------
    *          |     expr (código de son1)    |               *corpo do while
    *          ---- TAC_JMP                   |
    *               TAC_LABEL2 <-------------
    *               ...
    *
    * Tudo isso precisa ser juntado quando chegar no nodo da AST que representa o while
    */
   return tacJoin(tacLabel1, tacJoin(son0, tacJoin(tacJf, tacJoin(son1, tacJoin(tacJmp, tacLabel2)))));
}