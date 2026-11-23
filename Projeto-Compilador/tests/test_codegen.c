#include <string.h>
#include "codegen.h"
#include "teste_util.h"

static int test_mapear_tipo_java_para_csharp(void)
{
    ESPERAR(strcmp(mapear_tipo_csharp("boolean"), "bool") == 0);
    ESPERAR(strcmp(mapear_tipo_csharp("String"), "string") == 0);
    ESPERAR(strcmp(mapear_tipo_csharp("int"), "int") == 0);
    ESPERAR(mapear_tipo_csharp(NULL) == NULL);
    return 1;
}

static int test_gerar_classe_com_main_compativel_com_csharp(void)
{
    NoAst *metodo = ast_criar_no(NO_METODO, "main", "void", NULL, NULL);
    NoAst *programa = ast_criar_no(NO_PROGRAMA, "Exemplo", NULL, metodo, NULL);
    FILE *saida = tmpfile();
    const char *esperado =
        "using System;\n"
        "\n"
        "class Exemplo\n"
        "{\n"
        "    public static void Main(string[] args)\n"
        "    {\n"
        "    }\n"
        "}\n";

    ESPERAR(saida != NULL);
    gerar_codigo_csharp(programa, saida);
    ESPERAR(strcmp(ler_e_fechar(saida), esperado) == 0);

    ast_liberar(programa);
    return 1;
}

static int test_gerar_metodo_com_parametros_e_retorno_convertidos(void)
{
    NoAst *parametros = ast_anexar(
        ast_criar_no(NO_PARAMETRO, "texto", "String", NULL, NULL),
        ast_criar_no(NO_PARAMETRO, "ativo", "boolean", NULL, NULL));
    NoAst *metodo = ast_criar_no(NO_METODO, "processar", "int", parametros, NULL);
    NoAst *programa = ast_criar_no(NO_PROGRAMA, "Exemplo", NULL, metodo, NULL);
    FILE *saida = tmpfile();
    const char *esperado =
        "using System;\n"
        "\n"
        "class Exemplo\n"
        "{\n"
        "    public int processar(string texto, bool ativo)\n"
        "    {\n"
        "        throw new NotImplementedException();\n"
        "    }\n"
        "}\n";

    ESPERAR(saida != NULL);
    gerar_codigo_csharp(programa, saida);
    ESPERAR(strcmp(ler_e_fechar(saida), esperado) == 0);

    ast_liberar(programa);
    return 1;
}

int main(void)
{
    EXECUTAR_TESTE(test_mapear_tipo_java_para_csharp);
    EXECUTAR_TESTE(test_gerar_classe_com_main_compativel_com_csharp);
    EXECUTAR_TESTE(test_gerar_metodo_com_parametros_e_retorno_convertidos);
    return resumo_testes();
}
