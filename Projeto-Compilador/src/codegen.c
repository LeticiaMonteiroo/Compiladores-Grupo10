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
    if (!eh_main && metodo->tipo != NULL && strcmp(metodo->tipo, "void") != 0) {
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
