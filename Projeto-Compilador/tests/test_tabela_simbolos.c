#include <string.h>
#include "tabela_simbolos.h"
#include "teste_util.h"

static int test_tabela_insere_e_busca_simbolo(void)
{
    TabelaSimbolos *tabela = tabela_criar();
    const Simbolo *simbolo;

    ESPERAR(tabela != NULL);
    ESPERAR(tabela_inserir(tabela, "contador", "int", SIMBOLO_VARIAVEL) == 1);

    simbolo = tabela_buscar(tabela, "contador");
    ESPERAR(simbolo != NULL);
    ESPERAR(strcmp(simbolo->nome, "contador") == 0);
    ESPERAR(strcmp(simbolo->tipo, "int") == 0);
    ESPERAR(simbolo->categoria == SIMBOLO_VARIAVEL);
    ESPERAR(tabela->total == 1);

    tabela_liberar(tabela);
    return 1;
}

static int test_tabela_busca_inexistente_retorna_nulo(void)
{
    TabelaSimbolos *tabela = tabela_criar();

    ESPERAR(tabela_buscar(tabela, "x") == NULL);
    tabela_inserir(tabela, "y", "double", SIMBOLO_VARIAVEL);
    ESPERAR(tabela_buscar(tabela, "x") == NULL);
    ESPERAR(tabela_buscar(tabela, "y") != NULL);

    tabela_liberar(tabela);
    return 1;
}

static int test_tabela_rejeita_nome_duplicado(void)
{
    TabelaSimbolos *tabela = tabela_criar();

    ESPERAR(tabela_inserir(tabela, "x", "int", SIMBOLO_VARIAVEL) == 1);
    ESPERAR(tabela_inserir(tabela, "x", "double", SIMBOLO_VARIAVEL) == 0);
    ESPERAR(tabela->total == 1);
    ESPERAR(strcmp(tabela_buscar(tabela, "x")->tipo, "int") == 0);

    tabela_liberar(tabela);
    return 1;
}

static int test_tabela_imprime_na_ordem_de_insercao(void)
{
    TabelaSimbolos *tabela = tabela_criar();
    FILE *saida = tmpfile();
    const char *esperado =
        "variavel i : int\n"
        "metodo main : void\n"
        "classe Teste\n";

    ESPERAR(saida != NULL);
    tabela_inserir(tabela, "i", "int", SIMBOLO_VARIAVEL);
    tabela_inserir(tabela, "main", "void", SIMBOLO_METODO);
    tabela_inserir(tabela, "Teste", NULL, SIMBOLO_CLASSE);

    tabela_imprimir(tabela, saida);
    ESPERAR(strcmp(ler_e_fechar(saida), esperado) == 0);

    tabela_liberar(tabela);
    return 1;
}

int main(void)
{
    EXECUTAR_TESTE(test_tabela_insere_e_busca_simbolo);
    EXECUTAR_TESTE(test_tabela_busca_inexistente_retorna_nulo);
    EXECUTAR_TESTE(test_tabela_rejeita_nome_duplicado);
    EXECUTAR_TESTE(test_tabela_imprime_na_ordem_de_insercao);
    return resumo_testes();
}