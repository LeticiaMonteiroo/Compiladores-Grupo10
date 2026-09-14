%{
#include <stdio.h>
#include <stdlib.h>

int yylex(void);
void yyerror(const char *s);
%}

/* -----gera campo de memoria para armazenar um tipo de valor----------------- */
%union {
    int    intval;
    double doubval;
    char   charval;
    char*  strval;
    int    boolval;
}

/* -----palavras reservadas-------------- */
%token TYPE_INT
%token TYPE_LONG
%token TYPE_BOOL
%token TYPE_DOUBLE
%token TYPE_FLOAT
%token TYPE_CHAR
%token TYPE_STRING

%token CLASS
%token PUBLIC
%token PRIVATE
%token STATIC
%token VOID

/* -----tokens para armazenar valores/campos de cada tipo-------------- */
%token <intval> INT_VALOR
%token <doubval> DOUBLE_VALOR
%token <strval> STRING_VALOR
%token <charval> CHAR_VALOR
%token <boolval> BOOLEAN_VALOR
%token NULL_VALOR

/* ----------identificador---------- */
%token <strval> IDENTIFIER

/* ----------delimitadores---------- */
%token ASSIGN                          /* = */
%token LBRACE RBRACE                   /* {} */
%token LPAREN RPAREN                   /* () */
%token LBRACKET RBRACKET               /* [] */
%token SEMICOLON                       /*  ; */
%token COMMA                           /* , */
%token DOT                             /* . */

/* ----------tipos nao-terminais---------- */
%type <strval> tipo

%%

/* classe unica com Main contendo declaraçoes de variaveis */
programa
    : PUBLIC CLASS IDENTIFIER LBRACE metodoMain RBRACE
        { printf("Classe '%s' reconhecida com sucesso.\n", $3); }
    ;

metodoMain
    : PUBLIC STATIC VOID IDENTIFIER LPAREN TYPE_STRING LBRACKET RBRACKET IDENTIFIER RPAREN LBRACE listaDeclaracoes RBRACE
        { printf("Metodo '%s' reconhecido.\n", $4); }
    ;

listaDeclaracoes
    : listaDeclaracoes declaracaoVariavel
    | /* vazio */
    ;

declaracaoVariavel
    : tipo IDENTIFIER ASSIGN INT_LITERAL SEMICOLON
        { printf("var int: %s = %s\n", $2, "..."); }
    | tipo IDENTIFIER ASSIGN DOUBLE_LITERAL SEMICOLON
        { printf("var double: %s\n", $2); }
    | tipo IDENTIFIER ASSIGN STRING_LITERAL SEMICOLON
        { printf("var string: %s\n", $2); }
    | tipo IDENTIFIER ASSIGN CHAR_LITERAL SEMICOLON
        { printf("var char: %s\n", $2); }
    | tipo IDENTIFIER ASSIGN BOOLEAN_LITERAL SEMICOLON
        { printf("var boolean: %s\n", $2); }
    | tipo IDENTIFIER ASSIGN NULL_LITERAL SEMICOLON
        { printf("var nula: %s\n", $2); }
    | tipo IDENTIFIER SEMICOLON
        { printf("var sem inicializacao: %s\n", $2); }
    ;

tipo /* agrupa os 9 tokens de tipo numa unica regra reutilizavel, devolvendo o nome do tipo como string via $$ */
    : TYPE_INT      { $$ = "int"; }
    | TYPE_DOUBLE   { $$ = "double"; }
    | TYPE_FLOAT    { $$ = "float"; }
    | TYPE_BOOLEAN  { $$ = "boolean"; }
    | TYPE_CHAR     { $$ = "char"; }
    | TYPE_LONG     { $$ = "long"; }
    | TYPE_SHORT    { $$ = "short"; }
    | TYPE_BYTE     { $$ = "byte"; }
    | TYPE_STRING   { $$ = "String"; }
    ;

%%



void yyerror(const char *s) {
    fprintf(stderr, "Erro sintático: %s\n", s);
}

int main(void) {
    yyparse();
    return 0;
}