#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include "semantico.h"

static int erros_semanticos;

static const Escopo *escopo_atual = NULL;

static int ultima_ordem_declarada = INT_MAX;

static const char *erro(const char *fmt, ...)
{
    va_list args;
    fprintf(stderr, "Erro semantico: ");
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fputc('\n', stderr);
    erros_semanticos++;
    return NULL;
}

void semantico_registrar_redeclaracao(int linha, const char *nome)
{
    fprintf(stderr, "Erro semantico (linha %d): identificador '%s' ja declarado\n", linha, nome);
    erros_semanticos++;
}

void semantico_registrar_falha_insercao(int linha, const char *nome)
{
    fprintf(stderr, "Erro semantico (linha %d): nao foi possivel registrar identificador '%s'\n", linha, nome);
    erros_semanticos++;
}

static int eh_variavel(const Simbolo *s)
{
    return s->categoria == SIMBOLO_VARIAVEL || s->categoria == SIMBOLO_PARAMETRO;
}

static const Simbolo *buscar_visivel(const char *nome, const TabelaSimbolos *tabela)
{
    return escopo_atual != NULL ? tabela_buscar_em(escopo_atual, nome)
                                : tabela_buscar(tabela, nome);
}

static void marcar_declarado(const char *nome, const TabelaSimbolos *tabela)
{
    const Simbolo *s = buscar_visivel(nome, tabela);

    if (s && s->ordem > ultima_ordem_declarada) ultima_ordem_declarada = s->ordem;
}

/* Retorna posto numérico (1..6) ou 0 se não for numérico */
static int posto_numerico(const char *t)
{
    if (!t) return 0;
    if (strcmp(t, "byte") == 0)   return 1;
    if (strcmp(t, "short") == 0)  return 2;
    if (strcmp(t, "int") == 0)    return 3;
    if (strcmp(t, "long") == 0)   return 4;
    if (strcmp(t, "float") == 0)  return 5;
    if (strcmp(t, "double") == 0) return 6;
    return 0;
}

static const char *promover(const char *a, const char *b)
{
    int pa = posto_numerico(a), pb = posto_numerico(b);
    int maior = pa > pb ? pa : pb;
    if (maior == 6) return "double";
    if (maior == 5) return "float";
    if (maior == 4) return "long";
    return "int";
}

static int pode_atribuir(const char *dest, const char *orig)
{
    int pd = posto_numerico(dest), po = posto_numerico(orig);
    if (!dest || !orig) return 0;
    if (strcmp(dest, orig) == 0) return 1;
    if (strcmp(dest, "String") == 0 && strcmp(orig, "null") == 0) return 1;
    return pd > 0 && po > 0 && pd >= po && !(pd == 3 && po < 1);
}

static const char *resolver_identificador(const char *nome, const TabelaSimbolos *tabela)
{
    const Simbolo *s = buscar_visivel(nome, tabela);

    if (!s) {
        const Simbolo *outro = tabela_buscar_qualquer(tabela, nome);

        erro("variavel '%s' usada sem ter sido declarada", nome ? nome : "?");
        if (outro && eh_variavel(outro))
            fprintf(stderr, "  nota: '%s' existe em outro escopo (%s), mas nao e visivel aqui\n",
                    nome, outro->escopo && outro->escopo->rotulo ? outro->escopo->rotulo : "bloco");
        return NULL;
    }
    if (!eh_variavel(s))
        return erro("'%s' e %s, nao uma variavel", nome, s->categoria == SIMBOLO_METODO ? "um metodo" : "uma classe");
    if (s->ordem > ultima_ordem_declarada)
        return erro("variavel '%s' usada antes de sua declaracao", nome);
    return s->tipo;
}

