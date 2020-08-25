%{

#include "tokens.h"

int running = 1;
int lineNumber = 1;

%}

%x COMMENT

%%

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

"<="           { return OPERATOR_LE;  }
">="           { return OPERATOR_GE;  }
"=="           { return OPERATOR_EQ;  }
"!="           { return OPERATOR_DIF; }

[+-]?[0-9ABCDEF]+                   { return LIT_INTEGER; }
[+-]?[0-9ABCDEF]*\.[0-9ABCDEF]+     { return LIT_FLOAT;   }
"TRUE"                              { return LIT_TRUE;    }
"FALSE"                             { return LIT_FALSE;   }
'.'                                 { return LIT_CHAR;    }
\"[^\"]*\"                              { return LIT_STRING;  }

[A-Za-z@_]+[A-Za-z@_0-9]*    { return TK_IDENTIFIER; }

"//".*     {}

"/*"       { BEGIN(COMMENT); }
<COMMENT>"\n"    { lineNumber++;   }
<COMMENT>.       {}
<COMMENT>"*/"    { BEGIN(INITIAL); }

[ \t]      {}
"\n"       { lineNumber++; }
.          { return TOKEN_ERROR; }

%%

int isRunning()
{
	return running;
}

int yywrap()
{
	running = 0;
	return 1;
}

#include "main.c"