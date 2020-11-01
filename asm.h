/* Trabalho de Compiladores 2020/ERE
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul
*/

#ifndef ASM_HEADER
#define ASM_HEADER

#include "tac.h"

// Função principal que transforma o código intermediário em um código assembly
// para arquitetura-alvo x64/Linux usando sintaxe AT&T sobre o compilador gcc
void generateAsm(TAC_NODE *tac);

// Função que é chamada por generateAsm para percorrer as TACs recursivamente e gerar o código
void generateAsmFromTac(TAC_NODE *tac);

#endif