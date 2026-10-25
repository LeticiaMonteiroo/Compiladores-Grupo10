#include <stdio.h>
#include "parser.tab.h"
#include "ast.h"
#include "tabela_simbolos.h"

int main(void) {
    int resultado;

    tabela_simbolos = tabela_criar();
    resultado = yyparse();

    if (resultado == 0) {
        printf("Analise sintatica concluida com sucesso.\n");
        printf("Arvore sintatica abstrata:\n");
        ast_imprimir(ast_raiz, stdout);
        printf("Tabela de simbolos:\n");
        tabela_imprimir(tabela_simbolos, stdout);
    }

    ast_liberar(ast_raiz);
    tabela_liberar(tabela_simbolos);

    return resultado == 0 ? 0 : 1;
}