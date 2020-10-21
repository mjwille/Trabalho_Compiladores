/* Trabalho de Compiladores 2020/ERE
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul
*/

#ifndef SEMANTIC_HEADER
#define SEMANTIC_HEADER

#include "ast.h"

// Função principal que chama todos os passos da análise semântica e possivelmente reporta erro semântico no fim
void semanticAnalysis(AST_NODE *node);

// Verifica se o nodo é um dos nodos que declaram variável ou função
int isDeclaration(AST_NODE *node);

// Especifica semanticamente o tipo do identificador nas declarações de variáveis e funções, e define seu tipo (datatype)
void redefineIdentifiers(AST_NODE *node);

// Verifica se algum identificador usado no código não foi declarado
void checkUndeclared(AST_NODE *node);

// Verifica se o uso dos identificadores está compatível com sua declaração
void checkUsage(AST_NODE *node);

#endif