%{
#include "parser.tab.h"
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>
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

/* Comentarios */
"//".*                      { /* ignorar comentário de linha */ }
"/*"([^*]|\*+[^*/])*\*+"/"  { /* ignorar comentário de bloco */ }

/* Palavras-chave de classe */
"class"                 { return CLASS; }
"public"                { return PUBLIC; }
"private"               { return PRIVATE; }
"protected"             { return PROTECTED; }
"static"                { return STATIC; }
"void"                  { return VOID; }
"return"                { return RETURN; }

/* Tipos primitivos */
"int"                   { return TYPE_INT; }
"double"                { return TYPE_DOUBLE; }
"float"                 { return TYPE_FLOAT; }
"boolean"               { return TYPE_BOOLEAN; }
"char"                  { return TYPE_CHAR; }
"long"                  { return TYPE_LONG; }
"short"                 { return TYPE_SHORT; }
"byte"                  { return TYPE_BYTE; }
"String"                { return TYPE_STRING; }

/* Literais */
"true"                  { yylval.boolval = 1; return BOOLEAN_LITERAL; }
"false"                 { yylval.boolval = 0; return BOOLEAN_LITERAL; }
"null"                  { return NULL_LITERAL; }
[0-9]+"."[0-9]+         { yylval.dval = atof(yytext); return DOUBLE_LITERAL; }
[0-9]+                  { yylval.ival = atoi(yytext); return INT_LITERAL; }
\"([^\"\\]|\\.)*\"      { yylval.sval = strdup(yytext); return STRING_LITERAL; }
\'([^\'\\]|\\.)\'       { yylval.cval = yytext[1]; return CHAR_LITERAL; }

/* Identificadores */
[a-zA-Z_$][a-zA-Z0-9_$]* { yylval.sval = strdup(yytext); return IDENTIFIER; }

/* Operadores aritmeticos */
"++"            { return INC; }
"--"            { return DEC; }
"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return TIMES; }
"/"             { return DIVIDE; }
"%"             { return MOD; }

/* Operadores relacionais */
"=="            { return EQ; }
"!="            { return NEQ; }
">="            { return GTE; }
"<="            { return LTE; }
">"             { return GT; }
"<"             { return LT; }

/* Operadores logicos */
"&&"            { return AND; }
"||"            { return OR; }
"!"             { return NOT; }

/* Operadores de bits */
">>>"           { return URSHIFT; }
"<<"            { return LSHIFT; }
">>"            { return RSHIFT; }
"&"             { return BIT_AND; }
"|"             { return BIT_OR; }
"^"             { return BIT_XOR; }
"~"             { return BIT_NOT; }

/* Operadores de atribuicao */
"+="            { return PLUS_ASSIGN; }
"-="            { return MINUS_ASSIGN; }
"*="            { return TIMES_ASSIGN; }
"/="            { return DIVIDE_ASSIGN; }
"%="            { return MOD_ASSIGN; }
"="             { return ASSIGN; }

/* Delimitadores e pontuacao */
"{"             { return LBRACE; }
"}"             { return RBRACE; }
"("             { return LPAREN; }
")"             { return RPAREN; }
"["             { return LBRACKET; }
"]"             { return RBRACKET; }
";"             { return SEMICOLON; }
","             { return COMMA; }
"."             { return DOT; }

[ \t\n]+        { /* ignorar espaços em branco */ }
.               { printf("Caractere inválido: %s\n", yytext); }

%%