const char *inferir_tipo_expressao(const NoAst *no, const TabelaSimbolos *tabela)
{
    if (!no) return NULL;

    switch (no->tipo_no) {
        case NO_INT:           return "int";
        case NO_DOUBLE:        return "double";
        case NO_STRING:        return "String";
        case NO_CHAR:          return "char";
        case NO_BOOLEAN:       return "boolean";
        case NO_NULL:          return "null";
        case NO_IDENTIFICADOR: return resolver_identificador(no->nome, tabela);

        case NO_UNARIO:
        case NO_PREFIXO:
        case NO_POSFIXO: {
            const char *t = inferir_tipo_expressao(no->a, tabela);
            if (!t) return NULL;
            if (strcmp(no->nome, "!") == 0)
                return strcmp(t, "boolean") == 0 ? "boolean" : erro("operador '!' exige 'boolean', recebido '%s'", t);
            return posto_numerico(t) ? t : erro("operador '%s' exige tipo numerico, recebido '%s'", no->nome, t);
        }

        case NO_BINARIO: {
            const char *esq = inferir_tipo_expressao(no->a, tabela);
            const char *dir = inferir_tipo_expressao(no->b, tabela);
            const char *op = no->nome;
            if (!esq || !dir) return NULL;

            if (strcmp(op, "&&") == 0 || strcmp(op, "||") == 0) {
                if (strcmp(esq, "boolean") != 0 || strcmp(dir, "boolean") != 0)
                    return erro("operador logico '%s' exige operandos 'boolean' (recebido '%s' e '%s')", op, esq, dir);
                return "boolean";
            }

            if (strcmp(op, ">") == 0 || strcmp(op, "<") == 0 || strcmp(op, ">=") == 0 || strcmp(op, "<=") == 0) {
                if (!posto_numerico(esq) || !posto_numerico(dir))
                    return erro("operador relacional '%s' exige tipos numericos (recebido '%s' e '%s')", op, esq, dir);
                return "boolean";
            }

            if (strcmp(op, "==") == 0 || strcmp(op, "!=") == 0) {
                int ok = strcmp(esq, dir) == 0 || (posto_numerico(esq) && posto_numerico(dir)) ||
                         (strcmp(esq, "String") == 0 && strcmp(dir, "null") == 0) ||
                         (strcmp(esq, "null") == 0 && strcmp(dir, "String") == 0);
                return ok ? "boolean" : erro("comparacao '%s' invalida entre '%s' e '%s'", op, esq, dir);
            }

            if (strcmp(op, "+") == 0 && (strcmp(esq, "String") == 0 || strcmp(dir, "String") == 0))
                return "String";

            if (!posto_numerico(esq) || !posto_numerico(dir))
                return erro("operador aritmetico '%s' incompativel com '%s' e '%s'", op, esq, dir);
            return promover(esq, dir);
        }

        default:
            return erro("no invalido encontrado dentro de uma expressao");
    }
}

static void percorrer(const NoAst *no, const TabelaSimbolos *tabela)
{
    for (; no != NULL; no = no->prox) {
        switch (no->tipo_no) {
            case NO_PROGRAMA:
                percorrer(no->a, tabela);
                break;

            case NO_METODO: {
                const Escopo *anterior = escopo_atual;

                if (no->escopo) escopo_atual = no->escopo;
                percorrer(no->a, tabela);   /* parametros */
                percorrer(no->b, tabela);   /* corpo */
                escopo_atual = anterior;
                break;
            }

            case NO_PARAMETRO:
                marcar_declarado(no->nome, tabela);
                break;

            case NO_DECLARACAO: {
                const char *t = inferir_tipo_expressao(no->a, tabela);
                if (t && !pode_atribuir(no->tipo, t))
                    erro("nao e possivel inicializar '%s' (%s) com tipo '%s'", no->nome, no->tipo, t);
                marcar_declarado(no->nome, tabela);
                break;
            }

            case NO_ATRIBUICAO: {
                const char *tv = inferir_tipo_expressao(no->a, tabela);
                const char *te = inferir_tipo_expressao(no->b, tabela);
                if (tv && te) {
                    if (strcmp(no->nome, "=") != 0 && (!posto_numerico(tv) || !posto_numerico(te)))
                        erro("operador composto '%s' exige tipos numericos", no->nome);
                    else if (!pode_atribuir(tv, te))
                        erro("atribuicao incompativel de '%s' para '%s'", te, tv);
                }
                break;
            }

            case NO_WHILE: {
                const Escopo *anterior = escopo_atual;
                const char *tc = inferir_tipo_expressao(no->a, tabela);

                if (tc && strcmp(tc, "boolean") != 0)
                    erro("condicao do 'while' deve ser 'boolean', recebido '%s'", tc);
                if (no->escopo) escopo_atual = no->escopo;
                percorrer(no->b, tabela);
                escopo_atual = anterior;
                break;
            }

            case NO_PRINTLN:
            case NO_PRINT:
                inferir_tipo_expressao(no->a, tabela);
                break;

            case NO_PRINTF:
                for (const NoAst *arg = no->b; arg; arg = arg->prox)
                    inferir_tipo_expressao(arg, tabela);
                break;

            case NO_IF: {
                const char *tc = inferir_tipo_expressao(no->a, tabela);

                if (tc && strcmp(tc, "boolean") != 0) {
                    erro(
                        "condicao do 'if' deve ser 'boolean', recebido '%s'",
                        tc
                    );
                }

                percorrer(no->b, tabela); // Bloco verdadeiro
                percorrer(no->c, tabela); // Bloco falso
                break;
            }

            case NO_BLOCO: {
                const Escopo *anterior = escopo_atual;

                if (no->escopo) {
                    escopo_atual = no->escopo;
                }

                percorrer(no->a, tabela);

                escopo_atual = anterior;
                break;
            }

            case NO_PREFIXO:
            case NO_POSFIXO:
                inferir_tipo_expressao(no, tabela);
                break;

            default:
                break;
        }
    }
}

int analisar_semantica(const NoAst *raiz, const TabelaSimbolos *tabela)
{
    if (!raiz || !tabela) {
        erro("contexto de analise invalido");
        return erros_semanticos;
    }
    escopo_atual = tabela->global;
    ultima_ordem_declarada = -1;
    percorrer(raiz, tabela);
    escopo_atual = NULL;
    ultima_ordem_declarada = INT_MAX;
    return erros_semanticos;
}