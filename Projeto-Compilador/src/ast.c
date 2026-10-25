#include <stdlib.h>
#include <string.h>
#include "ast.h"

static const char *ROTULOS[] = {
    "Programa", "Metodo", "Declaracao", "While", "Atribuicao",
    "Prefixo", "Posfixo", "Binario", "Unario", "Identificador",
    "Int", "Double", "String", "Char", "Boolean", "Null"
};

static void *alocar_ou_sair(void *ptr)
{
    if (ptr == NULL) {
        fprintf(stderr, "Erro: memoria insuficiente\n");
        exit(1);
    }
    return ptr;
}

NoAst *ast_criar_no(TipoNo tipo_no, const char *nome, const char *tipo, NoAst *a, NoAst *b)
{
    NoAst *no = alocar_ou_sair(calloc(1, sizeof(NoAst)));

    no->tipo_no = tipo_no;
    no->nome = (nome != NULL) ? alocar_ou_sair(strdup(nome)) : NULL;
    no->tipo = tipo;
    no->a = a;
    no->b = b;
    return no;
}

NoAst *ast_criar_identificador(const char *nome)
{
    return ast_criar_no(NO_IDENTIFICADOR, nome, NULL, NULL, NULL);
}

NoAst *ast_criar_int(int valor)
{
    NoAst *no = ast_criar_no(NO_INT, NULL, NULL, NULL, NULL);

    no->valor.ival = valor;
    return no;
}

NoAst *ast_criar_double(double valor)
{
    NoAst *no = ast_criar_no(NO_DOUBLE, NULL, NULL, NULL, NULL);

    no->valor.dval = valor;
    return no;
}

NoAst *ast_criar_char(char valor)
{
    NoAst *no = ast_criar_no(NO_CHAR, NULL, NULL, NULL, NULL);

    no->valor.cval = valor;
    return no;
}

NoAst *ast_criar_boolean(int valor)
{
    NoAst *no = ast_criar_no(NO_BOOLEAN, NULL, NULL, NULL, NULL);

    no->valor.ival = valor;
    return no;
}

NoAst *ast_anexar(NoAst *lista, NoAst *novo)
{
    NoAst *ultimo = lista;

    if (lista == NULL) {
        return novo;
    }
    while (ultimo->prox != NULL) {
        ultimo = ultimo->prox;
    }
    ultimo->prox = novo;
    return lista;
}

void ast_liberar(NoAst *no)
{
    while (no != NULL) {
        NoAst *prox = no->prox;

        ast_liberar(no->a);
        ast_liberar(no->b);
        free(no->nome);
        free(no);
        no = prox;
    }
}

static void imprimir_no(const NoAst *no, int nivel, FILE *saida)
{
    for (; no != NULL; no = no->prox) {
        fprintf(saida, "%*s%s", nivel * 2, "", ROTULOS[no->tipo_no]);

        switch (no->tipo_no) {
            case NO_INT:     fprintf(saida, " %d", no->valor.ival); break;
            case NO_DOUBLE:  fprintf(saida, " %g", no->valor.dval); break;
            case NO_CHAR:    fprintf(saida, " '%c'", no->valor.cval); break;
            case NO_BOOLEAN: fprintf(saida, " %s", no->valor.ival ? "true" : "false"); break;
            default:
                if (no->nome != NULL) {
                    fprintf(saida, " %s", no->nome);
                }
        }

        if (no->tipo != NULL) {
            fprintf(saida, " : %s", no->tipo);
        }
        fputc('\n', saida);

        imprimir_no(no->a, nivel + 1, saida);
        imprimir_no(no->b, nivel + 1, saida);
    }
}

void ast_imprimir(const NoAst *no, FILE *saida)
{
    imprimir_no(no, 0, saida);
}