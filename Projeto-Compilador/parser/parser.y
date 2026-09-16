%{
#include <stdio.h>
#include <stdlib.h>

int yylex(void);
void yyerror(const char *s);
%}

/* -----gera campo de memoria para armazenar um tipo de valor----- */
%union {
    int    ival;
    double dval;
    char   cval;
    char*  sval;
    int    boolval;
}

/* -----palavras reservadas-------------- */
%token CLASS
%token PUBLIC PRIVATE PROTECTED STATIC VOID RETURN

/* -----tipos primitivos-------------- */
%token TYPE_INT TYPE_DOUBLE TYPE_FLOAT TYPE_BOOLEAN TYPE_CHAR
%token TYPE_LONG TYPE_SHORT TYPE_BYTE TYPE_STRING

/* -----literais tipados-------------- */
%token <ival>    INT_LITERAL
%token <dval>    DOUBLE_LITERAL
%token <sval>    STRING_LITERAL
%token <cval>    CHAR_LITERAL
%token <boolval> BOOLEAN_LITERAL
%token NULL_LITERAL

/* ----------identificador---------- */
%token <sval> IDENTIFIER

/* ----------operadores e delimitadores---------- */
%token INC DEC PLUS MINUS TIMES DIVIDE MOD
%token EQ NEQ GTE LTE GT LT
%token AND OR NOT
%token URSHIFT LSHIFT RSHIFT BIT_AND BIT_OR BIT_XOR BIT_NOT
%token PLUS_ASSIGN MINUS_ASSIGN TIMES_ASSIGN DIVIDE_ASSIGN MOD_ASSIGN ASSIGN
%token LPAREN RPAREN LBRACE RBRACE LBRACKET RBRACKET SEMICOLON COMMA DOT

/* ----------tipos dos nao-terminais---------- */
%type <sval> tipo tipo_retorno

/* 2. Precedencia e associatividade */
%right ASSIGN PLUS_ASSIGN MINUS_ASSIGN TIMES_ASSIGN DIVIDE_ASSIGN MOD_ASSIGN
%left OR
%left AND
%left BIT_OR
%left BIT_XOR
%left BIT_AND
%left EQ NEQ
%left GT LT GTE LTE
%left LSHIFT RSHIFT URSHIFT
%left PLUS MINUS
%left TIMES DIVIDE MOD
%right NOT BIT_NOT INC DEC UMINUS

%%

/* classe unica contendo variaveis e metodos  */
programa:
    PUBLIC CLASS IDENTIFIER LBRACE lista_membros RBRACE
        { printf("Classe '%s' reconhecida com sucesso.\n", $3); }
  ;

lista_membros:
    lista_membros membro
  | /* vazio */
  ;

membro:
    declaracao_variavel
  | metodo
  ;

/* 3. Metodos, parametros e retorno */
metodo:
    modificadores tipo_retorno IDENTIFIER LPAREN parametros_opt RPAREN bloco
        { printf("Metodo '%s' reconhecido.\n", $3); }
  ;

modificadores:
    modificadores modificador
  | /* vazio */
  ;

modificador:
    PUBLIC | PRIVATE | PROTECTED | STATIC
  ;

tipo_retorno:
    tipo   { $$ = $1; }
  | VOID   { $$ = "void"; }
  ;

/* agrupa os tipos primitivos + tipos customizados */
tipo:
    TYPE_INT      { $$ = "int"; }
  | TYPE_DOUBLE   { $$ = "double"; }
  | TYPE_FLOAT    { $$ = "float"; }
  | TYPE_BOOLEAN  { $$ = "boolean"; }
  | TYPE_CHAR     { $$ = "char"; }
  | TYPE_LONG     { $$ = "long"; }
  | TYPE_SHORT    { $$ = "short"; }
  | TYPE_BYTE     { $$ = "byte"; }
  | TYPE_STRING   { $$ = "String"; }
  | IDENTIFIER    { $$ = $1; }
  ;

parametros_opt:
    lista_parametros
  | /* vazio */
  ;

lista_parametros:
    parametro
  | lista_parametros COMMA parametro
  ;

/* aceita tanto "tipo nome" quanto "tipo[] nome" (ex.: String[] args do main) */
parametro:
    tipo IDENTIFIER
  | tipo LBRACKET RBRACKET IDENTIFIER
  ;

bloco:
    LBRACE lista_instrucoes RBRACE
  ;

lista_instrucoes:
    lista_instrucoes instrucao
  | /* vazio */
  ;

instrucao:
    declaracao_variavel
  | expressao SEMICOLON
  | RETURN expressao SEMICOLON
  | RETURN SEMICOLON
  | bloco /* permite blocos aninhados, como for/if no futuro */
  ;

/* declaração de variável genérica: aceita qualquer expressão como inicializador,
   não apenas um literal solto */
declaracao_variavel:
    tipo IDENTIFIER ASSIGN expressao SEMICOLON
        { printf("Variavel '%s' (%s) declarada.\n", $2, $1); }
  | tipo IDENTIFIER SEMICOLON
        { printf("Variavel '%s' (%s) declarada sem inicializacao.\n", $2, $1); }
  ;

/* 4. Expressões: operadores completos + literais tipados + chamadas de método */
expressao:
    expressao ASSIGN expressao
  | expressao PLUS_ASSIGN expressao
  | expressao MINUS_ASSIGN expressao
  | expressao TIMES_ASSIGN expressao
  | expressao DIVIDE_ASSIGN expressao
  | expressao MOD_ASSIGN expressao
  | expressao OR expressao
  | expressao AND expressao
  | expressao BIT_OR expressao
  | expressao BIT_XOR expressao
  | expressao BIT_AND expressao
  | expressao EQ expressao
  | expressao NEQ expressao
  | expressao GTE expressao
  | expressao LTE expressao
  | expressao GT expressao
  | expressao LT expressao
  | expressao LSHIFT expressao
  | expressao RSHIFT expressao
  | expressao URSHIFT expressao
  | expressao PLUS expressao
  | expressao MINUS expressao
  | expressao TIMES expressao
  | expressao DIVIDE expressao
  | expressao MOD expressao
  | NOT expressao
  | BIT_NOT expressao
  | INC expressao
  | expressao INC
  | DEC expressao
  | expressao DEC
  | MINUS expressao %prec UMINUS
  | MINUS expressao %prec UMINUS
  | LPAREN expressao RPAREN
  | IDENTIFIER                              /* variável local */
  | IDENTIFIER LPAREN argumentos_opt RPAREN /* chamada de metodo */
  | INT_LITERAL
  | DOUBLE_LITERAL
  | STRING_LITERAL
  | CHAR_LITERAL
  | BOOLEAN_LITERAL
  | NULL_LITERAL
  ;

argumentos_opt:
    lista_argumentos
  | /* vazio */
  ;

lista_argumentos:
    expressao
  | lista_argumentos COMMA expressao
  ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erro sintático: %s\n", s);
}

int main(void) {
    if (yyparse() == 0) {
        printf("Análise concluída com sucesso.\n");
    }
    return 0;
}
