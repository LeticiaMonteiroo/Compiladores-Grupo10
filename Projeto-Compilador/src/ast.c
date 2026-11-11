#include <stdlib.h>
#include <string.h>
#include "ast.h"

static const char *ROTULOS[] = {
    "Programa", "Metodo", "Declaracao", "While", "Printf", "Atribuicao",
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

static char *concatenar(const char *prefixo, const char *extra)
{
    char *resultado = alocar_ou_sair(malloc(strlen(prefixo) + strlen(extra) + 1));

    strcpy(resultado, prefixo);
    strcat(resultado, extra);
    return resultado;
}

static void imprimir_descricao(const NoAst *no, FILE *saida)
{
    fprintf(saida, "%s", ROTULOS[no->tipo_no]);

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
}

static void imprimir_filhos(const NoAst *pai, const char *prefixo, FILE *saida);

static void imprimir_no(const NoAst *no, const char *prefixo, int ultimo, FILE *saida)
{
    char *prefixo_filhos = concatenar(prefixo, ultimo ? "    " : "\xe2\x94\x82   ");

    fprintf(saida, "%s%s", prefixo, ultimo ? "\xe2\x94\x94\xe2\x94\x80\xe2\x94\x80 " : "\xe2\x94\x9c\xe2\x94\x80\xe2\x94\x80 ");
    imprimir_descricao(no, saida);
    imprimir_filhos(no, prefixo_filhos, saida);
    free(prefixo_filhos);
}

static void imprimir_lista(const NoAst *lista, const char *prefixo, int ultima_lista, FILE *saida)
{
    for (; lista != NULL; lista = lista->prox) {
        imprimir_no(lista, prefixo, ultima_lista && lista->prox == NULL, saida);
    }
}

static void imprimir_grupo(const char *titulo, const NoAst *lista, const char *prefixo, int ultimo, FILE *saida)
{
    char *prefixo_filhos = concatenar(prefixo, ultimo ? "    " : "\xe2\x94\x82   ");

    fprintf(saida, "%s%s%s\n", prefixo, ultimo ? "\xe2\x94\x94\xe2\x94\x80\xe2\x94\x80 " : "\xe2\x94\x9c\xe2\x94\x80\xe2\x94\x80 ", titulo);
    imprimir_lista(lista, prefixo_filhos, 1, saida);
    free(prefixo_filhos);
}

static void imprimir_filhos(const NoAst *pai, const char *prefixo, FILE *saida)
{
    if (pai->tipo_no == NO_WHILE) {
        imprimir_grupo("Condicao", pai->a, prefixo, pai->b == NULL, saida);
        if (pai->b != NULL) {
            imprimir_grupo("Corpo", pai->b, prefixo, 1, saida);
        }
        return;
    }
    imprimir_lista(pai->a, prefixo, pai->b == NULL, saida);
    imprimir_lista(pai->b, prefixo, 1, saida);
}

void ast_imprimir(const NoAst *no, FILE *saida)
{
    for (; no != NULL; no = no->prox) {
        imprimir_descricao(no, saida);
        imprimir_filhos(no, "", saida);
    }
}