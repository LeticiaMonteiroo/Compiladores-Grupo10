%{
#include <stdio.h>
#include <stdlib.h>

int yylex(void);
void yyerror(const char *s);
extern int yylineno;
%}

%union {
    int    ival;
    double dval;
    char   cval;
    char  *sval;
    int    boolval;
}


/* Palavras-chave de classe / membros */

%token CLASS PUBLIC PRIVATE PROTECTED STATIC VOID RETURN
%token WHILE


/* Tipos */

%token TYPE_INT TYPE_DOUBLE TYPE_FLOAT TYPE_BOOLEAN TYPE_CHAR
%token TYPE_LONG TYPE_SHORT TYPE_BYTE TYPE_STRING


/* Literais */

%token <ival>    INT_LITERAL
%token <dval>    DOUBLE_LITERAL
%token <sval>    STRING_LITERAL
%token <cval>    CHAR_LITERAL
%token <boolval> BOOLEAN_LITERAL
%token NULL_LITERAL


/* Identificador */

%token <sval> IDENTIFIER


/* Operadores */

%token INC DEC PLUS MINUS TIMES DIVIDE MOD

%token EQ NEQ GTE LTE GT LT

%token AND OR NOT

%token URSHIFT LSHIFT RSHIFT BIT_AND BIT_OR BIT_XOR BIT_NOT

%token PLUS_ASSIGN MINUS_ASSIGN TIMES_ASSIGN DIVIDE_ASSIGN MOD_ASSIGN ASSIGN


/* Delimitadores */

%token LBRACE RBRACE LPAREN RPAREN
%token LBRACKET RBRACKET
%token SEMICOLON COMMA DOT


/* Precedência dos operadores lógicos */

%left OR
%left AND
%right NOT


%start programa

%%


/* =========================================================
   REGRA RAIZ
   ========================================================= */

programa
    : modificador_classe_opt CLASS IDENTIFIER LBRACE lista_membros RBRACE
        {
            printf("Programa reconhecido: classe '%s'\n", $3);
            free($3);
        }
    ;


modificador_classe_opt
    : PUBLIC
    |
    ;


/* =========================================================
   LISTA DE MEMBROS
   ========================================================= */

lista_membros
    :
    | lista_membros membro
    ;


membro
    : membro_metodo
    ;


/* =========================================================
   MÉTODOS
   ========================================================= */

membro_metodo
    : modificadores_membro tipo_retorno IDENTIFIER
      LPAREN RPAREN
      LBRACE lista_comandos RBRACE
        {
            printf("Metodo reconhecido: %s\n", $3);
            free($3);
        }
    ;


modificadores_membro
    : PUBLIC
    | PRIVATE
    | PROTECTED
    | PUBLIC STATIC
    | PRIVATE STATIC
    | PROTECTED STATIC
    | STATIC
    ;


tipo_retorno
    : VOID
    | TYPE_INT
    | TYPE_DOUBLE
    | TYPE_FLOAT
    | TYPE_BOOLEAN
    | TYPE_CHAR
    | TYPE_LONG
    | TYPE_SHORT
    | TYPE_BYTE
    | TYPE_STRING
    ;


/* =========================================================
   COMANDOS
   ========================================================= */

lista_comandos
    :
    | lista_comandos comando
    ;


comando
    : comando_while
    ;


/* =========================================================
   WHILE
   ========================================================= */

comando_while
    : WHILE LPAREN expressao RPAREN LBRACE lista_comandos RBRACE
        {
            printf("While reconhecido\n");
        }
    ;


/* =========================================================
   EXPRESSÕES
   ========================================================= */

expressao
    : expressao OR expressao_and
        {
            printf("Operador logico OR reconhecido\n");
        }
    | expressao_and
    ;


expressao_and
    : expressao_and AND expressao_not
        {
            printf("Operador logico AND reconhecido\n");
        }
    | expressao_not
    ;


expressao_not
    : NOT expressao_not
        {
            printf("Operador logico NOT reconhecido\n");
        }
    | expressao_relacional
    ;


expressao_relacional
    : operando EQ operando
        {
            printf("Operador relacional == reconhecido\n");
        }
    | operando NEQ operando
        {
            printf("Operador relacional != reconhecido\n");
        }
    | operando GTE operando
        {
            printf("Operador relacional >= reconhecido\n");
        }
    | operando LTE operando
        {
            printf("Operador relacional <= reconhecido\n");
        }
    | operando GT operando
        {
            printf("Operador relacional > reconhecido\n");
        }
    | operando LT operando
        {
            printf("Operador relacional < reconhecido\n");
        }
    | operando
    ;


operando
    : IDENTIFIER
        {
            printf("Expressao: identificador '%s'\n", $1);
            free($1);
        }

    | INT_LITERAL
        {
            printf("Expressao: literal int %d\n", $1);
        }

    | DOUBLE_LITERAL
        {
            printf("Expressao: literal double %f\n", $1);
        }

    | STRING_LITERAL
        {
            printf("Expressao: literal string %s\n", $1);
            free($1);
        }

    | CHAR_LITERAL
        {
            printf("Expressao: literal char '%c'\n", $1);
        }

    | BOOLEAN_LITERAL
        {
            printf("Expressao: literal boolean %d\n", $1);
        }

    | NULL_LITERAL
        {
            printf("Expressao: literal null\n");
        }

    | LPAREN expressao RPAREN
    ;


%%


void yyerror(const char *s) {
    fprintf(
        stderr,
        "Erro de sintaxe (linha %d): %s\n",
        yylineno,
        s
    );
}


int main(void) {

    if (yyparse() == 0) {
        printf("Analise sintatica concluida com sucesso.\n");
        return 0;
    }

    return 1;
}