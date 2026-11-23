#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantico.h"

static int erros_semanticos;

static const NoAst *metodo_atual = NULL;

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

    if (nome == NULL) {
        return 0;
    }
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

static const char *buscar_tipo_parametro_local(const char *nome)
{
    const NoAst *param;

    if (metodo_atual == NULL || nome == NULL) {
        return NULL;
    }

    for (param = metodo_atual->a; param != NULL; param = param->prox) {
        if (param->tipo_no == NO_PARAMETRO &&
            param->nome != NULL &&
            strcmp(param->nome, nome) == 0) {
            return param->tipo;
        }
    }
    return NULL;
}

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

static int eh_tipo_numerico(const char *tipo)
{
    if (tipo == NULL) return 0;
    return strcmp(tipo, "int") == 0 ||
           strcmp(tipo, "double") == 0 ||
           strcmp(tipo, "float") == 0 ||
           strcmp(tipo, "long") == 0 ||
           strcmp(tipo, "short") == 0 ||
           strcmp(tipo, "byte") == 0;
}

static const char *promover_tipo_numerico(const char *t1, const char *t2)
{
    if (strcmp(t1, "double") == 0 || strcmp(t2, "double") == 0) return "double";
    if (strcmp(t1, "float") == 0  || strcmp(t2, "float") == 0)  return "float";
    if (strcmp(t1, "long") == 0   || strcmp(t2, "long") == 0)   return "long";
    return "int";
}

static int tipos_compativeis_atribuicao(const char *destino, const char *origem)
{
    if (destino == NULL || origem == NULL) return 0;
    if (strcmp(destino, origem) == 0) return 1;
    if (strcmp(destino, "String") == 0 && strcmp(origem, "null") == 0) return 1;

    if (eh_tipo_numerico(destino) && eh_tipo_numerico(origem)) {
        if (strcmp(destino, "double") == 0) return 1;
        if (strcmp(destino, "float") == 0 && strcmp(origem, "double") != 0) return 1;
        if (strcmp(destino, "long") == 0 &&
           (strcmp(origem, "int") == 0 || strcmp(origem, "short") == 0 || strcmp(origem, "byte") == 0)) return 1;
        if (strcmp(destino, "int") == 0 &&
           (strcmp(origem, "short") == 0 || strcmp(origem, "byte") == 0)) return 1;
    }
    return 0;
}

