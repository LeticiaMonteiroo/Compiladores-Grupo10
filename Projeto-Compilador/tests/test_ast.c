#include <string.h>
#include "ast.h"
#include "teste_util.h"

static int test_ast_cria_literal_e_identificador(void)
{
    char buffer[] = "contador";
    NoAst *inteiro = ast_criar_int(42);
    NoAst *id = ast_criar_identificador(buffer);

    buffer[0] = 'X';

    ESPERAR(inteiro->tipo_no == NO_INT);
    ESPERAR(inteiro->valor.ival == 42);
    ESPERAR(inteiro->nome == NULL);
    ESPERAR(id->tipo_no == NO_IDENTIFICADOR);
    ESPERAR(strcmp(id->nome, "contador") == 0);

    ast_liberar(inteiro);
    ast_liberar(id);
    return 1;
}

static int test_ast_cria_while_com_condicao_e_corpo(void)
{
    NoAst *condicao = ast_criar_no(NO_BINARIO, ">", NULL,
        ast_criar_identificador("x"), ast_criar_int(0));
    NoAst *corpo = ast_anexar(
        ast_criar_no(NO_ATRIBUICAO, "-=", NULL, ast_criar_identificador("x"), ast_criar_int(1)),
        ast_criar_no(NO_POSFIXO, "++", NULL, ast_criar_identificador("i"), NULL));
    NoAst *laco = ast_criar_no(NO_WHILE, NULL, NULL, condicao, corpo);

    ESPERAR(laco->tipo_no == NO_WHILE);
    ESPERAR(laco->a->tipo_no == NO_BINARIO);
    ESPERAR(strcmp(laco->a->nome, ">") == 0);
    ESPERAR(laco->a->a->tipo_no == NO_IDENTIFICADOR);
    ESPERAR(laco->a->b->valor.ival == 0);
    ESPERAR(laco->b->tipo_no == NO_ATRIBUICAO);
    ESPERAR(laco->b->prox->tipo_no == NO_POSFIXO);
    ESPERAR(laco->b->prox->prox == NULL);

    ast_liberar(laco);
    return 1;
}

static int test_ast_anexar_preserva_ordem(void)
{
    NoAst *lista = NULL;

    lista = ast_anexar(lista, ast_criar_int(1));
    lista = ast_anexar(lista, ast_criar_int(2));
    lista = ast_anexar(lista, ast_criar_int(3));

    ESPERAR(lista->valor.ival == 1);
    ESPERAR(lista->prox->valor.ival == 2);
    ESPERAR(lista->prox->prox->valor.ival == 3);
    ESPERAR(lista->prox->prox->prox == NULL);

    ast_liberar(lista);
    return 1;
}

static int test_ast_imprime_representacao_da_arvore(void)
{
    NoAst *declaracao = ast_criar_no(NO_DECLARACAO, "valor", "double", ast_criar_double(10.5), NULL);
    NoAst *metodo = ast_criar_no(NO_METODO, "main", "void", declaracao, NULL);
    NoAst *programa = ast_criar_no(NO_PROGRAMA, "Teste", NULL, metodo, NULL);
    FILE *saida = tmpfile();
    const char *esperado =
        "Programa Teste\n"
        "└── Metodo main : void\n"
        "    └── Declaracao valor : double\n"
        "        └── Double 10.5\n";

    ESPERAR(saida != NULL);
    ast_imprimir(programa, saida);
    ESPERAR(strcmp(ler_e_fechar(saida), esperado) == 0);

    ast_liberar(programa);
    return 1;
}

