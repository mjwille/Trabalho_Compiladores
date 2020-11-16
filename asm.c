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
// arquitetura-alvo x86-64/UNIX usando sintaxe AT&T usando o assembler do gcc
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

	// Coloca as variáveis e temporários na sessão de dados
	fprintf(fp, "\n.section __DATA, __data\n\n");
	addVarsToAsm(tac);
	addTempsToAsm();
	// Adiciona strings para sessão de strings
	fprintf(fp, "\n.section __TEXT, __cstring, cstring_literals\n\n");
	addStringsToAsm();

	// Coloca o código assembly na sessão de código (text)
	fprintf(fp, "\n.section __TEXT, __text, regular, pure_instructions\n\n");
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
		fprintf(fp, "# Início de Função\n");
		// Se for a função main
		if(!strcmp(tac->res->text, "main")) {
			// Apenas declara como global e coloca o label de início de função
			fprintf(fp, ".globl _main\n");
			fprintf(fp, "_main:\n");
			// Preâmbulo da main
			fprintf(fp, "\tpushq %%rbp\n");
			fprintf(fp, "\tmovq %%rsp, %%rbp\n");
			IS_INSIDE_MAIN = 1;
		}
		// Se for alguma outra função
		else {
			fprintf(fp, ".globl %s\n\n", tac->res->text);
			fprintf(fp, "%s:\n", tac->res->text);
			// TODO: Preâmbulo da outra função
		}
	}

	// Final de função
	if(tac->opcode == TAC_ENDFUN) {
		fprintf(fp, "\t# Fim de Função\n");
		// Só for main, precisa fazer syscall de exit
		if(!strcmp(tac->res->text, "main")) {
			IS_INSIDE_MAIN = 0;
		}
		fprintf(fp, "\tpopq %%rbp\n");
		fprintf(fp, "\tretq\n");
	}

	// Retorno de função
	else if(tac->opcode == TAC_RET) {
		fprintf(fp, "\t# Retorno\n");

		if(IS_INSIDE_MAIN) {
			// TODO: coloca valor de retorno no %eax
		}
		// Se não estiver dentro da main
		else {
			// TODO: coloca valor de retorno no %eax
		}
	}

	// Chamada de função
	else if(tac->opcode == TAC_CALL) {
		fprintf(fp, "\tcall %s\n", tac->op1->text);   // TODO: ainda tem mais por fazer na chamada (prólogo e parâmetros)
	}

	// Adição
	else if(tac->opcode == TAC_ADD) {
		fprintf(fp, "\t# Operação ADD\n");
		asmBinaryOperation(tac, "add");
	}

	// Subtração
	else if(tac->opcode == TAC_SUB) {
		fprintf(fp, "\t# Operação SUB\n");
		asmBinaryOperation(tac, "sub");
	}

	// Multiplicação
	else if(tac->opcode == TAC_MUL) {
		fprintf(fp, "\t# Operação MUL\n");
		asmBinaryOperation(tac, "imul");
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

	// Xor
	else if(tac->opcode == TAC_XOR) {
		fprintf(fp, "\t# Operação XOR\n");
		asmBinaryOperation(tac, "xor");
	}

	// Or
	else if(tac->opcode == TAC_OR) {
		fprintf(fp, "\t# Operação OR\n");
		asmBinaryOperation(tac, "or");
	}

	// Not (Boolean Not, e não o 'not' do assembly que inverte todos os bits)
	else if(tac->opcode == TAC_NOT) {
		fprintf(fp, "\t# Operação NOT\n");

		HASH_NODE *label1 = makeLabel();
		HASH_NODE *label2 = makeLabel();

		fprintf(fp, "\tmov %s, %%eax\n", tac->op1->text);
		// Testa se é true
		fprintf(fp, "\tcmp $1, %%eax\n");
		// Se for true (igual a 1), pula
		fprintf(fp, "\tje %s\n", label1->text);
		// Senão, coloca 1 na resposta da TAC na sessão de dados (era false)
		fprintf(fp, "\tmov $1, %%eax\n");
		fprintf(fp, "\tmov %%eax, %s\n", tac->res->text);
		// Pula para não fazer a condição do true
		fprintf(fp, "\tjmp %s\n", label2->text);
		// Se true vai pular pra cá, onde coloca zero na resposta da TAC na sessão de dados (era true)
		fprintf(fp, "\t%s:\n", label1->text);
		fprintf(fp, "\tmov $0, %%eax\n");
		fprintf(fp, "\tmov %%eax, %s\n", tac->res->text);
		// Label para pular depois de colocar 1
		fprintf(fp, "\t%s:\n", label2->text);
	}

	else if(tac->opcode == TAC_EQ) {
		fprintf(fp, "\t# Comparação EQ\n");
		asmComparisonOperation(tac, "je");
	}

	else if(tac->opcode == TAC_DIF) {
		fprintf(fp, "\t# Comparação DIF\n");
		asmComparisonOperation(tac, "jne");
	}

	else if(tac->opcode == TAC_GE) {
		fprintf(fp, "\t# Comparação GE\n");
		asmComparisonOperation(tac, "jge");
	}

	else if(tac->opcode == TAC_LE) {
		fprintf(fp, "\t# Comparação LE\n");
		asmComparisonOperation(tac, "jle");
	}

	else if(tac->opcode == TAC_GT) {
		fprintf(fp, "\t# Comparação GT\n");
		asmComparisonOperation(tac, "jg");
	}

	else if(tac->opcode == TAC_LT) {
		fprintf(fp, "\t# Comparação LT\n");
		asmComparisonOperation(tac, "jl");
	}

	// Atribuição
	else if(tac->opcode == TAC_COPY) {
		fprintf(fp, "\t# Atribuição\n");
		// Copia valor a ser atribuído para %eax
		fprintf(fp, "\tmovq %s(%%rip), %%rax\n", tac->op1->text);
		// Coloca valor em %eax para a variável do lado esquerdo da atribuição
		fprintf(fp, "\tmovq %%rax, %s(%%rip)\n", tac->res->text);
	}

	// Label para fazer os jumps (condicionais e incondicionais)
	else if(tac->opcode == TAC_LABEL) {
		fprintf(fp, "\t# Label\n");
		// Apenas coloca o label no código assembly
		fprintf(fp, "\t%s:\n", tac->res->text);
	}

	// Jump Incondicional
	else if(tac->opcode == TAC_JMP) {
		fprintf(fp, "\t# Pulo incondicional\n");
		fprintf(fp, "\tjmp %s\n", tac->res->text);
	}

	// Jump Condicional (Jump If False)
	else if(tac->opcode == TAC_JF) {
		fprintf(fp, "\t# Pulo condicional\n");
		// Move o operador a ser testado falso para o registrador %eax
		fprintf(fp, "\tmovl %s(%%rip), %%eax\n", tac->op1->text);
		// Faz comparação com false
		fprintf(fp, "\tcmpl $0, %%eax\n");
		// Pula para o label se realmente for false (igual a zero)
		fprintf(fp, "\tje %s\n", tac->res->text);
	}

	else if(tac->opcode == TAC_PRINT) {
		fprintf(fp, "\t# Print\n");
		// Se for uma string
		if(tac->res->type == SYMBOL_LIT_STRING) {
			fprintf(fp, "\tleaq %s(%%rip), %%rdi\n", tac->res->strName);
			fprintf(fp, "\tmovb $0, %%al\n");
			fprintf(fp, "\tcallq _printf\n");
		}
		// Se for um dígito
		else {
			// Se for um escalar
			if(tac->res->type == SYMBOL_LIT_INTEGER) {
				fprintf(fp, "\tleaq __strDigit(%%rip), %%rdi\n");
				fprintf(fp, "\tmovl $%s, %%esi\n", tac->res->text);
				fprintf(fp, "\tmovb $0, %%al\n");
				fprintf(fp, "\tcallq _printf\n");
			}
			// Se for uma variável
			else if (tac->res->type == SYMBOL_SCALAR) {
				fprintf(fp, "\tleaq __strDigit(%%rip), %%rdi\n");
				fprintf(fp, "\tmovl %s(%%rip), %%esi\n", tac->res->text);
				fprintf(fp, "\tmovb $0, %%al\n");
				fprintf(fp, "\tcallq _printf\n");
			}
		}
	}

	// ...

   // Vai para a próxima TAC
   generateAsmFromTac(tac->next);
}