const char *inferir_tipo_expressao(const NoAst *no, const TabelaSimbolos *tabela)
{
    if (no == NULL) return NULL;

    switch (no->tipo_no) {
        case NO_INT:     return "int";
        case NO_DOUBLE:  return "double";
        case NO_STRING:  return "String";
        case NO_CHAR:    return "char";
        case NO_BOOLEAN: return "boolean";
        case NO_NULL:    return "null";

        case NO_IDENTIFICADOR: {
            const char *tipo_param;
            const Simbolo *simbolo;

            if (!eh_visivel(no->nome)) {
                simbolo = tabela_buscar(tabela, no->nome);
                if (simbolo == NULL) {
                    fprintf(stderr,
                            "Erro semantico: variavel '%s' usada sem ter sido declarada\n",
                            no->nome ? no->nome : "?");
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
                return NULL;
            }

            tipo_param = buscar_tipo_parametro_local(no->nome);
            if (tipo_param != NULL) {
                return tipo_param;
            }

            simbolo = tabela_buscar(tabela, no->nome);
            if (simbolo == NULL) {
                fprintf(stderr,
                        "Erro semantico: variavel '%s' usada sem ter sido declarada\n",
                        no->nome ? no->nome : "?");
                erros_semanticos++;
                return NULL;
            }
            if (simbolo->categoria != SIMBOLO_VARIAVEL) {
                fprintf(stderr,
                        "Erro semantico: '%s' e %s, nao uma variavel\n",
                        no->nome,
                        simbolo->categoria == SIMBOLO_METODO ? "um metodo" : "uma classe");
                erros_semanticos++;
                return NULL;
            }
            return simbolo->tipo;
        }

        case NO_UNARIO: {
            const char *tipo_op = inferir_tipo_expressao(no->a, tabela);
            if (tipo_op == NULL) return NULL;

            if (strcmp(no->nome, "!") == 0) {
                if (strcmp(tipo_op, "boolean") != 0) {
                    fprintf(stderr,
                            "Erro semantico: operador '!' exige 'boolean', recebido '%s'\n",
                            tipo_op);
                    erros_semanticos++;
                    return NULL;
                }
                return "boolean";
            }

            if (!eh_tipo_numerico(tipo_op)) {
                fprintf(stderr,
                        "Erro semantico: operador unario '%s' exige tipo numerico, recebido '%s'\n",
                        no->nome, tipo_op);
                erros_semanticos++;
                return NULL;
            }
            return tipo_op;
        }

        case NO_PREFIXO:
        case NO_POSFIXO: {
            const char *tipo_op = inferir_tipo_expressao(no->a, tabela);
            if (tipo_op == NULL) return NULL;

            if (!eh_tipo_numerico(tipo_op)) {
                fprintf(stderr,
                        "Erro semantico: operador '%s' exige variavel numerica, recebido '%s'\n",
                        no->nome, tipo_op);
                erros_semanticos++;
                return NULL;
            }
            return tipo_op;
        }

        case NO_BINARIO: {
            const char *esq = inferir_tipo_expressao(no->a, tabela);
            const char *dir = inferir_tipo_expressao(no->b, tabela);
            const char *op = no->nome;

            if (esq == NULL || dir == NULL) return NULL;

            if (strcmp(op, "&&") == 0 || strcmp(op, "||") == 0) {
                if (strcmp(esq, "boolean") != 0 || strcmp(dir, "boolean") != 0) {
                    fprintf(stderr,
                            "Erro semantico: operador logico '%s' exige operandos 'boolean' (recebido '%s' e '%s')\n",
                            op, esq, dir);
                    erros_semanticos++;
                    return NULL;
                }
                return "boolean";
            }

            if (strcmp(op, ">") == 0 || strcmp(op, "<") == 0 ||
                strcmp(op, ">=") == 0 || strcmp(op, "<=") == 0) {
                if (!eh_tipo_numerico(esq) || !eh_tipo_numerico(dir)) {
                    fprintf(stderr,
                            "Erro semantico: operador relacional '%s' exige tipos numericos (recebido '%s' e '%s')\n",
                            op, esq, dir);
                    erros_semanticos++;
                    return NULL;
                }
                return "boolean";
            }

            if (strcmp(op, "==") == 0 || strcmp(op, "!=") == 0) {
                int iguais = (strcmp(esq, dir) == 0);
                int ambos_numericos = (eh_tipo_numerico(esq) && eh_tipo_numerico(dir));
                int string_com_null = (strcmp(esq, "String") == 0 && strcmp(dir, "null") == 0) ||
                                      (strcmp(esq, "null") == 0 && strcmp(dir, "String") == 0);

                if (!iguais && !ambos_numericos && !string_com_null) {
                    fprintf(stderr,
                            "Erro semantico: comparacao '%s' invalida entre '%s' e '%s'\n",
                            op, esq, dir);
                    erros_semanticos++;
                    return NULL;
                }
                return "boolean";
            }

            if (strcmp(op, "+") == 0 &&
               (strcmp(esq, "String") == 0 || strcmp(dir, "String") == 0)) {
                return "String";
            }

            if (strcmp(op, "+") == 0 || strcmp(op, "-") == 0 ||
                strcmp(op, "*") == 0 || strcmp(op, "/") == 0 ||
                strcmp(op, "%") == 0) {
                if (!eh_tipo_numerico(esq) || !eh_tipo_numerico(dir)) {
                    fprintf(stderr,
                            "Erro semantico: operador aritmetico '%s' incompativel com '%s' e '%s'\n",
                            op, esq, dir);
                    erros_semanticos++;
                    return NULL;
                }
                return promover_tipo_numerico(esq, dir);
            }

            return NULL;
        }

        default:
            fprintf(stderr, "Erro semantico: no invalido encontrado dentro de uma expressao\n");
            erros_semanticos++;
            return NULL;
    }
}

/* Travessia recursiva da AST (filhos em a/b, irmaos em prox) */
static void percorrer(const NoAst *no, const TabelaSimbolos *tabela)
{
    for (; no != NULL; no = no->prox) {
        switch (no->tipo_no) {
            case NO_PROGRAMA:
                percorrer(no->a, tabela);
                break;

            case NO_METODO:
                limpar_visiveis();
                metodo_atual = no;
                percorrer(no->a, tabela);   /* parametros */
                percorrer(no->b, tabela);   /* corpo */
                metodo_atual = NULL;
                limpar_visiveis();
                break;

            case NO_PARAMETRO:
                marcar_visivel(no->nome);
                break;

            case NO_DECLARACAO:
                if (no->a != NULL) {
                    const char *tipo_init = inferir_tipo_expressao(no->a, tabela);
                    if (tipo_init != NULL && !tipos_compativeis_atribuicao(no->tipo, tipo_init)) {
                        fprintf(stderr,
                                "Erro semantico: nao e possivel inicializar '%s' (%s) com tipo '%s'\n",
                                no->nome, no->tipo, tipo_init);
                        erros_semanticos++;
                    }
                }
                marcar_visivel(no->nome);
                break;

            case NO_ATRIBUICAO: {
                const char *tipo_var = inferir_tipo_expressao(no->a, tabela);
                const char *tipo_val = inferir_tipo_expressao(no->b, tabela);

                if (tipo_var != NULL && tipo_val != NULL) {
                    if (strcmp(no->nome, "=") != 0 && (!eh_tipo_numerico(tipo_var) || !eh_tipo_numerico(tipo_val))) {
                        fprintf(stderr,
                                "Erro semantico: operador composto '%s' exige tipos numericos\n",
                                no->nome);
                        erros_semanticos++;
                    } else if (!tipos_compativeis_atribuicao(tipo_var, tipo_val)) {
                        fprintf(stderr,
                                "Erro semantico: atribuicao incompativel de '%s' para '%s'\n",
                                tipo_val, tipo_var);
                        erros_semanticos++;
                    }
                }
                break;
            }

            case NO_WHILE: {
                const char *tipo_cond = inferir_tipo_expressao(no->a, tabela);
                if (tipo_cond != NULL && strcmp(tipo_cond, "boolean") != 0) {
                    fprintf(stderr,
                            "Erro semantico: condicao do 'while' deve ser 'boolean', recebido '%s'\n",
                            tipo_cond);
                    erros_semanticos++;
                }
                percorrer(no->b, tabela);
                break;
            }

            case NO_PRINTLN:
            case NO_PRINT:
                if (no->a != NULL) {
                    inferir_tipo_expressao(no->a, tabela);
                }
                break;

            case NO_PRINTF: {
                const NoAst *arg;
                for (arg = no->b; arg != NULL; arg = arg->prox) {
                    inferir_tipo_expressao(arg, tabela);
                }
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
    if (raiz == NULL || tabela == NULL) {
        fprintf(stderr, "Erro semantico: contexto de analise invalido\n");
        erros_semanticos++;
        return erros_semanticos;
    }

    percorrer(raiz, tabela);
    return erros_semanticos;
}