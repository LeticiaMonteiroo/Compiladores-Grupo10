%{
#include "parser.tab.h"
%}

%%

"class"                 { return CLASS; }
"public"                { return PUBLIC; }
"private"               { return PRIVATE; }
"static"                { return STATIC; }
"void"                  { return VOID; }

"int"                   { return TYPE_INT; }
"double"                { return TYPE_DOUBLE; }
"float"                 { return TYPE_FLOAT; }
"boolean"               { return TYPE_BOOLEAN; }
"char"                  { return TYPE_CHAR; }
"long"                  { return TYPE_LONG; }
"short"                 { return TYPE_SHORT; }
"byte"                  { return TYPE_BYTE; }
"String"                { return TYPE_STRING; }

"true"                  { yylval.boolval = 1; return BOOLEAN_LITERAL; }
"false"                 { yylval.boolval = 0; return BOOLEAN_LITERAL; }
"null"                  { return NULL_LITERAL; }

[0-9]+"."[0-9]+         { yylval.dval = atof(yytext); return DOUBLE_LITERAL; }
[0-9]+                  { yylval.ival = atoi(yytext); return INT_LITERAL; }
\"([^\"\\]|\\.)*\"      { yylval.sval = strdup(yytext); return STRING_LITERAL; }
\'([^\'\\]|\\.)\'       { yylval.cval = yytext[1]; return CHAR_LITERAL; }

[a-zA-Z_$][a-zA-Z0-9_$]* { yylval.sval = strdup(yytext); return IDENTIFIER; }

"="                     { return ASSIGN; }
"{"                     { return LBRACE; }
"}"                     { return RBRACE; }
"("                     { return LPAREN; }
")"                     { return RPAREN; }
"["                     { return LBRACKET; }
"]"                     { return RBRACKET; }
";"                     { return SEMICOLON; }
","                     { return COMMA; }
"."                     { return DOT; }

"//".*                  { /*ignorar comeentario em linha */ }
"/*"([^*]|\*+[^*/])*\*+"/" { /*ignorar comentario de bloco */ }
[ \t\n]+        { /* Ignorar espaços em branco */ }
.               { printf("Caractere inválido: %s\n", yytext); }

%%