#include <stdio.h>
#include <string.h>
#include "codegen.h"

const char *mapear_tipo_csharp(const char *tipo_java)
{
    if (tipo_java == NULL) {
        return NULL;
    }
    if (strcmp(tipo_java, "boolean") == 0) {
        return "bool";
    }
    if (strcmp(tipo_java, "String") == 0) {
        return "string";
    }
    return tipo_java;
}

static void emitir_indentacao(int nivel, FILE *saida)
{
    int i;
    for (i = 0; i < nivel * 4; i++) {
        fputc(' ', saida);
    }
}

static int precedencia_operador(const NoAst *no)
{
    if (no == NULL) {
        return 100;
    }
    switch (no->tipo_no) {
        case NO_INT:
        case NO_DOUBLE:
        case NO_STRING:
        case NO_CHAR:
        case NO_BOOLEAN:
        case NO_NULL:
        case NO_IDENTIFICADOR:
            return 10;
        case NO_POSFIXO:
            return 9;
        case NO_UNARIO:
        case NO_PREFIXO:
            return 8;
        case NO_BINARIO:
            if (no->nome == NULL) return 0;
            if (strcmp(no->nome, "*") == 0 || strcmp(no->nome, "/") == 0 || strcmp(no->nome, "%") == 0)
                return 7;
            if (strcmp(no->nome, "+") == 0 || strcmp(no->nome, "-") == 0)
                return 6;
            if (strcmp(no->nome, "<") == 0 || strcmp(no->nome, "<=") == 0 ||
                strcmp(no->nome, ">") == 0 || strcmp(no->nome, ">=") == 0)
                return 5;
            if (strcmp(no->nome, "==") == 0 || strcmp(no->nome, "!=") == 0)
                return 4;
            if (strcmp(no->nome, "&&") == 0)
                return 3;
            if (strcmp(no->nome, "||") == 0)
                return 2;
            return 1;
        default:
            return 0;
    }
}

static void emitir_expressao(const NoAst *no, FILE *saida);

static void emitir_subexpressao(const NoAst *filho, int prec_pai, int associativo_dir, FILE *saida)
{
    int prec_filho = precedencia_operador(filho);
    int precisa_parenteses = (prec_filho < prec_pai) || (associativo_dir && prec_filho == prec_pai);

    if (precisa_parenteses) {
        fputc('(', saida);
    }
    emitir_expressao(filho, saida);
    if (precisa_parenteses) {
        fputc(')', saida);
    }
}

static void emitir_expressao(const NoAst *no, FILE *saida)
{
    int prec;

    if (no == NULL) {
        return;
    }

    prec = precedencia_operador(no);

    switch (no->tipo_no) {
        case NO_INT:
            fprintf(saida, "%d", no->valor.ival);
            break;

        case NO_DOUBLE: {
            char buf[64];
            snprintf(buf, sizeof(buf), "%g", no->valor.dval);
            fputs(buf, saida);
            if (strchr(buf, '.') == NULL && strchr(buf, 'e') == NULL && strchr(buf, 'E') == NULL) {
                fputs(".0", saida);
            }
            break;
        }

        case NO_STRING:
            fputs(no->nome ? no->nome : "\"\"", saida);
            break;

        case NO_CHAR:
            fprintf(saida, "'%c'", no->valor.cval);
            break;

        case NO_BOOLEAN:
            fputs(no->valor.ival ? "true" : "false", saida);
            break;

        case NO_NULL:
            fputs("null", saida);
            break;

        case NO_IDENTIFICADOR:
            fputs(no->nome ? no->nome : "", saida);
            break;

        case NO_UNARIO:
            fputs(no->nome ? no->nome : "", saida);
            emitir_subexpressao(no->a, prec, 0, saida);
            break;

        case NO_PREFIXO:
            fputs(no->nome ? no->nome : "", saida);
            emitir_expressao(no->a, saida);
            break;

        case NO_POSFIXO:
            emitir_expressao(no->a, saida);
            fputs(no->nome ? no->nome : "", saida);
            break;

        case NO_BINARIO: {
            int nao_associativo = no->nome != NULL &&
                (strcmp(no->nome, "-") == 0 || strcmp(no->nome, "/") == 0 || strcmp(no->nome, "%") == 0);
            emitir_subexpressao(no->a, prec, 0, saida);
            fprintf(saida, " %s ", no->nome ? no->nome : "");
            emitir_subexpressao(no->b, prec, nao_associativo, saida);
            break;
        }

        default:
            break;
    }
}

static void emitir_comandos(
    const NoAst *no,
    int nivel,
    FILE *saida
);

static void emitir_bloco(
    const NoAst *bloco,
    int nivel,
    FILE *saida
)
{
    emitir_indentacao(nivel, saida);
    fputs("{\n", saida);

    if (bloco != NULL) {
        emitir_comandos(bloco->a, nivel + 1, saida);
    }

    emitir_indentacao(nivel, saida);
    fputs("}\n", saida);
}

