%{
#include "parser.tab.h"
#include <stdlib.h>
%}

%%

/* Palavras-chave do escopo de métodos e retornos */
"public"        { return PUBLIC; }
"private"       { return PRIVATE; }
"protected"     { return PROTECTED; }
"static"        { return STATIC; }
"void"          { return VOID; }
"int"           { return INT; }
"return"        { return RETURN; }

/* Identificadores (Nomes de classes, métodos e variáveis) */
[a-zA-Z_][a-zA-Z0-9_]* { return ID; }

/* Operadores estruturais de métodos e blocos */
"{"             { return LBRACE; }
"}"             { return RBRACE; }
","             { return COMMA; }

/* Regras originais do colega */
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