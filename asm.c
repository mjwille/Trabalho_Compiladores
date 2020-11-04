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
// Como a função main é um caso a parte, precisa identifcar quando está nela (retorno é diferente, por exemplo)
int IS_INSIDE_MAIN = 0;


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

	// Coloca comentários iniciais
	fprintf(fp, "# Código assembly gerado a partir do código fonte da linguagem\n");
	fprintf(fp, "# Trabalho de Compiladores 2020/ERE\n");
	fprintf(fp, "# Nome: Marcelo Jantsch Wille\n");
	fprintf(fp, "# Universidade Federal do Rio Grande do Sul\n");

	// Coloca os dados na sessão de dados
	fprintf(fp, "\n.section .data\n\n");
	// Adiciona o que está na tabela hash à sessão de dados
	addVarsToData(tac);
	addTempsToData();
	//addStringsToData();

	// Coloca o código assembly
	fprintf(fp, "\n.section .text\n\n");
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
			// Apenas declara como global e coloca o label de início de função
			fprintf(fp, ".globl _start\n\n");
			fprintf(fp, "_start:\n");
			IS_INSIDE_MAIN = 1;
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
			IS_INSIDE_MAIN = 0;
		}
		// TODO: precisa tratar o fim de funções ?
	}

	// Retorno de função
	else if(tac->opcode == TAC_RET) {
		fprintf(fp, "\t# Retorno\n");
		if(IS_INSIDE_MAIN) {
			// Move 1 para %eax antes da chamada de sistema (valor 1 em %eax = exit)
			fprintf(fp, "\tmov   $1, %%eax\n");
			// Valor de retorno vai estar no primeiro operando, que precisa ser colocado em %ebx
			fprintf(fp, "\tmov %s, %%ebx\n", tac->op1->text);
			// Faz a chamada de sistema com o mnêmonico 'int'
			fprintf(fp, "\tint  $0x80\n\n");
		}
		if(!IS_INSIDE_MAIN) {
			fprintf(fp, "\tret\n\n");    // TODO: ainda tem mais por fazer no retorno (epílogo?)
		}
	}

	// Chamada de função
	else if(tac->opcode == TAC_CALL) {
		fprintf(fp, "\tcall %s\n", tac->op1->text);   // TODO: ainda tem mais por fazer na chamada (prólogo e parâmetros)
	}

	// Adição
	else if(tac->opcode == TAC_ADD) {
		generateAsmBinOperation(tac, "add");
	}

	// Subtração
	else if(tac->opcode == TAC_SUB) {
		generateAsmBinOperation(tac, "sub");
	}

	// Multiplicação
	else if(tac->opcode == TAC_MUL) {
		generateAsmBinOperation(tac, "imul");
	}

	else if(tac->opcode == TAC_DIV) {
		fprintf(fp, "\t# Operação binária\n");
		// Coloca dividendo em %eax
		// Se for literal, precisa ser modo imediato
		if(tac->op1->type == SYMBOL_LIT_INTEGER) {                    // TODO: char, float, vetor, ...
			fprintf(fp, "\tmov $%s, %%eax\n", tac->op1->text);

		}
		// Caso não seja literal, acessa pelo modo direto a variável na sessão de dados
		else {
			fprintf(fp, "\tmov %s, %%eax\n", tac->op1->text);
		}
		// Coloca divisor em %ebx
		// Mesma coisa para o operando 2
		if(tac->op2->type == SYMBOL_LIT_INTEGER) {                    // TODO: char, float, vetor, ...
			fprintf(fp, "\tmov $%s, %%ebx\n", tac->op2->text);
		}
		else {
			fprintf(fp, "\tmov %s, %%ebx\n", tac->op2->text);
		}
		// %edx precisa ser zero
		fprintf(fp, "\tmov $0, %%edx\n");
		// Faz a operação
		fprintf(fp, "\tidiv %%ebx, %%eax\n");
		// Quociente vai para %eax e resto para %edx
		// Coloca resultado da divisão para o campo resultado da TAC (na sessão de dados do assembly)
		fprintf(fp, "\tmov %%eax, %s\n", tac->res->text);
	}

	// Atribuição
	else if(tac->opcode == TAC_COPY) {
		fprintf(fp, "\t# Atribuição\n");
		// Copia valor a ser atribuído para %eax
		fprintf(fp, "\tmov %s, %%eax\n", tac->op1->text);
		// Coloca valor em %eax para a variável do lado esquerdo da atribuição
		fprintf(fp, "\tmov %%eax, %s\n", tac->res->text);
	}

	// Label para fazer os jumps (condicionais e incondicionais)
	else if(tac->opcode == TAC_LABEL) {
		// Apenas coloca o label no código assembly
		fprintf(fp, "\t%s:\n", tac->res->text);
	}

	// Jump
	else if(tac->opcode == TAC_JMP) {
		fprintf(fp, "\tjmp %s\n", tac->res->text);
	}

	// ...

   // Vai para a próxima TAC
   generateAsmFromTac(tac->next);
}


