#include <stdlib.h>
#include <string.h>
#include "tabela_simbolos.h"

static const char *ROTULOS_CATEGORIA[] = { "classe", "metodo", "variavel", "parametro" };

static const char *ROTULOS_ESCOPO[] = { "global", "metodo", "bloco" };

static Escopo *criar_escopo(TabelaSimbolos *tabela, TipoEscopo tipo, const char *rotulo, Escopo *pai)
{
    Escopo *escopo = calloc(1, sizeof(Escopo));

    if (escopo == NULL) {
        return NULL;
    }
    if (rotulo != NULL) {
        escopo->rotulo = strdup(rotulo);
        if (escopo->rotulo == NULL) {
            free(escopo);
            return NULL;
        }
    }
    escopo->id = tabela->total_escopos++;
    escopo->nivel = (pai != NULL) ? pai->nivel + 1 : 0;
    escopo->tipo = tipo;
    escopo->pai = pai;

    if (tabela->escopos_fim == NULL) {
        tabela->escopos = escopo;
    } else {
        tabela->escopos_fim->prox_escopo = escopo;
    }
    tabela->escopos_fim = escopo;
    return escopo;
}

TabelaSimbolos *tabela_criar(void)
{
    TabelaSimbolos *tabela = calloc(1, sizeof(TabelaSimbolos));

    if (tabela == NULL) {
        return NULL;
    }
    tabela->global = criar_escopo(tabela, ESCOPO_GLOBAL, NULL, NULL);
    if (tabela->global == NULL) {
        free(tabela);
        return NULL;
    }
    tabela->atual = tabela->global;
    return tabela;
}

static void liberar_simbolo(Simbolo *simbolo)
{
    free(simbolo->nome);
    free(simbolo->tipo);
    free(simbolo);
}

void tabela_liberar(TabelaSimbolos *tabela)
{
    Simbolo *simbolo;
    Escopo *escopo;

    if (tabela == NULL) {
        return;
    }

    simbolo = tabela->inicio;
    while (simbolo != NULL) {
        Simbolo *prox = simbolo->prox;

        liberar_simbolo(simbolo);
        simbolo = prox;
    }

    escopo = tabela->escopos;
    while (escopo != NULL) {
        Escopo *prox = escopo->prox_escopo;

        free(escopo->rotulo);
        free(escopo);
        escopo = prox;
    }
    free(tabela);
}

Escopo *tabela_entrar_escopo(TabelaSimbolos *tabela, TipoEscopo tipo, const char *rotulo)
{
    Escopo *escopo;

    if (tabela == NULL) {
        return NULL;
    }
    escopo = criar_escopo(tabela, tipo, rotulo, tabela->atual);
    if (escopo != NULL) {
        tabela->atual = escopo;
    }
    return escopo;
}

void tabela_sair_escopo(TabelaSimbolos *tabela)
{
    if (tabela != NULL && tabela->atual != NULL && tabela->atual->pai != NULL) {
        tabela->atual = tabela->atual->pai;
    }
}

const Escopo *tabela_escopo_atual(const TabelaSimbolos *tabela)
{
    return (tabela != NULL) ? tabela->atual : NULL;
}

static const Simbolo *buscar_no_escopo(const Escopo *escopo, const char *nome)
{
    const Simbolo *simbolo;

    for (simbolo = escopo->primeiro; simbolo != NULL; simbolo = simbolo->prox_no_escopo) {
        if (strcmp(simbolo->nome, nome) == 0) {
            return simbolo;
        }
    }
    return NULL;
}

static int categoria_de_variavel(CategoriaSimbolo categoria)
{
    return categoria == SIMBOLO_VARIAVEL || categoria == SIMBOLO_PARAMETRO;
}

const Simbolo *tabela_buscar_em(const Escopo *escopo, const char *nome)
{
    if (nome == NULL) {
        return NULL;
    }
    for (; escopo != NULL; escopo = escopo->pai) {
        const Simbolo *simbolo = buscar_no_escopo(escopo, nome);

        if (simbolo != NULL) {
            return simbolo;
        }
    }
    return NULL;
}

