#ifndef AST_H
#define AST_H

#include <stdio.h>

typedef enum {
    NO_PROGRAMA,
    NO_METODO,
    NO_DECLARACAO,
    NO_WHILE,
    NO_ATRIBUICAO,
    NO_PREFIXO,
    NO_POSFIXO,
    NO_BINARIO,
    NO_UNARIO,
    NO_IDENTIFICADOR,
    NO_INT,
    NO_DOUBLE,
    NO_STRING,
    NO_CHAR,
    NO_BOOLEAN,
    NO_NULL
} TipoNo;

typedef struct NoAst {
    TipoNo tipo_no;
    char *nome;          
    const char *tipo;    
    union {
        int    ival;
        double dval;
        char   cval;
    } valor;
    struct NoAst *a;     
    struct NoAst *b;     
    struct NoAst *prox;  
} NoAst;

NoAst *ast_criar_no(TipoNo tipo_no, const char *nome, const char *tipo, NoAst *a, NoAst *b);

NoAst *ast_criar_identificador(const char *nome);
NoAst *ast_criar_int(int valor);
NoAst *ast_criar_double(double valor);
NoAst *ast_criar_char(char valor);
NoAst *ast_criar_boolean(int valor);

NoAst *ast_anexar(NoAst *lista, NoAst *novo);

void ast_liberar(NoAst *no);

void ast_imprimir(const NoAst *no, FILE *saida);

#endif