// Gera código assembly para operações binárias de soma, subtração e multiplicação
void generateAsmBinOperation(TAC_NODE *tac, char *mnemonic) {
	fprintf(fp, "\t# Operação binária\n");
	// Copia os operandos para %eax e %ebx
	// Se for literal, precisa ser modo imediato
	if(tac->op1->type == SYMBOL_LIT_INTEGER) {                    // TODO: char, float, vetor, ...
		fprintf(fp, "\tmov $%s, %%eax\n", tac->op1->text);

	}
	// Caso não seja literal, acessa pelo modo direto a variável na sessão de dados
	else {
		fprintf(fp, "\tmov %s, %%eax\n", tac->op1->text);
	}

	// Mesma coisa para o operando 2
	if(tac->op2->type == SYMBOL_LIT_INTEGER) {                    // TODO: char, float, vetor, ...
		fprintf(fp, "\tmov $%s, %%ebx\n", tac->op2->text);
	}
	else {
		fprintf(fp, "\tmov %s, %%ebx\n", tac->op2->text);
	}

	// Faz a operação (resultado fica em %eax)
	fprintf(fp, "\t%s %%ebx, %%eax\n", mnemonic);
	// Coloca resultado da operação para o campo resultado da TAC (na sessão de dados do assembly)
	fprintf(fp, "\tmov %%eax, %s\n", tac->res->text);
}


/* Abaixo, funções para colocar dados na sessão de dados do assembly.
 * -> Declarações globais de escalares e vetores
 * -> Strings usadas com o comando print
 * -> Temporaríos gerados pelas TACs (precisa percorrer as TACs uma vez antes de começar a gerar o assembly)
 */


// Percorre as TACs e coloca na sessão de dados do assembly as variáveis globais
void addVarsToData(TAC_NODE *tac) {
	// Condição de parada do percorrimento das TACs
   if(tac == NULL)
      return;

	// Declarações de variáveis escalares globais
	if(tac->opcode == TAC_VARDECL) {
		// Se variável global for do tipo char
		if(tac->res->dataType == DATATYPE_CHAR) {
			fprintf(fp, "%s:\n", tac->res->text);
			// TODO: vai o char ou a representação inteira dele após .byte?
		}
		// Se variável global for do tipo int
		else if(tac->res->dataType == DATATYPE_INT) {
			fprintf(fp, "%s:\n", tac->res->text);
			fprintf(fp, "\t.long %s\n", tac->op1->text);
		}
		// Se variável global for do tipo float
		else if(tac->res->dataType == DATATYPE_FLOAT) {
			fprintf(fp, "%s:\n", tac->res->text);
			// TODO: como representar floats na sessão de dados do assembly?
		}
		// Se variável global for do tipo boolean
		else {
			fprintf(fp, "%s:\n", tac->res->text);
			// TODO: como representar booleanos (0 ou 1)?
		}
	}

	// Declarações de variáveis vetores globais
	else if(tac->opcode == TAC_VECDECL) {
		// TODO: vetores vai ser uma lista, mas incorre nos mesmos problemas de tipos que faltam do anterior
	}

	// Vai para a próxima TAC
   addVarsToData(tac->next);
}


// Percorre a tabela de símbolos e coloca os temporários na sessão de dados do assembly
void addTempsToData() {
	char *prefix = "__temp";
	int i;
	HASH_NODE *node;
	// Percorre todo índice da tabela de símbolos
	for(i=0; i<HASH_SIZE; i++) {
		if(HASH_TABLE[i] != NULL) {
			// Percorre toda a lista encadeada dos índices
			for(node=HASH_TABLE[i]; node; node=node->next) {
				// Se texto começa com __temp
				if(!strncmp(prefix, node->text, strlen(prefix))) {
					// Coloca o temporário na sessão de dados do assembly
					fprintf(fp, "%s:\n", node->text);
					fprintf(fp, "\t.long 0\n");
				}
			}
		}
	}
}