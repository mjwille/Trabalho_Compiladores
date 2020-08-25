%{

#include "tokens.h"
#include "hash.h"

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
[+-]?[0-9ABCDEF]+                      { hashInsert(yytext); return LIT_INTEGER; }
[+-]?[0-9ABCDEF]*\.[0-9ABCDEF]+        { hashInsert(yytext); return LIT_FLOAT;   }
"TRUE"                                 { hashInsert(yytext); return LIT_TRUE;    }
"FALSE"                                { hashInsert(yytext); return LIT_FALSE;   }
'.'                                    { hashInsert(yytext); return LIT_CHAR;    }
\"[^\"]*\"                             { hashInsert(yytext); return LIT_STRING;  }

 /* Identificadores */
[A-Za-z@_]+[A-Za-z@_0-9]*              { hashInsert(yytext); return TK_IDENTIFIER; }

 /* Comentários */
"//".*     {}

"/*"             { BEGIN(COMMENT); }
<COMMENT>"\n"    { lineNumber++;   }
<COMMENT>.       {}
<COMMENT>"*/"    { BEGIN(INITIAL); }

 /* Ignorar espaços e tabs */
[ \t]      {}

 /* Contador de linhas */
"\n"       { lineNumber++; }

 /* Considerar todo o resto como erro */
.          { return TOKEN_ERROR; }

%%

// Definição do trabalho: função que retorna se o análisador léxico está rodando
int isRunning()
{
	return running;
}

// Definição do trabalho: função que retorna o número da linha atual
int getLineNumber()
{
	return lineNumber;
}

// Fim do arquivo (da análise léxica).
int yywrap()
{
	running = 0;
	return 1;
}

#include "main.c"