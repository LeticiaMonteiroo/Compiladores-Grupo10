#include <stdlib.h>
#include <string.h>
#include "tabela_simbolos.h"

static const char *ROTULOS_CATEGORIA[] = { "classe", "metodo", "variavel" };

TabelaSimbolos *tabela_criar(void)
{
    return calloc(1, sizeof(TabelaSimbolos));
}

static void liberar_simbolo(Simbolo *simbolo)
{
    free(simbolo->nome);
    free(simbolo->tipo);
    free(simbolo);
}

void tabela_liberar(TabelaSimbolos *tabela)
{
    Simbolo *atual;

    if (tabela == NULL) {
        return;
    }

    atual = tabela->inicio;
    while (atual != NULL) {
        Simbolo *prox = atual->prox;

        liberar_simbolo(atual);
        atual = prox;
    }
    free(tabela);
}

const Simbolo *tabela_buscar(const TabelaSimbolos *tabela, const char *nome)
{
    const Simbolo *atual;

    if (tabela == NULL || nome == NULL) {
        return NULL;
    }

    for (atual = tabela->inicio; atual != NULL; atual = atual->prox) {
        if (strcmp(atual->nome, nome) == 0) {
            return atual;
        }
    }
    return NULL;
}

int tabela_inserir(TabelaSimbolos *tabela, const char *nome, const char *tipo, CategoriaSimbolo categoria)
{
    Simbolo *simbolo;

    if (tabela == NULL || nome == NULL || tabela_buscar(tabela, nome) != NULL) {
        return 0;
    }

    simbolo = calloc(1, sizeof(Simbolo));
    if (simbolo == NULL) {
        return 0;
    }

    simbolo->nome = strdup(nome);
    simbolo->tipo = (tipo != NULL) ? strdup(tipo) : NULL;
    if (simbolo->nome == NULL || (tipo != NULL && simbolo->tipo == NULL)) {
        liberar_simbolo(simbolo);
        return 0;
    }
    simbolo->categoria = categoria;

    if (tabela->fim == NULL) {
        tabela->inicio = simbolo;
    } else {
        tabela->fim->prox = simbolo;
    }
    tabela->fim = simbolo;
    tabela->total++;
    return 1;
}

void tabela_imprimir(const TabelaSimbolos *tabela, FILE *saida)
{
    const Simbolo *atual;

    if (tabela == NULL) {
        return;
    }

    for (atual = tabela->inicio; atual != NULL; atual = atual->prox) {
        fprintf(saida, "%s %s", ROTULOS_CATEGORIA[atual->categoria], atual->nome);
        if (atual->tipo != NULL) {
            fprintf(saida, " : %s", atual->tipo);
        }
        fputc('\n', saida);
    }
}