%code requires {
#include "ast.h"
#include "tabela_simbolos.h"
}

%code provides {
extern NoAst *ast_raiz;
extern TabelaSimbolos *tabela_simbolos;
}

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
    NoAst *no;
    const char *tnome;
}

%code {
NoAst *ast_raiz = NULL;
TabelaSimbolos *tabela_simbolos = NULL;

static NoAst *criar_declaracao(const char *tipo, const char *nome, NoAst *inicializacao)
{
    tabela_inserir(tabela_simbolos, nome, tipo, SIMBOLO_VARIAVEL);
    return ast_criar_no(NO_DECLARACAO, nome, tipo, inicializacao, NULL);
}
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


/* Liberação de valores */

%destructor { ast_liberar($$); } <no>
%destructor { free($$); } <sval>


/* Tipos dos não-terminais */

%type <tnome> tipo tipo_retorno
%type <no> lista_membros membro membro_metodo
%type <no> lista_comandos comando comando_declaracao comando_while
%type <no> comando_reatribuicao comando_incremento_decremento
%type <no> expressao expressao_and expressao_not expressao_relacional
%type <no> expressao_aritmetica termo fator operando


/* Precedência dos operadores lógicos */

%left OR
%left AND
%right NOT


%start programa

%%


/* REGRA RAIZ */

programa
    : modificador_classe_opt CLASS IDENTIFIER LBRACE lista_membros RBRACE
        {
            printf("Programa reconhecido: classe '%s'\n", $3);
            ast_raiz = ast_criar_no(NO_PROGRAMA, $3, NULL, $5, NULL);
            tabela_inserir(tabela_simbolos, $3, NULL, SIMBOLO_CLASSE);
            free($3);
        }
    ;

/* Variaveis */

tipo
    :TYPE_INT { $$ = "int"; }
    | TYPE_DOUBLE { $$ = "double"; }
    | TYPE_FLOAT { $$ = "float"; }
    | TYPE_BOOLEAN { $$ = "boolean"; }
    | TYPE_CHAR { $$ = "char"; }
    | TYPE_LONG { $$ = "long"; }
    | TYPE_SHORT { $$ = "short"; }
    | TYPE_BYTE { $$ = "byte"; }
    | TYPE_STRING { $$ = "String"; }
    ;
 


modificador_classe_opt
    : PUBLIC
    |
    ;


/* LISTA DE MEMBROS */

lista_membros
    :
        {
            $$ = NULL;
        }
    | lista_membros membro
        {
            $$ = ast_anexar($1, $2);
        }
    ;


membro
    : membro_metodo
    ;


/* MÉTODOS */

membro_metodo
    : modificadores_membro tipo_retorno IDENTIFIER
      LPAREN RPAREN
      LBRACE lista_comandos RBRACE
        {
            printf("Metodo reconhecido: %s\n", $3);
            $$ = ast_criar_no(NO_METODO, $3, $2, $7, NULL);
            tabela_inserir(tabela_simbolos, $3, $2, SIMBOLO_METODO);
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
    : VOID { $$ = "void"; }
    | tipo
    ;


/* COMANDOS */

lista_comandos
    :
        {
            $$ = NULL;
        }
    | lista_comandos comando
        {
            $$ = ast_anexar($1, $2);
        }
    ;

comando_declaracao
: tipo IDENTIFIER SEMICOLON
        {
            printf("Declaracao de variavel simples '%s' reconhecida\n", $2);
            $$ = criar_declaracao($1, $2, NULL);
            free($2);
        }
    | tipo IDENTIFIER ASSIGN expressao SEMICOLON
        {
            printf("Declaracao de variavel '%s' com inicializacao reconhecida\n", $2);
            $$ = criar_declaracao($1, $2, $4);
            free($2);
        }
    ;

comando
    : comando_while
    | comando_reatribuicao
    | comando_incremento_decremento
    | comando_declaracao
    ;


/* WHILE */

comando_while
    : WHILE LPAREN expressao RPAREN LBRACE lista_comandos RBRACE
        {
            printf("While reconhecido\n");
            $$ = ast_criar_no(NO_WHILE, NULL, NULL, $3, $6);
        }
    ;


/* Reatribuição de variaveis */

comando_reatribuicao
    : IDENTIFIER ASSIGN expressao SEMICOLON
        {
            printf("Reatribuicao simples '=' em '%s' reconhecida\n", $1);
            $$ = ast_criar_no(NO_ATRIBUICAO, "=", NULL, ast_criar_identificador($1), $3);
            free($1);
        }
    | IDENTIFIER PLUS_ASSIGN expressao SEMICOLON
        {
            printf("Reatribuicao composta '+=' em '%s' reconhecida\n", $1);
            $$ = ast_criar_no(NO_ATRIBUICAO, "+=", NULL, ast_criar_identificador($1), $3);
            free($1);
        }
    | IDENTIFIER MINUS_ASSIGN expressao SEMICOLON
        {
            printf("Reatribuicao composta '-=' em '%s' reconhecida\n", $1);
            $$ = ast_criar_no(NO_ATRIBUICAO, "-=", NULL, ast_criar_identificador($1), $3);
            free($1);
        }
    | IDENTIFIER TIMES_ASSIGN expressao SEMICOLON
        {
            printf("Reatribuicao composta '*=' em '%s' reconhecida\n", $1);
            $$ = ast_criar_no(NO_ATRIBUICAO, "*=", NULL, ast_criar_identificador($1), $3);
            free($1);
        }
    | IDENTIFIER DIVIDE_ASSIGN expressao SEMICOLON
        {
            printf("Reatribuicao composta '/=' em '%s' reconhecida\n", $1);
            $$ = ast_criar_no(NO_ATRIBUICAO, "/=", NULL, ast_criar_identificador($1), $3);
            free($1);
        }
    | IDENTIFIER MOD_ASSIGN expressao SEMICOLON
        {
            printf("Reatribuicao composta '%%=' em '%s' reconhecida\n", $1);
            $$ = ast_criar_no(NO_ATRIBUICAO, "%=", NULL, ast_criar_identificador($1), $3);
            free($1);
        }
    ;


/* Incremento e decremento */

comando_incremento_decremento
    : IDENTIFIER INC SEMICOLON
        {
            printf("Comando pos-incremento '++' em '%s' reconhecido\n", $1);
            $$ = ast_criar_no(NO_POSFIXO, "++", NULL, ast_criar_identificador($1), NULL);
            free($1);
        }
    | IDENTIFIER DEC SEMICOLON
        {
            printf("Comando pos-decremento '--' em '%s' reconhecido\n", $1);
            $$ = ast_criar_no(NO_POSFIXO, "--", NULL, ast_criar_identificador($1), NULL);
            free($1);
        }
    | INC IDENTIFIER SEMICOLON
        {
            printf("Comando pre-incremento '++' em '%s' reconhecido\n", $2);
            $$ = ast_criar_no(NO_PREFIXO, "++", NULL, ast_criar_identificador($2), NULL);
            free($2);
        }
    | DEC IDENTIFIER SEMICOLON
        {
            printf("Comando pre-decremento '--' em '%s' reconhecido\n", $2);
            $$ = ast_criar_no(NO_PREFIXO, "--", NULL, ast_criar_identificador($2), NULL);
            free($2);
        }
    ;


/* EXPRESSÕES */

expressao
    : expressao OR expressao_and
        {
            printf("Operador logico OR reconhecido\n");
            $$ = ast_criar_no(NO_BINARIO, "||", NULL, $1, $3);
        }
    | expressao_and
    ;


expressao_and
    : expressao_and AND expressao_not
        {
            printf("Operador logico AND reconhecido\n");
            $$ = ast_criar_no(NO_BINARIO, "&&", NULL, $1, $3);
        }
    | expressao_not
    ;


expressao_not
    : NOT expressao_not
        {
            printf("Operador logico NOT reconhecido\n");
            $$ = ast_criar_no(NO_UNARIO, "!", NULL, $2, NULL);
        }
    | expressao_relacional
    ;


expressao_relacional
    : expressao_aritmetica EQ expressao_aritmetica
        {
            printf("Operador relacional == reconhecido\n");
            $$ = ast_criar_no(NO_BINARIO, "==", NULL, $1, $3);
        }
    | expressao_aritmetica NEQ expressao_aritmetica
        {
            printf("Operador relacional != reconhecido\n");
            $$ = ast_criar_no(NO_BINARIO, "!=", NULL, $1, $3);
        }
    | expressao_aritmetica GTE expressao_aritmetica
        {
            printf("Operador relacional >= reconhecido\n");
            $$ = ast_criar_no(NO_BINARIO, ">=", NULL, $1, $3);
        }
    | expressao_aritmetica LTE expressao_aritmetica
        {
            printf("Operador relacional <= reconhecido\n");
            $$ = ast_criar_no(NO_BINARIO, "<=", NULL, $1, $3);
        }
    | expressao_aritmetica GT expressao_aritmetica
        {
            printf("Operador relacional > reconhecido\n");
            $$ = ast_criar_no(NO_BINARIO, ">", NULL, $1, $3);
        }
    | expressao_aritmetica LT expressao_aritmetica
        {
            printf("Operador relacional < reconhecido\n");
            $$ = ast_criar_no(NO_BINARIO, "<", NULL, $1, $3);
        }
    | expressao_aritmetica
    ;

/* EXPRESSÕES ARITMÉTICAS */

/* + e - */
expressao_aritmetica
    : expressao_aritmetica PLUS termo
        {
            printf("Operador aritmetico + reconhecido\n");
            $$ = ast_criar_no(NO_BINARIO, "+", NULL, $1, $3);
        }
    | expressao_aritmetica MINUS termo
        {
            printf("Operador aritmetico - reconhecido\n");
            $$ = ast_criar_no(NO_BINARIO, "-", NULL, $1, $3);
        }
    | termo
    ;


/* *, / e % */
termo
    : termo TIMES fator
        {
            printf("Operador aritmetico * reconhecido\n");
            $$ = ast_criar_no(NO_BINARIO, "*", NULL, $1, $3);
        }
    | termo DIVIDE fator
        {
            printf("Operador aritmetico / reconhecido\n");
            $$ = ast_criar_no(NO_BINARIO, "/", NULL, $1, $3);
        }
    | termo MOD fator
        {
            printf("Operador aritmetico %% reconhecido\n");
            $$ = ast_criar_no(NO_BINARIO, "%", NULL, $1, $3);
        }
    | fator
    ;


/* sinal unário, incremento e decremento */
fator
    : INC IDENTIFIER
        {
            printf("Expressao: pre-incremento '++' em '%s'\n", $2);
            $$ = ast_criar_no(NO_PREFIXO, "++", NULL, ast_criar_identificador($2), NULL);
            free($2);
        }
    | DEC IDENTIFIER
        {
            printf("Expressao: pre-decremento '--' em '%s'\n", $2);
            $$ = ast_criar_no(NO_PREFIXO, "--", NULL, ast_criar_identificador($2), NULL);
            free($2);
        }
    | IDENTIFIER INC
        {
            printf("Expressao: pos-incremento '++' em '%s'\n", $1);
            $$ = ast_criar_no(NO_POSFIXO, "++", NULL, ast_criar_identificador($1), NULL);
            free($1);
        }
    | IDENTIFIER DEC
        {
            printf("Expressao: pos-decremento '--' em '%s'\n", $1);
            $$ = ast_criar_no(NO_POSFIXO, "--", NULL, ast_criar_identificador($1), NULL);
            free($1);
        }
    | MINUS fator
        {
            printf("Operador aritmetico unario - reconhecido\n");
            $$ = ast_criar_no(NO_UNARIO, "-", NULL, $2, NULL);
        }
    | PLUS fator
        {
            printf("Operador aritmetico unario + reconhecido\n");
            $$ = ast_criar_no(NO_UNARIO, "+", NULL, $2, NULL);
        }
    | operando
    ;



operando
    : IDENTIFIER
        {
            printf("Expressao: identificador '%s'\n", $1);
            $$ = ast_criar_identificador($1);
            free($1);
        }

    | INT_LITERAL
        {
            printf("Expressao: literal int %d\n", $1);
            $$ = ast_criar_int($1);
        }

    | DOUBLE_LITERAL
        {
            printf("Expressao: literal double %f\n", $1);
            $$ = ast_criar_double($1);
        }

    | STRING_LITERAL
        {
            printf("Expressao: literal string %s\n", $1);
            $$ = ast_criar_no(NO_STRING, $1, NULL, NULL, NULL);
            free($1);
        }

    | CHAR_LITERAL
        {
            printf("Expressao: literal char '%c'\n", $1);
            $$ = ast_criar_char($1);
        }

    | BOOLEAN_LITERAL
        {
            printf("Expressao: literal boolean %d\n", $1);
            $$ = ast_criar_boolean($1);
        }

    | NULL_LITERAL
        {
            printf("Expressao: literal null\n");
            $$ = ast_criar_no(NO_NULL, NULL, NULL, NULL, NULL);
        }

    | LPAREN expressao RPAREN
        {
            $$ = $2;
        }
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