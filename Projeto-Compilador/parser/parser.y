%{
#include <stdio.h>
#include <stdlib.h>

int yylex(void);
void yyerror(const char *s);
%}

/* 1. Declaração de todos os tokens que vêm do Flex */
%token NUM
%token INC DEC PLUS MINUS TIMES DIVIDE MOD
%token EQ NEQ GTE LTE GT LT
%token AND OR NOT
%token URSHIFT LSHIFT RSHIFT BIT_AND BIT_OR BIT_XOR BIT_NOT
%token PLUS_ASSIGN MINUS_ASSIGN TIMES_ASSIGN DIVIDE_ASSIGN MOD_ASSIGN ASSIGN
%token LPAREN RPAREN SEMI

/* 2. Regras de Precedência e Associatividade (Da MENOR para a MAIOR precedência) */
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
%right NOT BIT_NOT INC DEC UMINUS /* UMINUS é um token falso (pseudo-token) para o menos unário */

%%

/* Regra raiz para aceitar múltiplas expressões no arquivo */
programa:
    programa expressao SEMI
  | expressao SEMI
  ;

/* 3. Expansão das regras gramaticais para os operadores */
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
  | MINUS expressao %prec UMINUS  /* Garante que o menos unário (ex: -5) tenha a maior precedência */
  | LPAREN expressao RPAREN
  | NUM
  ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Erro sintático: %s\n", s);
}

int main(void) {
    // Inicia a análise sintática
    if (yyparse() == 0) {
        printf("Análise concluída com sucesso.\n");
    }
    return 0;
}