// Gera código assembly para operações binárias de soma, subtração e multiplicação
void asmBinaryOperation(TAC_NODE *tac, char *mnemonic) {
	// Copia os operandos para %eax e %ebx
	// Se for literal, precisa ser modo imediato
	if(tac->op1->type == SYMBOL_LIT_INTEGER) {                    // TODO: char, float, vetor, ...
		fprintf(fp, "\tmovq $%s, %%rax\n", tac->op1->text);

	}
	// Caso não seja literal, acessa em relação ao %rip
	else {
		fprintf(fp, "\tmovq %s(%%rip), %%rax\n", tac->op1->text);
	}

	// Mesma coisa para o operando 2
	if(tac->op2->type == SYMBOL_LIT_INTEGER) {                    // TODO: char, float, vetor, ...
		fprintf(fp, "\tmovq $%s, %%rbx\n", tac->op2->text);
	}
	else {
		fprintf(fp, "\tmovq %s(%%rip), %%rbx\n", tac->op2->text);
	}

	// Faz a operação (resultado fica em %eax)
	fprintf(fp, "\t%s %%rbx, %%rax\n", mnemonic);
	// Coloca resultado da operação para o campo resultado da TAC (na sessão de dados do assembly)
	fprintf(fp, "\tmovq %%rax, %s(%%rip)\n", tac->res->text);
}


