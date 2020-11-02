/* Trabalho de Compiladores 2020/ERE
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "asm.h"

// Arquivo de output do assembly
FILE *fp = NULL;
// Nome do arquivo de output
char *OUTPUT_FILE = "./asm_code.s";


// Função principal que transforma o código intermediário em um código assembly para
// arquitetura-alvo x86-64/Linux usando sintaxe AT&T usando o assembler GNU Assembler (GAS)
void generateAsm(TAC_NODE *tac) {

	// Abre arquivo para escrever o assembly correspondente ao programa
	fp = fopen(OUTPUT_FILE, "a");
	// Se não conseguiu abrir o arquivo, dá mensagem de erro e sai do programa
	if(fp == 0) {
		fprintf(stderr, "Error.\nCould not open output file %s to write assembly code.\n", OUTPUT_FILE);
		exit(5);
	}

	// Coloca os dados
	fprintf(fp, ".section .data\n\n");
	// Adiciona o que está na tabela hash à sessão de dados
	// ...

	// Coloca o código assembly
	fprintf(fp, ".section .text\n\n");
	// Converte o código intermediário das TACs para assembly
	generateAsmFromTac(tac);

	fclose(fp);
}


// Função que é chamada por generateAsm para percorrer as TACs recursivamente e gerar o código
void generateAsmFromTac(TAC_NODE *tac) {
	// Condição de parada do percorrimento das TACs
   if(tac == NULL)
      return;

	// Início de função
	if(tac->opcode == TAC_BEGINFUN) {
		// Se for a função main
		if(!strcmp(tac->res->text, "main")) {
			fprintf(fp, ".globl _start\n\n");
			fprintf(fp, "_start:\n");
		}
		// Se for alguma outra função
		else {
			fprintf(fp, ".globl %s\n\n", tac->res->text);
			fprintf(fp, "%s:\n", tac->res->text);
		}
	}

	// Final de função
	if(tac->opcode == TAC_ENDFUN) {
		// Só for main, precisa fazer syscall de exit
		if(!strcmp(tac->res->text, "main")) {
			// Como tudo nesse programa, o valor de retorno vai estar em %eax (coloca pra %ebx)
			fprintf(fp, "\tmov %%eax, %%ebx\n");
			// Move 1 para %eax antes da chamada de sistema (valor 1 em %eax = exit)
			fprintf(fp, "\tmov   $1, %%eax\n");
			// Faz a chamada de sistema com o mnêmonico 'int'
			fprintf(fp, "\tint  $0x80\n\n");
		}
		// TODO: ainda precisa tratar o fim de funções que não são main ? (epílogo)
	}

	// Retorno de função
	else if(tac->opcode == TAC_RET) {
		fprintf(fp, "\tret\n\n");       // TODO: ainda tem mais por fazer no retorno
	}

	// Chamada de função
	else if(tac->opcode == TAC_CALL) {
		fprintf(fp, "\tcall %s\n", tac->op1->text);   // TODO: ainda tem mais por fazer na chamada (prólogo e parâmetros)
	}

	// ...

   // Vai para a próxima TAC
   generateAsmFromTac(tac->next);
}