static void emitir_comandos(const NoAst *no, int nivel, FILE *saida)
{
    for (; no != NULL; no = no->prox) {
        switch (no->tipo_no) {
            case NO_DECLARACAO:
                emitir_indentacao(nivel, saida);
                fprintf(saida, "%s %s", mapear_tipo_csharp(no->tipo), no->nome);
                if (no->a != NULL) {
                    fputs(" = ", saida);
                    emitir_expressao(no->a, saida);
                }
                fputs(";\n", saida);
                break;

            case NO_ATRIBUICAO:
                emitir_indentacao(nivel, saida);
                emitir_expressao(no->a, saida);
                fprintf(saida, " %s ", no->nome ? no->nome : "=");
                emitir_expressao(no->b, saida);
                fputs(";\n", saida);
                break;

            case NO_PREFIXO:
            case NO_POSFIXO:
                emitir_indentacao(nivel, saida);
                emitir_expressao(no, saida);
                fputs(";\n", saida);
                break;

            case NO_WHILE:
                emitir_indentacao(nivel, saida);
                fputs("while (", saida);
                emitir_expressao(no->a, saida);
                fputs(")\n", saida);
                emitir_indentacao(nivel, saida);
                fputs("{\n", saida);
                emitir_comandos(no->b, nivel + 1, saida);
                emitir_indentacao(nivel, saida);
                fputs("}\n", saida);
                break;

            case NO_IF:
                emitir_indentacao(nivel, saida);
                fputs("if (", saida);
                emitir_expressao(no->a, saida);
                fputs(")\n", saida);
                emitir_bloco(no->b, nivel, saida);
                if (no->c != NULL) {
                    emitir_indentacao(nivel, saida);
                    fputs("else\n", saida);
                    emitir_bloco(no->c, nivel, saida);
                }
                break;

            case NO_PRINTLN:
                emitir_indentacao(nivel, saida);
                fputs("Console.WriteLine(", saida);
                if (no->a != NULL) {
                    emitir_expressao(no->a, saida);
                }
                fputs(");\n", saida);
                break;

            case NO_PRINT:
                emitir_indentacao(nivel, saida);
                fputs("Console.Write(", saida);
                if (no->a != NULL) {
                    emitir_expressao(no->a, saida);
                }
                fputs(");\n", saida);
                break;

            case NO_PRINTF: {
                const NoAst *arg;
                emitir_indentacao(nivel, saida);
                fputs("Console.Write(", saida);
                emitir_expressao(no->a, saida);
                for (arg = no->b; arg != NULL; arg = arg->prox) {
                    fputs(", ", saida);
                    emitir_expressao(arg, saida);
                }
                fputs(");\n", saida);
                break;
            }

            default:
                break;
        }
}
}

static void emitir_parametros(const NoAst *parametro, FILE *saida)
{
    int primeiro = 1;

    for (; parametro != NULL; parametro = parametro->prox) {
        if (!primeiro) {
            fputs(", ", saida);
        }
        fprintf(saida, "%s %s",
                mapear_tipo_csharp(parametro->tipo), parametro->nome);
        primeiro = 0;
    }
}

static void emitir_metodo(const NoAst *metodo, FILE *saida)
{
    int eh_main = metodo->nome != NULL && strcmp(metodo->nome, "main") == 0;

    if (eh_main) {
        fputs("    public static void Main(string[] args)\n", saida);
    } else {
        fprintf(saida, "    public %s %s(",
                mapear_tipo_csharp(metodo->tipo), metodo->nome);
        emitir_parametros(metodo->a, saida);
        fputs(")\n", saida);
    }

    fputs("    {\n", saida);
    emitir_comandos(metodo->b, 2, saida);
    if (!eh_main && metodo->tipo != NULL && strcmp(metodo->tipo, "void") != 0 && metodo->b == NULL) {
        fputs("        throw new NotImplementedException();\n", saida);
    }
    fputs("    }\n", saida);
}

static int ast_valida(const NoAst *raiz)
{
    const NoAst *metodo;

    for (metodo = raiz->a; metodo != NULL; metodo = metodo->prox) {
        const NoAst *parametro;

        if (metodo->tipo_no != NO_METODO || metodo->nome == NULL || metodo->tipo == NULL) {
            return 0;
        }
        if (strcmp(metodo->nome, "main") == 0) {
            continue;
        }
        for (parametro = metodo->a; parametro != NULL; parametro = parametro->prox) {
            if (parametro->tipo_no != NO_PARAMETRO ||
                parametro->nome == NULL || parametro->tipo == NULL) {
                return 0;
            }
        }
    }
    return 1;
}

void gerar_codigo_csharp(const NoAst *raiz, FILE *saida)
{
    const NoAst *membro;

    if (saida == NULL) {
        fprintf(stderr, "Erro: arquivo de saida do codegen invalido\n");
        return;
    }
    if (raiz == NULL || raiz->tipo_no != NO_PROGRAMA || raiz->nome == NULL) {
        fprintf(stderr, "Erro: raiz da AST invalida para geracao de codigo C#\n");
        return;
    }
    if (!ast_valida(raiz)) {
        fprintf(stderr, "Erro: membros invalidos na AST para geracao de codigo C#\n");
        return;
    }

    fputs("using System;\n\n", saida);
    fprintf(saida, "class %s\n{\n", raiz->nome);
    for (membro = raiz->a; membro != NULL; membro = membro->prox) {
        if (membro->tipo_no == NO_METODO) {
            emitir_metodo(membro, saida);
            if (membro->prox != NULL) {
                fputc('\n', saida);
            }
        }
    }
    fputs("}\n", saida);

    if (ferror(saida)) {
        fprintf(stderr, "Erro ao escrever codigo C#\n");
    }
}