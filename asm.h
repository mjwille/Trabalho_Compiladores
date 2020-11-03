/* Trabalho de Compiladores 2020/ERE
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul
*/

#ifndef ASM_HEADER
#define ASM_HEADER

#include "tac.h"

// Função principal que transforma o código intermediário em um código assembly para
// arquitetura-alvo x86-64/Linux usando sintaxe AT&T usando o assembler GNU Assembler (GAS)
void generateAsm(TAC_NODE *tac);

// Função que é chamada por generateAsm para percorrer as TACs recursivamente e gerar o código
void generateAsmFromTac(TAC_NODE *tac);

// Percorre as TACs e coloca na sessão de dados do assembly as variáveis globais
void addVarsToData(TAC_NODE *tac);

// Percorre a tabela de símbolos e coloca os temporários na sessão de dados do assembly
void addTempsToData();

#endif