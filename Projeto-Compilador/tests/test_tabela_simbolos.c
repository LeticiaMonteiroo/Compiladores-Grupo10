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

static int test_escopos_irmaos_aceitam_o_mesmo_nome(void)
{
    TabelaSimbolos *tabela = tabela_criar();

    tabela_entrar_escopo(tabela, ESCOPO_METODO, "f");
    ESPERAR(tabela_inserir(tabela, "x", "int", SIMBOLO_VARIAVEL) == 1);
    tabela_sair_escopo(tabela);

    tabela_entrar_escopo(tabela, ESCOPO_METODO, "g");
    ESPERAR(tabela_inserir(tabela, "x", "double", SIMBOLO_VARIAVEL) == 1);
    ESPERAR(strcmp(tabela_buscar(tabela, "x")->tipo, "double") == 0);
    tabela_sair_escopo(tabela);

    ESPERAR(tabela->total == 2);
    ESPERAR(tabela->total_escopos == 3);

    tabela_liberar(tabela);
    return 1;
}

static int test_simbolo_some_da_busca_ao_sair_do_escopo(void)
{
    TabelaSimbolos *tabela = tabela_criar();
    const Simbolo *simbolo;

    tabela_entrar_escopo(tabela, ESCOPO_METODO, "f");
    tabela_inserir(tabela, "local", "int", SIMBOLO_VARIAVEL);
    ESPERAR(tabela_buscar(tabela, "local") != NULL);
    tabela_sair_escopo(tabela);

    ESPERAR(tabela_buscar(tabela, "local") == NULL);
    simbolo = tabela_buscar_qualquer(tabela, "local");
    ESPERAR(simbolo != NULL);
    ESPERAR(strcmp(simbolo->escopo->rotulo, "f") == 0);

    tabela_liberar(tabela);
    return 1;
}

static int test_busca_sobe_ate_o_escopo_global(void)
{
    TabelaSimbolos *tabela = tabela_criar();
    const Escopo *bloco;

    tabela_inserir(tabela, "calcular", "int", SIMBOLO_METODO);
    tabela_entrar_escopo(tabela, ESCOPO_METODO, "calcular");
    tabela_inserir(tabela, "a", "int", SIMBOLO_PARAMETRO);
    bloco = tabela_entrar_escopo(tabela, ESCOPO_BLOCO, NULL);

    ESPERAR(tabela_buscar_em(bloco, "a") != NULL);
    ESPERAR(tabela_buscar_em(bloco, "calcular") != NULL);
    ESPERAR(tabela_buscar_em(bloco, "b") == NULL);
    ESPERAR(bloco->nivel == 2);
    ESPERAR(bloco->pai->pai == tabela->global);

    tabela_liberar(tabela);
    return 1;
}

static int test_variavel_nao_pode_sombrear_local_do_mesmo_metodo(void)
{
    TabelaSimbolos *tabela = tabela_criar();

    tabela_entrar_escopo(tabela, ESCOPO_METODO, "f");
    ESPERAR(tabela_inserir(tabela, "x", "int", SIMBOLO_PARAMETRO) == 1);
    ESPERAR(tabela_inserir(tabela, "x", "int", SIMBOLO_VARIAVEL) == 0);
    tabela_entrar_escopo(tabela, ESCOPO_BLOCO, NULL);
    ESPERAR(tabela_buscar_conflito(tabela, "x", SIMBOLO_VARIAVEL) != NULL);
    ESPERAR(tabela_inserir(tabela, "x", "int", SIMBOLO_VARIAVEL) == 0);
    ESPERAR(tabela_inserir(tabela, "y", "int", SIMBOLO_VARIAVEL) == 1);
    ESPERAR(tabela->total == 2);

    tabela_liberar(tabela);
    return 1;
}

static int test_variavel_local_pode_ter_nome_de_metodo(void)
{
    TabelaSimbolos *tabela = tabela_criar();

    tabela_inserir(tabela, "total", "int", SIMBOLO_METODO);
    ESPERAR(tabela_inserir(tabela, "total", "void", SIMBOLO_METODO) == 0);
    tabela_entrar_escopo(tabela, ESCOPO_METODO, "total");
    ESPERAR(tabela_inserir(tabela, "total", "int", SIMBOLO_VARIAVEL) == 1);
    ESPERAR(tabela_buscar(tabela, "total")->categoria == SIMBOLO_VARIAVEL);

    tabela_liberar(tabela);
    return 1;
}

static int test_sair_do_global_nao_faz_nada(void)
{
    TabelaSimbolos *tabela = tabela_criar();

    tabela_sair_escopo(tabela);
    ESPERAR(tabela_escopo_atual(tabela) == tabela->global);
    ESPERAR(tabela_inserir(tabela, "x", "int", SIMBOLO_VARIAVEL) == 1);

    tabela_liberar(tabela);
    return 1;
}

static int test_tabela_imprime_arvore_de_escopos(void)
{
    TabelaSimbolos *tabela = tabela_criar();
    FILE *saida = tmpfile();
    const char *esperado =
        "escopo global\n"
        "  metodo f : void\n"
        "  classe Teste\n"
        "  escopo metodo f\n"
        "    parametro p : int\n"
        "    variavel v : int\n"
        "    escopo bloco\n"
        "      variavel w : int\n";

    ESPERAR(saida != NULL);
    tabela_entrar_escopo(tabela, ESCOPO_METODO, "f");
    tabela_inserir(tabela, "p", "int", SIMBOLO_PARAMETRO);
    tabela_inserir(tabela, "v", "int", SIMBOLO_VARIAVEL);
    tabela_entrar_escopo(tabela, ESCOPO_BLOCO, NULL);
    tabela_inserir(tabela, "w", "int", SIMBOLO_VARIAVEL);
    tabela_sair_escopo(tabela);
    tabela_sair_escopo(tabela);
    tabela_inserir(tabela, "f", "void", SIMBOLO_METODO);
    tabela_inserir(tabela, "Teste", NULL, SIMBOLO_CLASSE);

    tabela_imprimir_escopos(tabela, saida);
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
    EXECUTAR_TESTE(test_escopos_irmaos_aceitam_o_mesmo_nome);
    EXECUTAR_TESTE(test_simbolo_some_da_busca_ao_sair_do_escopo);
    EXECUTAR_TESTE(test_busca_sobe_ate_o_escopo_global);
    EXECUTAR_TESTE(test_variavel_nao_pode_sombrear_local_do_mesmo_metodo);
    EXECUTAR_TESTE(test_variavel_local_pode_ter_nome_de_metodo);
    EXECUTAR_TESTE(test_sair_do_global_nao_faz_nada);
    EXECUTAR_TESTE(test_tabela_imprime_arvore_de_escopos);
    return resumo_testes();
}