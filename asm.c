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
// Variável auxiliar para guardar o nome do último vetor lido
char *prevVec = NULL;


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
	addVecsToAsm(tac);
	addTempsToAsm();
	// Adiciona strings para sessão de strings
	fprintf(fp, "\n.section __TEXT, __cstring, cstring_literals\n");
	addStringsToAsm();

	// Coloca o código assembly na sessão de código (text)
	fprintf(fp, "\n.section __TEXT, __text, regular, pure_instructions\n");
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
		fprintf(fp, "\n# Início de Função\n");
		// Se for a função main
		if(!strcmp(tac->res->text, "main")) {
			fprintf(fp, ".globl _main\n");
			fprintf(fp, "_main:\n");
		}
		// Se for alguma outra função
		else {
			fprintf(fp, ".globl %s\n", tac->res->text);
			fprintf(fp, "%s:\n", tac->res->text);
		}
		// Preâmbulo
		fprintf(fp, "\tpushq %%rbp\n");
		fprintf(fp, "\tmovq %%rsp, %%rbp\n");
	}

	// Final de função
	if(tac->opcode == TAC_ENDFUN) {
		fprintf(fp, "\t# Fim de Função\n");
	}

	// Retorno de função
	else if(tac->opcode == TAC_RET) {
		fprintf(fp, "\t# Retorno\n");
		// Copia valor a ser atribuído para %eax
		// Se for literal, precisa ser modo imediato
		if(tac->op1->type == SYMBOL_LIT_INTEGER) {
			fprintf(fp, "\tmovl $%s, %%eax\n", tac->op1->text);
		}
		// Caso não seja literal, acessa em relação ao %rip (modo indexado)
		else {
			fprintf(fp, "\tmovl %s(%%rip), %%eax\n", tac->op1->text);
		}
		fprintf(fp, "\tpopq %%rbp\n");
		fprintf(fp, "\tretq\n");
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
		if(tac->op1->type == SYMBOL_LIT_INTEGER) {
			fprintf(fp, "\tmovl $%s, %%eax\n", tac->op1->text);

		}
		// Caso não seja literal, acessa pelo modo direto a variável na sessão de dados
		else {
			fprintf(fp, "\tmovl %s(%%rip), %%eax\n", tac->op1->text);
		}
		// Coloca divisor em %ebx
		// Mesma coisa para o operando 2
		if(tac->op2->type == SYMBOL_LIT_INTEGER) {
			fprintf(fp, "\tmovl $%s, %%ebx\n", tac->op2->text);
		}
		else {
			fprintf(fp, "\tmovl %s(%%rip), %%ebx\n", tac->op2->text);
		}
		// %edx precisa ser zero
		fprintf(fp, "\tmovl $0, %%edx\n");
		// Faz a operação
		fprintf(fp, "\tidiv %%ebx, %%eax\n");
		// Quociente vai para %eax e resto para %edx
		// Coloca resultado da divisão para o campo resultado da TAC (na sessão de dados do assembly)
		fprintf(fp, "\tmovl %%eax, %s(%%rip)\n", tac->res->text);
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

		fprintf(fp, "\tmovl %s(%%rip), %%eax\n", tac->op1->text);
		// Testa se é true
		fprintf(fp, "\tcmpl $1, %%eax\n");
		// Se for true (igual a 1), pula
		fprintf(fp, "\tje %s\n", label1->text);
		// Senão, coloca 1 na resposta da TAC na sessão de dados (era false)
		fprintf(fp, "\tmovl $1, %%eax\n");
		fprintf(fp, "\tmovl %%eax, %s(%%rip)\n", tac->res->text);
		// Pula para não fazer a condição do true
		fprintf(fp, "\tjmp %s\n", label2->text);
		// Se true vai pular pra cá, onde coloca zero na resposta da TAC na sessão de dados (era true)
		fprintf(fp, "\t%s:\n", label1->text);
		fprintf(fp, "\tmovl $0, %%eax\n");
		fprintf(fp, "\tmovl %%eax, %s(%%rip)\n", tac->res->text);
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
		// Se for literal, usa modo imediato
		if(tac->op1->type == SYMBOL_LIT_INTEGER) {
			fprintf(fp, "\tmovq $%s, %%rax\n", tac->op1->text);
		}
		// Se for variável , usa modo indexado
		else {
			fprintf(fp, "\tmovq %s(%%rip), %%rax\n", tac->op1->text);
		}
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

	else if(tac->opcode == TAC_READ) {
		fprintf(fp, "\t# Read\n");
		fprintf(fp, "\tleaq __strDigit(%%rip), %%rdi\n");
		fprintf(fp, "\tleaq %s(%%rip), %%rsi\n", tac->res->text);
		fprintf(fp, "\tmovb $0, %%al\n");
		fprintf(fp, "\tcallq _scanf\n");
	}

	else if(tac->opcode == TAC_VECREAD) {
		fprintf(fp, "\t# Leitura de Vetor\n");
		// Se for literal, precisa ser modo imediato
		if(tac->op2->type == SYMBOL_LIT_INTEGER) {
			int i = 4*(atoi(tac->op2->text));
			// Copia valor a ser atribuído para %eax
			fprintf(fp, "\tmovl %s+%d(%%rip), %%eax\n", tac->op1->text, i);
		}
		// Se for uma varíavel
		else {
			fprintf(fp, "\tleaq %s(%%rip), %%rbx\n", tac->op1->text);
			fprintf(fp, "\tmovslq %s(%%rip), %%rcx\n", tac->op2->text);
			fprintf(fp, "\tmovl (%%rbx,%%rcx,4), %%eax\n");
		}
		// Coloca valor em %eax para a variável do lado esquerdo da atribuição
		fprintf(fp, "\tmovl %%eax, %s(%%rip)\n", tac->res->text);
	}

	else if(tac->opcode == TAC_VECCOPY) {
		fprintf(fp, "\t# Escrita em Vetor\n");
		// Copia valor a ser atribuído para %eax
		// Se for literal, precisa ser modo imediato
		if(tac->op2->type == SYMBOL_LIT_INTEGER) {
			fprintf(fp, "\tmovl $%s, %%eax\n", tac->op2->text);
		}
		// Caso não seja literal, acessa em relação ao %rip (modo indexado)
		else {
			fprintf(fp, "\tmovl %s(%%rip), %%eax\n", tac->op2->text);
		}

		// Se índice for um inteiro
		if(tac->op1->type == SYMBOL_LIT_INTEGER) {
			int i = 4*(atoi(tac->op1->text));
			// Coloca valor em %eax para a variável do lado esquerdo da atribuição
			fprintf(fp, "\tmovl %%eax, %s+%d(%%rip)\n", tac->res->text, i);
		}
		// Se for uma variável inteira
		else {
			fprintf(fp, "\tleaq %s(%%rip), %%rbx\n", tac->res->text);
			fprintf(fp, "\tmovslq %s(%%rip), %%rcx\n", tac->op1->text);
			fprintf(fp, "\tmovl %%eax, (%%rbx,%%rcx,4)\n");
		}
	}

	else if(tac->opcode == TAC_ARG) {
		fprintf(fp, "\t# Argumento de função\n");
		// Copia valor a ser passado como parâmetro para %eax
		// Se for literal, precisa ser modo imediato
		if(tac->res->type == SYMBOL_LIT_INTEGER) {
			fprintf(fp, "\tmovl $%s, %%eax\n", tac->res->text);
		}
		// Caso não seja literal, acessa em relação ao %rip (modo indexado)
		else {
			fprintf(fp, "\tmovl %s(%%rip), %%eax\n", tac->res->text);
		}
		// Coloca valor de %eax na variável que corresponde ao parâmetro da função
		fprintf(fp, "\tmovl %%eax, %s(%%rip)\n", tac->op2->text);
	}

	else if(tac->opcode == TAC_CALL) {
		fprintf(fp, "\t# Chamada de Função\n");
		fprintf(fp, "\tcallq %s\n", tac->op1->text);
		fprintf(fp, "\tmovl %%eax, %s(%%rip)\n", tac->res->text);    // Valor de retorno vai estar em %eax
	}

   // Vai para a próxima TAC
   generateAsmFromTac(tac->next);
}