// Gera código assembly para operações de comparação (igual, diferente, maior que, menor que, maior igual que, menor igual que)
void asmComparisonOperation(TAC_NODE *tac, char *mnemonic) {
	// Copia os operandos para %eax e %ebx
	// Se for literal, precisa ser modo imediato
	if(tac->op1->type == SYMBOL_LIT_INTEGER) {                    // TODO: char, float, vetor, ...
		fprintf(fp, "\tmovl $%s, %%eax\n", tac->op1->text);

	}
	// Caso não seja literal, acessa em relação ao %rip
	else {
		fprintf(fp, "\tmovl %s(%%rip), %%eax\n", tac->op1->text);
	}

	// Mesma coisa para o operando 2
	if(tac->op2->type == SYMBOL_LIT_INTEGER) {                    // TODO: char, float, vetor, ...
		fprintf(fp, "\tmovl $%s, %%ebx\n", tac->op2->text);
	}
	else {
		fprintf(fp, "\tmovl %s(%%rip), %%ebx\n", tac->op2->text);
	}

	// Faz a comparação
	fprintf(fp, "\tcmpl %%ebx, %%eax\n");

	// Cria os labels para efetuar os pulos
	HASH_NODE *label1 = makeLabel();
	HASH_NODE *label2 = makeLabel();

	// Cria a lógica dos operadore de comparação (baseado nas flags setadas pelo operador 'cmp' já feito)
	fprintf(fp, "\t%s %s\n", mnemonic, label1->text);
	fprintf(fp, "\tmovl $0, %%eax\n");
	fprintf(fp, "\tmovl %%eax, %s(%%rip)\n", tac->res->text);
	fprintf(fp, "\tjmp %s\n", label2->text);
	fprintf(fp, "\t%s:\n", label1->text);
	fprintf(fp, "\tmovl $1, %%eax\n");
	fprintf(fp, "\tmovl %%eax, %s(%%rip)\n", tac->res->text);
	fprintf(fp, "\t%s:\n", label2->text);
}


/* Abaixo, funções para colocar dados na sessão de dados do assembly.
 * -> Declarações globais de escalares e vetores
 * -> Strings usadas com o comando print
 * -> Temporaríos gerados pelas TACs (precisa percorrer as TACs uma vez antes de começar a gerar o assembly)
 */


// Percorre as TACs e coloca na sessão de dados do assembly as variáveis globais
void addVarsToAsm(TAC_NODE *tac) {
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
			// Se valor for FALSE, coloca valor zero. Se valor for TRUE, coloca valor 1
			if(!strcmp("TRUE", tac->op1->text))
				fprintf(fp, "\t.long 1\n");
			else
				fprintf(fp, "\t.long 0\n");
		}
	}

	// Declarações de variáveis vetores globais
	else if(tac->opcode == TAC_VECDECL) {
		// TODO: vetores vai ser uma lista, mas incorre nos mesmos problemas de tipos que faltam do anterior
	}

	// Vai para a próxima TAC
   addVarsToAsm(tac->next);
}


// Percorre a tabela de símbolos e coloca temporários na sessão de dados do assembly
void addTempsToAsm() {
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


// Percorre a tabela de símbolos e coloca strings na sessão de strings do assembly
void addStringsToAsm() {
	char *prefix = "__temp";
	int i;
	HASH_NODE *node;
	// Percorre todo índice da tabela de símbolos
	for(i=0; i<HASH_SIZE; i++) {
		if(HASH_TABLE[i] != NULL) {
			// Percorre toda a lista encadeada dos índices
			for(node=HASH_TABLE[i]; node; node=node->next) {
				// Se for um literal string
				if(node->type == SYMBOL_LIT_STRING) {
					// Coloca um nome para a string (para referenciar no assembly quando der print)
					node->strName = createStr(node->strName);
					// Coloca string na sessão de dados do assembly
					fprintf(fp, "%s:\n", node->strName);
					fprintf(fp, "\t.asciz %s\n", node->text);
				}
			}
		}
	}
	// Adiciona símbolo final para imprimir inteiros
	fprintf(fp, "__strDigit:\n");
	fprintf(fp, "\t.asciz \"%%d\"\n");
}