/* Trabalho de Compiladores 2020/ERE
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul
*/

#ifndef SEMANTIC_HEADER
#define SEMANTIC_HEADER

#include "ast.h"

// Função principal que chama todos os passos da análise semântica e possivelmente reporta erro semântico no fim
void semanticAnalysis(AST_NODE *node);

// Especifica semanticamente o tipo do identificador nas declarações de variáveis e funções, e define seu tipo (datatype)
void checkAndSetDeclarations(AST_NODE *node);

// Verifica se algum identificador usado no código não foi declarado
void checkUndeclared(AST_NODE *node);

// Verifica os tipos de dados para expressões, percorrendo recursivamente as operações de baixo pra cima e anotando valor nos nodos
void checkExprTypes(AST_NODE *node);

// Verifica se o uso dos identificadores está compatível com sua declaração
void checkUsage(AST_NODE *node);

// Função que analisa uso de tipo correto dos filhos de operadores aritméticos
void checkNumericSon(AST_NODE *node, char *sonSide);

// Função que analisa uso de tipo correto dos filhos de operadores booleanos
void checkBooleanSon(AST_NODE *node, char *sonSide);

// Verifica se o nodo é um dos nodos que representam operações numéricas
int isNumericOperator(AST_NODE *node);

// Verifica se o nodo é um dos nodos que representam operações booleanas
int isBooleanOperator(AST_NODE *node);

// Verifica se o nodo é um literal numérico compatível (CHAR, INTEGER, FLOAT)
int isNumericLiteral(AST_NODE *node);

// Verifica se o nodo é um literal booleano compatível (TRUE, FALSE)
int isBooleanLiteral(AST_NODE *node);

// Verifica se o nodo é um identificador (chamada de função, variável escalar ou vetor) com tipo numérico compatível
int isNumericIdentifier(AST_NODE *node);

// Verifica se o nodo é um identificador (chamada de função, variável escalar ou vetor) com tipo booleano compatível
int isBooleanIdentifier(AST_NODE *node);

// Verifica se o nodo é um dos nodos que declaram variável ou função
int isDeclaration(AST_NODE *node);

#endif