// Gera código assembly para operações binárias de soma, subtração e multiplicação
void asmBinaryOperation(TAC_NODE *tac, char *mnemonic) {
	// Copia os operandos para %eax e %ebx
	// Se for literal, precisa ser modo imediato
	if(tac->op1->type == SYMBOL_LIT_INTEGER) {
		fprintf(fp, "\tmovq $%s, %%rax\n", tac->op1->text);

	}
	// Caso não seja literal, acessa em relação ao %rip (modo indexado)
	else {
		fprintf(fp, "\tmovq %s(%%rip), %%rax\n", tac->op1->text);
	}

	// Mesma coisa para o operando 2
	if(tac->op2->type == SYMBOL_LIT_INTEGER) {
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
	if(tac->op1->type == SYMBOL_LIT_INTEGER) {
		fprintf(fp, "\tmovl $%s, %%eax\n", tac->op1->text);

	}
	// Caso não seja literal, acessa em relação ao %rip
	else {
		fprintf(fp, "\tmovl %s(%%rip), %%eax\n", tac->op1->text);
	}

	// Mesma coisa para o operando 2
	if(tac->op2->type == SYMBOL_LIT_INTEGER) {
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


// Percorre as TACs e coloca na sessão de dados do assembly as variáveis escalares
void addVarsToAsm(TAC_NODE *tac) {
	// Condição de parada do percorrimento das TACs
   if(tac == NULL)
      return;

	// Declarações de variáveis escalares
	if(tac->opcode == TAC_VARDECL) {
		// Se variável global for do tipo int
		if(tac->res->dataType == DATATYPE_INT) {
			fprintf(fp, "%s:\n", tac->res->text);
			fprintf(fp, "\t.long %s\n", tac->op1->text);
		}
		// Se variável global for do tipo boolean
		else if(tac->res->dataType == DATATYPE_BOOL) {
			fprintf(fp, "%s:\n", tac->res->text);
			// Se valor for FALSE, coloca valor zero. Se valor for TRUE, coloca valor 1
			if(!strcmp("TRUE", tac->op1->text))
				fprintf(fp, "\t.long 1\n");
			else
				fprintf(fp, "\t.long 0\n");
		}
	}

	// Argumentos (contém o parâmetro correspondente na TAC)
	if(tac->opcode == TAC_ARG) {
		fprintf(fp, "%s:\n", tac->op2->text);
		fprintf(fp, "\t.long 0\n");
	}

	// Vai para a próxima TAC
   addVarsToAsm(tac->next);
}


// Percorre as TACs e coloca na sessão de dados do assembly os vetores
void addVecsToAsm(TAC_NODE *tac) {
	// Condição de parada do percorrimento das TACs
   	if(tac == NULL)
      return;

	if(tac->opcode == TAC_VECDECL) {
		// Se vetor é um vetor sem valores de inicialização
		if(tac->op1 == NULL) {
			fprintf(fp,"%s:\n", tac->res->text);
			int i;
			for(i=0; i<atoi(tac->op2->text); i++) {
				fprintf(fp, "\t.long 0\n");
			}
		}
		// Se vetor é um vetor com valores de inicialização
		else {
			// Se prevVec já contém texto de vetor
			if(prevVec != NULL) {
				// Se for o mesmo que o vetor anterior, apenas adiciona no 'long' do assembly
				if(!strcmp(prevVec, tac->res->text)) {
					fprintf(fp, "\t.long %s\n", tac->op1->text);
				}
				// Se for diferente, cria nova variável no assembly e atualiza vetor (prevVector)
				else {
					fprintf(fp,"%s:\n", tac->res->text);
					fprintf(fp, "\t.long %s\n", tac->op1->text);
					prevVec = tac->res->text;
				}
			}
			// Senão é a primeira vez que lê uma declaração de vetor
			else {
				// Inicializa nova variável no assembly
				fprintf(fp,"%s:\n", tac->res->text);
				fprintf(fp, "\t.long %s\n", tac->op1->text);
				prevVec = tac->res->text;
			}
		}
	}

	// Vai para a próxima TAC
	addVecsToAsm(tac->next);
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
					fprintf(fp, "\n%s:\n", node->strName);
					fprintf(fp, "\t.asciz %s\n", node->text);
				}
			}
		}
	}
	// Adiciona símbolo final para imprimir inteiros
	fprintf(fp, "\n__strDigit:\n");
	fprintf(fp, "\t.asciz \"%%d\"\n");
}