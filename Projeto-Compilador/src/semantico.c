#include <stdio.h>
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

int analisar_semantica(const NoAst *raiz, const TabelaSimbolos *tabela)
{
    if (raiz == NULL || tabela == NULL) {
        fprintf(stderr, "Erro semantico: contexto de analise invalido\n");
        erros_semanticos++;
    }

    return erros_semanticos;
}
