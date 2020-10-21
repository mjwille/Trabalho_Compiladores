/* Trabalho de Compiladores 2020/ERE
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul
*/

#ifndef SEMANTIC_HEADER
#define SEMANTIC_HEADER

#include "ast.h"

// Função principal que chama todos os passos da análise semântica e possivelmente reporta erro semântico no fim
void semanticAnalysis(AST_NODE *node);

// Verifica se identificadores foram declarados, colocando o datatype do identificador caso sim
void checkDeclarations(AST_NODE *node);

#endif