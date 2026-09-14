%{
#include "parser.tab.h"
%}

%%

[0-9]+          { yylval = atoi(yytext); return NUM; }

"++"            { return INC; }
"--"            { return DEC; }
"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return TIMES; }
"/"             { return DIVIDE; }
"%"             { return MOD; }

"=="            { return EQ; }
"!="            { return NEQ; }
">="            { return GTE; }
"<="            { return LTE; }
">"             { return GT; }
"<"             { return LT; }

"&&"            { return AND; }
"||"            { return OR; }
"!"             { return NOT; }

">>>"           { return URSHIFT; }
"<<"            { return LSHIFT; }
">>"            { return RSHIFT; }
"&"             { return BIT_AND; }
"|"             { return BIT_OR; }
"^"             { return BIT_XOR; }
"~"             { return BIT_NOT; }

"+="            { return PLUS_ASSIGN; }
"-="            { return MINUS_ASSIGN; }
"*="            { return TIMES_ASSIGN; }
"/="            { return DIVIDE_ASSIGN; }
"%="            { return MOD_ASSIGN; }
"="             { return ASSIGN; }

";"             { return SEMI; }
"("             { return LPAREN; }
")"             { return RPAREN; }

[ \t\n]+        { }
.               { printf("Caractere inválido: %s\n", yytext); }

%%