#include <stdio.h>
#include <string.h>
#include "parser.tab.h"
#include "ast.h"
#include "tabela_simbolos.h"
#include "semantico.h"
#include "codegen.h"

extern FILE *yyin;

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
    fclose(saida);
    printf("Arquivo C# gerado com sucesso: %s\n", destino);
    return 1;
}

int main(int argc, char **argv)
{
    int resultado;
    int erros_semanticos = 0;
    int erro_saida = 0;

    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror(argv[1]);
            return 1;
        }
    }

    tabela_simbolos = tabela_criar();
    resultado = yyparse();

    if (resultado == 0) {
        erros_semanticos = analisar_semantica(ast_raiz, tabela_simbolos);
        if (erros_semanticos == 0) {
            printf("Analise sintatica concluida com sucesso.\n");
            printf("Arvore sintatica abstrata:\n");
            ast_imprimir(ast_raiz, stdout);
            printf("Tabela de simbolos:\n");
            tabela_imprimir_escopos(tabela_simbolos, stdout);

            if (argc > 2 && !salvar_csharp(ast_raiz, argv[2])) {
                erro_saida = 1;
            }
        }
    }

    if (argc > 1) {
        fclose(yyin);
    }

    ast_liberar(ast_raiz);
    tabela_liberar(tabela_simbolos);

    return (resultado == 0 && erros_semanticos == 0 && !erro_saida) ? 0 : 1;
}