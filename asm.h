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

// Gera código assembly para operações binárias (soma, subtração e multiplicação, xor, or)
void asmBinaryOperation(TAC_NODE *tac, char *mnemonic);

// Gera código assembly para operações de comparação (igual, diferente, maior que, menor que, maior igual que, menor igual que)
void asmComparisonOperation(TAC_NODE *tac, char *mnemonic);

// Percorre as TACs e coloca na sessão de dados do assembly as variáveis globais
void addVarsToData(TAC_NODE *tac);

// Percorre a tabela de símbolos e coloca temporários e strings na sessão de dados do assembly
void addSymbolsToData();

#endif