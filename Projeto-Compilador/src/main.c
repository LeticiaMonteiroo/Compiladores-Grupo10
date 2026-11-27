#include <stdio.h>
#include <string.h>
#include "parser.tab.h"
#include "ast.h"
#include "tabela_simbolos.h"
#include "semantico.h"
#include "codegen.h"

extern FILE *yyin;
extern int erros_lexicos;

static int salvar_csharp(const NoAst *raiz, const char *caminho_arg)
{
    char nome_auto[256];
    const char *destino = caminho_arg;
    FILE *saida;

    if (strcmp(caminho_arg, "--cs") == 0) {
        snprintf(nome_auto, sizeof(nome_auto), "%s.cs", raiz->nome);
        destino = nome_auto;
    }

    saida = fopen(destino, "w");
    if (!saida) {
        perror(destino);
        return 0;
    }

    gerar_codigo_csharp(raiz, saida);
    if (fclose(saida) != 0) {
        perror(destino);
        return 0;
    }
    printf("Arquivo C# gerado com sucesso: %s\n", destino);
    return 1;
}

int main(int argc, char **argv)
{
    int status = 1;

    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror(argv[1]);
            return 1;
        }
    }

    tabela_simbolos = tabela_criar();

    if (yyparse() != 0 || erros_lexicos > 0) {
        fprintf(stderr, "Compilacao abortada: erros lexicos ou sintaticos.\n");
    } else if (analisar_semantica(ast_raiz, tabela_simbolos) != 0) {
        fprintf(stderr, "Compilacao abortada: erros semanticos.\n");
    } else {
        printf("Analise sintatica concluida com sucesso.\n");
        printf("Arvore sintatica abstrata:\n");
        ast_imprimir(ast_raiz, stdout);
        printf("Tabela de simbolos:\n");
        tabela_imprimir_escopos(tabela_simbolos, stdout);

        if (argc <= 2 || salvar_csharp(ast_raiz, argv[2])) {
            status = 0;
        }
    }

    if (argc > 1) {
        fclose(yyin);
    }

    ast_liberar(ast_raiz);
    tabela_liberar(tabela_simbolos);

    return status;
}