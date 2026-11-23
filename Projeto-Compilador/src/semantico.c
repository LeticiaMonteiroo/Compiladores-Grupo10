#include <stdio.h>
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
            const Simbolo *simb = tabela_buscar(tabela, no->nome);
            if (simb == NULL) {
                fprintf(stderr, "Erro semantico: identificador '%s' nao declarado\n",
                        no->nome ? no->nome : "?");
                erros_semanticos++;
                return NULL;
            }
            if (simb->categoria != SIMBOLO_VARIAVEL) {
                fprintf(stderr, "Erro semantico: '%s' nao e uma variavel\n", simb->nome);
                erros_semanticos++;
                return NULL;
            }
            return simb->tipo;
        }

        case NO_UNARIO: {
            const char *tipo_op = inferir_tipo_expressao(no->a, tabela);
            if (tipo_op == NULL) return NULL;

            if (strcmp(no->nome, "!") == 0) {
                if (strcmp(tipo_op, "boolean") != 0) {
                    fprintf(stderr, "Erro semantico: operador '!' exige 'boolean', recebido '%s'\n", tipo_op);
                    erros_semanticos++;
                    return NULL;
                }
                return "boolean";
            }

            if (!eh_tipo_numerico(tipo_op)) {
                fprintf(stderr, "Erro semantico: operador unario '%s' exige tipo numerico, recebido '%s'\n",
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
                fprintf(stderr, "Erro semantico: operador '%s' exige variavel numerica, recebido '%s'\n",
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

static void verificar_no(const NoAst *no, const TabelaSimbolos *tabela)
{
    for (; no != NULL; no = no->prox) {
        switch (no->tipo_no) {
            case NO_PROGRAMA:
                verificar_no(no->a, tabela);
                break;

            case NO_METODO:
                verificar_no(no->b, tabela);
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
                verificar_no(no->b, tabela);
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

    verificar_no(raiz, tabela);
    return erros_semanticos;
}