const Simbolo *tabela_buscar(const TabelaSimbolos *tabela, const char *nome)
{
    if (tabela == NULL) {
        return NULL;
    }
    return tabela_buscar_em(tabela->atual, nome);
}

const Simbolo *tabela_buscar_qualquer(const TabelaSimbolos *tabela, const char *nome)
{
    const Simbolo *simbolo;

    if (tabela == NULL || nome == NULL) {
        return NULL;
    }
    for (simbolo = tabela->inicio; simbolo != NULL; simbolo = simbolo->prox) {
        if (strcmp(simbolo->nome, nome) == 0) {
            return simbolo;
        }
    }
    return NULL;
}

const Simbolo *tabela_buscar_conflito(const TabelaSimbolos *tabela, const char *nome, CategoriaSimbolo categoria)
{
    const Escopo *escopo;
    const Simbolo *simbolo;

    if (tabela == NULL || nome == NULL) {
        return NULL;
    }

    simbolo = buscar_no_escopo(tabela->atual, nome);
    if (simbolo != NULL || !categoria_de_variavel(categoria)) {
        return simbolo;
    }

    /* sobe pelos escopos do mesmo metodo; o global (classe e metodos) nao entra */
    for (escopo = tabela->atual->pai; escopo != NULL && escopo->tipo != ESCOPO_GLOBAL; escopo = escopo->pai) {
        simbolo = buscar_no_escopo(escopo, nome);
        if (simbolo != NULL && categoria_de_variavel(simbolo->categoria)) {
            return simbolo;
        }
    }
    return NULL;
}

int tabela_inserir(TabelaSimbolos *tabela, const char *nome, const char *tipo, CategoriaSimbolo categoria)
{
    Simbolo *simbolo;

    if (tabela == NULL || nome == NULL || tabela_buscar_conflito(tabela, nome, categoria) != NULL) {
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
    simbolo->ordem = tabela->total;
    simbolo->escopo = tabela->atual;

    if (tabela->fim == NULL) {
        tabela->inicio = simbolo;
    } else {
        tabela->fim->prox = simbolo;
    }
    tabela->fim = simbolo;

    if (tabela->atual->ultimo == NULL) {
        tabela->atual->primeiro = simbolo;
    } else {
        tabela->atual->ultimo->prox_no_escopo = simbolo;
    }
    tabela->atual->ultimo = simbolo;

    tabela->total++;
    return 1;
}

static void imprimir_simbolo(const Simbolo *simbolo, FILE *saida, int recuo)
{
    fprintf(saida, "%*s%s %s", recuo, "", ROTULOS_CATEGORIA[simbolo->categoria], simbolo->nome);
    if (simbolo->tipo != NULL) {
        fprintf(saida, " : %s", simbolo->tipo);
    }
    fputc('\n', saida);
}

void tabela_imprimir(const TabelaSimbolos *tabela, FILE *saida)
{
    const Simbolo *atual;

    if (tabela == NULL) {
        return;
    }

    for (atual = tabela->inicio; atual != NULL; atual = atual->prox) {
        imprimir_simbolo(atual, saida, 0);
    }
}

void tabela_imprimir_escopos(const TabelaSimbolos *tabela, FILE *saida)
{
    const Escopo *escopo;
    const Simbolo *simbolo;

    if (tabela == NULL) {
        return;
    }

    for (escopo = tabela->escopos; escopo != NULL; escopo = escopo->prox_escopo) {
        fprintf(saida, "%*sescopo %s", 2 * escopo->nivel, "", ROTULOS_ESCOPO[escopo->tipo]);
        if (escopo->rotulo != NULL) {
            fprintf(saida, " %s", escopo->rotulo);
        }
        fputc('\n', saida);
        for (simbolo = escopo->primeiro; simbolo != NULL; simbolo = simbolo->prox_no_escopo) {
            imprimir_simbolo(simbolo, saida, 2 * escopo->nivel + 2);
        }
    }
}