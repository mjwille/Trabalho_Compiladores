/* Trabalho de Compiladores 2020/ERE
   Nome: Marcelo Jantsch Wille
   Universidade Federal do Rio Grande do Sul
*/

%{

#include "hash.h"
#include "ast.h"
#include "y.tab.h"

int running = 1;
int lineNumber = 1;

%}

%x COMMENT

%%

 /* Palavras Reservadas (Keywords) */
"char"         { return KW_CHAR;   }
"int"          { return KW_INT;    }
"float"        { return KW_FLOAT;  }
"bool"         { return KW_BOOL;   }
"if"           { return KW_IF;     }
"then"         { return KW_THEN;   }
"else"         { return KW_ELSE;   }
"while"        { return KW_WHILE;  }
"loop"         { return KW_LOOP;   }
"read"         { return KW_READ;   }
"print"        { return KW_PRINT;  }
"return"       { return KW_RETURN; }

 /* Caracteres Especiais */
","            { return yytext[0]; }
";"            { return yytext[0]; }
":"            { return yytext[0]; }
"("            { return yytext[0]; }
")"            { return yytext[0]; }
"["            { return yytext[0]; }
"]"            { return yytext[0]; }
"{"            { return yytext[0]; }
"}"            { return yytext[0]; }
"+"            { return yytext[0]; }
"-"            { return yytext[0]; }
"*"            { return yytext[0]; }
"/"            { return yytext[0]; }
"<"            { return yytext[0]; }
">"            { return yytext[0]; }
"="            { return yytext[0]; }
"|"            { return yytext[0]; }
"^"            { return yytext[0]; }
"~"            { return yytext[0]; }
"&"            { return yytext[0]; }
"$"            { return yytext[0]; }
"#"            { return yytext[0]; }

 /* Operadores */
"<="           { return OPERATOR_LE;  }
">="           { return OPERATOR_GE;  }
"=="           { return OPERATOR_EQ;  }
"!="           { return OPERATOR_DIF; }

 /* Literais */
[+-]?[0-9ABCDEF]+                      { yylval.symbol = hashInsert(yytext, SYMBOL_LIT_INTEGER, 0, NULL);   return LIT_INTEGER; }
[+-]?[0-9ABCDEF]*\.[0-9ABCDEF]+        { yylval.symbol = hashInsert(yytext, SYMBOL_LIT_FLOAT,   0, NULL);   return LIT_FLOAT;   }
"TRUE"                                 { yylval.symbol = hashInsert(yytext, SYMBOL_LIT_TRUE,    0, NULL);   return LIT_TRUE;    }
"FALSE"                                { yylval.symbol = hashInsert(yytext, SYMBOL_LIT_FALSE,   0, NULL);   return LIT_FALSE;   }
'.'                                    { yylval.symbol = hashInsert(yytext, SYMBOL_LIT_CHAR,    0, NULL);   return LIT_CHAR;    }
\"[^\"]*\"                             { yylval.symbol = hashInsert(yytext, SYMBOL_LIT_STRING,  0, NULL);   return LIT_STRING;  }

 /* Identificadores */
[A-Za-z@_]+[A-Za-z@_0-9]*              { yylval.symbol = hashInsert(yytext, SYMBOL_IDENTIFIER, 0, NULL); return TK_IDENTIFIER;  }

 /* Comentários */
"//".*     {}

"/*"             { BEGIN(COMMENT); }
<COMMENT>"\n"    { lineNumber++;   }
<COMMENT>.       {}
<COMMENT>"*/"    { BEGIN(INITIAL); }

 /* Ignorar espaços e tabs */
[ \t\r]    {}

 /* Contador de linhas */
"\n"       { lineNumber++; }

 /* Considerar todo o resto como erro */
.          { return TOKEN_ERROR; }

%%

// Definição do trabalho: função que retorna se o análisador léxico está rodando
int isRunning(void)
{
	return running;
}

// Definição do trabalho: função que retorna o número da linha atual
int getLineNumber(void)
{
	return lineNumber;
}

// Definição do trabalho: função que faz a inicialização das estruturas necessárias
void initMe(void)
{
	// Inicializa a tabela hash com valores default NULL
	hashInit();
}

// Função de fim da análise léxica.
int yywrap()
{
	running = 0;
	return 1;
}