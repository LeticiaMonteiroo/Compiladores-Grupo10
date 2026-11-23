#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantico.h"

static int erros_semanticos;

void semantico_registrar_redeclaracao(int linha, const char *nome)
{
    fprintf(stderr,
            "Erro semantico (linha %d): identificador '%s' ja declarado\n",
            linha, nome);
    erros_semanticos++;
}

void semantico_registrar_falha_insercao(int linha, const char *nome)
{
    fprintf(stderr,
            "Erro semantico (linha %d): nao foi possivel registrar identificador '%s'\n",
            linha, nome);
    erros_semanticos++;
}

typedef struct Visivel {
    const char *nome;
    struct Visivel *prox;
} Visivel;

static Visivel *visiveis;

static void marcar_visivel(const char *nome)
{
    Visivel *v = malloc(sizeof(Visivel));

    if (v == NULL) {
        fprintf(stderr, "Erro: memoria insuficiente\n");
        exit(1);
    }
    v->nome = nome;
    v->prox = visiveis;
    visiveis = v;
}

static int eh_visivel(const char *nome)
{
    const Visivel *v;

    for (v = visiveis; v != NULL; v = v->prox) {
        if (strcmp(v->nome, nome) == 0) {
            return 1;
        }
    }
    return 0;
}

static void limpar_visiveis(void)
{
    while (visiveis != NULL) {
        Visivel *prox = visiveis->prox;

        free(visiveis);
        visiveis = prox;
    }
}

static void verificar_uso(const NoAst *no, const TabelaSimbolos *tabela)
{
    const Simbolo *simbolo;

    if (eh_visivel(no->nome)) {
        return;
    }

    simbolo = tabela_buscar(tabela, no->nome);

    if (simbolo == NULL) {
        fprintf(stderr,
                "Erro semantico: variavel '%s' usada sem ter sido declarada\n",
                no->nome);
    } else if (simbolo->categoria != SIMBOLO_VARIAVEL) {
        fprintf(stderr,
                "Erro semantico: '%s' e %s, nao uma variavel\n",
                no->nome,
                simbolo->categoria == SIMBOLO_METODO ? "um metodo" : "uma classe");
    } else {
        fprintf(stderr,
                "Erro semantico: variavel '%s' usada antes de sua declaracao\n",
                no->nome);
    }
    erros_semanticos++;
}

/* Travessia recursiva da AST (filhos em a/b, irmaos em prox) */

static void percorrer(const NoAst *no, const TabelaSimbolos *tabela)
{
    for (; no != NULL; no = no->prox) {
        switch (no->tipo_no) {
            case NO_METODO:
                limpar_visiveis();
                percorrer(no->a, tabela);   /* parametros */
                percorrer(no->b, tabela);   /* corpo */
                limpar_visiveis();
                break;

            case NO_PARAMETRO:
                marcar_visivel(no->nome);
                break;

            case NO_DECLARACAO:
                percorrer(no->a, tabela);   /* inicializacao, antes de marcar */
                marcar_visivel(no->nome);
                break;

            case NO_IDENTIFICADOR:
                verificar_uso(no, tabela);
                break;

            default:
                /* ATRIBUICAO, PREFIXO, POSFIXO, WHILE, BINARIO, PRINT... */
                percorrer(no->a, tabela);
                percorrer(no->b, tabela);
                break;
        }
    }
}

int analisar_semantica(const NoAst *raiz, const TabelaSimbolos *tabela)
{
    if (raiz == NULL || tabela == NULL) {
        fprintf(stderr, "Erro semantico: contexto de analise invalido\n");
        erros_semanticos++;
        return erros_semanticos;
    }

    percorrer(raiz, tabela);
    return erros_semanticos;
}
