#include <stdio.h>
#include "parser.tab.h"
#include "ast.h"
#include "tabela_simbolos.h"
#include "semantico.h"

extern FILE *yyin;

int main(int argc, char **argv) {
    int resultado;
    int erros_semanticos = 0;

    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (yyin == NULL) {
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
            tabela_imprimir(tabela_simbolos, stdout);
        }
    }

    if (argc > 1) {
        fclose(yyin);
    }

    ast_liberar(ast_raiz);
    tabela_liberar(tabela_simbolos);

    return resultado == 0 && erros_semanticos == 0 ? 0 : 1;
}