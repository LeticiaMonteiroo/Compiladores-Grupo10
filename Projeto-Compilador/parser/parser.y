%{
#include <stdio.h>
#include <stdlib.h>

int yylex(void);
void yyerror(const char *s);
%}

/* 1. Declaração de todos os tokens que vêm do Flex */
%token NUM ID
%token INC DEC PLUS MINUS TIMES DIVIDE MOD
%token EQ NEQ GTE LTE GT LT
%token AND OR NOT
%token URSHIFT LSHIFT RSHIFT BIT_AND BIT_OR BIT_XOR BIT_NOT
%token PLUS_ASSIGN MINUS_ASSIGN TIMES_ASSIGN DIVIDE_ASSIGN MOD_ASSIGN ASSIGN
%token LPAREN RPAREN LBRACE RBRACE COMMA SEMI
%token PUBLIC PRIVATE PROTECTED STATIC VOID INT RETURN

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
%right NOT BIT_NOT INC DEC UMINUS 

%%

/* Regra raiz atualizada para aceitar métodos e instruções soltas */
programa:
    programa elemento
  | elemento
  ;

elemento:
    metodo
  | instrucao
  ;

/* 3. Escopo do Usuário: Métodos, Parâmetros e Retorno */
metodo:
    modificadores tipo ID LPAREN parametros_opt RPAREN bloco
  ;

modificadores:
    modificadores modificador
  | /* vazio */
  ;

modificador:
    PUBLIC | PRIVATE | PROTECTED | STATIC
  ;

tipo:
    INT | VOID | ID  /* ID permite tipos customizados ou objetos como 'String' */
  ;

parametros_opt:
    lista_parametros
  | /* vazio */
  ;

lista_parametros:
    parametro
  | lista_parametros COMMA parametro
  ;

parametro:
    tipo ID
  ;

bloco:
    LBRACE lista_instrucoes RBRACE
  ;

lista_instrucoes:
    lista_instrucoes instrucao
  | /* vazio */
  ;

instrucao:
    expressao SEMI
  | RETURN expressao SEMI
  | RETURN SEMI
  | bloco /* Permite blocos aninhados, como for/if no futuro */
  ;

/* 4. Expansão das regras gramaticais para os operadores e chamadas */
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
  | LPAREN expressao RPAREN
  | NUM
  | ID                              /* Suporte a variáveis locais nas expressões */
  | ID LPAREN argumentos_opt RPAREN /* Suporte a chamadas de métodos */
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