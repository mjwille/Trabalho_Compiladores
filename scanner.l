%{

#include "tokens.h"

int Running = 1;
int lineNumber = 1;

%}

%x COMMENT

%%

"a"            {printf("A");}
"b"            {printf("B");}
"c"            {printf("C");}

%%

int main()
{
	yylex();
}

int yywrap()
{
	return 1;
}