static int test_ast_cria_printf_com_string_literal(void)
{
    NoAst *formato = ast_criar_no(NO_STRING, "\"Ola\"", NULL, NULL, NULL);
    NoAst *printf_no = ast_criar_no(NO_PRINTF, NULL, NULL, formato, NULL);
    FILE *saida = tmpfile();
    const char *esperado =
        "Printf\n"
        "└── String \"Ola\"\n";

    ESPERAR(printf_no->tipo_no == NO_PRINTF);
    ESPERAR(printf_no->a->tipo_no == NO_STRING);
    ESPERAR(strcmp(printf_no->a->nome, "\"Ola\"") == 0);
    ESPERAR(printf_no->b == NULL);

    ESPERAR(saida != NULL);
    ast_imprimir(printf_no, saida);
    ESPERAR(strcmp(ler_e_fechar(saida), esperado) == 0);

    ast_liberar(printf_no);
    return 1;
}

static int test_ast_cria_printf_com_identificador_e_expressao(void)
{
    NoAst *formato = ast_criar_no(NO_STRING, "\"%d %d\"", NULL, NULL, NULL);
    NoAst *argumentos = ast_anexar(
        ast_criar_identificador("idade"),
        ast_criar_no(NO_BINARIO, "+", NULL, ast_criar_identificador("x"), ast_criar_int(1)));
    NoAst *printf_no = ast_criar_no(NO_PRINTF, NULL, NULL, formato, argumentos);
    FILE *saida = tmpfile();
    const char *esperado =
        "Printf\n"
        "├── String \"%d %d\"\n"
        "├── Identificador idade\n"
        "└── Binario +\n"
        "    ├── Identificador x\n"
        "    └── Int 1\n";

    ESPERAR(printf_no->a->tipo_no == NO_STRING);
    ESPERAR(printf_no->b->tipo_no == NO_IDENTIFICADOR);
    ESPERAR(printf_no->b->prox->tipo_no == NO_BINARIO);
    ESPERAR(printf_no->b->prox->prox == NULL);

    ESPERAR(saida != NULL);
    ast_imprimir(printf_no, saida);
    ESPERAR(strcmp(ler_e_fechar(saida), esperado) == 0);

    ast_liberar(printf_no);
    return 1;
}

static int test_ast_imprime_while_com_condicao_e_corpo(void)
{
    NoAst *condicao = ast_criar_no(NO_BINARIO, ">", NULL,
        ast_criar_identificador("x"), ast_criar_int(0));
    NoAst *corpo = ast_anexar(
        ast_criar_no(NO_ATRIBUICAO, "-=", NULL, ast_criar_identificador("x"), ast_criar_int(1)),
        ast_criar_no(NO_POSFIXO, "++", NULL, ast_criar_identificador("i"), NULL));
    NoAst *laco = ast_criar_no(NO_WHILE, NULL, NULL, condicao, corpo);
    FILE *saida = tmpfile();
    const char *esperado =
        "While\n"
        "├── Condicao\n"
        "│   └── Binario >\n"
        "│       ├── Identificador x\n"
        "│       └── Int 0\n"
        "└── Corpo\n"
        "    ├── Atribuicao -=\n"
        "    │   ├── Identificador x\n"
        "    │   └── Int 1\n"
        "    └── Posfixo ++\n"
        "        └── Identificador i\n";

    ESPERAR(saida != NULL);
    ast_imprimir(laco, saida);
    ESPERAR(strcmp(ler_e_fechar(saida), esperado) == 0);

    ast_liberar(laco);
    return 1;
}

int main(void)
{
    EXECUTAR_TESTE(test_ast_cria_literal_e_identificador);
    EXECUTAR_TESTE(test_ast_cria_while_com_condicao_e_corpo);
    EXECUTAR_TESTE(test_ast_anexar_preserva_ordem);
    EXECUTAR_TESTE(test_ast_imprime_representacao_da_arvore);
    EXECUTAR_TESTE(test_ast_cria_printf_com_string_literal);
    EXECUTAR_TESTE(test_ast_cria_printf_com_identificador_e_expressao);
    EXECUTAR_TESTE(test_ast_imprime_while_com_condicao_e_corpo);
    return resumo_testes();
}