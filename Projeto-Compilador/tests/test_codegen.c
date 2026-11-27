#include <string.h>
#include "codegen.h"
#include "teste_util.h"


static NoAst *id(const char *nome) { return ast_criar_identificador(nome); }
static NoAst *num(int v) { return ast_criar_int(v); }
static NoAst *bin(const char *op, NoAst *a, NoAst *b) { return ast_criar_no(NO_BINARIO, op, NULL, a, b); }
static NoAst *un(const char *op, NoAst *a) { return ast_criar_no(NO_UNARIO, op, NULL, a, NULL); }
static NoAst *pre(const char *op, const char *nome) { return ast_criar_no(NO_PREFIXO, op, NULL, id(nome), NULL); }
static NoAst *decl(const char *tipo, const char *nome, NoAst *init) { return ast_criar_no(NO_DECLARACAO, nome, tipo, init, NULL); }
static NoAst *println_de(NoAst *e) { return ast_criar_no(NO_PRINTLN, NULL, NULL, e, NULL); }
static NoAst *texto(const char *literal) { return ast_criar_no(NO_STRING, literal, NULL, NULL, NULL); }

static const char *corpo_gerado(NoAst *comandos)
{
    static char corpo[4096];
    NoAst *metodo = ast_criar_no(NO_METODO, "main", "void", NULL, comandos);
    NoAst *programa = ast_criar_no(NO_PROGRAMA, "T", NULL, metodo, NULL);
    FILE *saida = tmpfile();
    const char *completo;
    const char *inicio;
    const char *fim;
    size_t tamanho;

    gerar_codigo_csharp(programa, saida);
    completo = ler_e_fechar(saida);
    inicio = strstr(completo, "    {\n");
    fim = strstr(completo, "    }\n}\n");
    corpo[0] = '\0';
    if (inicio != NULL && fim != NULL && fim >= inicio + 6) {
        inicio += 6;
        tamanho = (size_t)(fim - inicio);
        memcpy(corpo, inicio, tamanho);
        corpo[tamanho] = '\0';
    }
    ast_liberar(programa);
    return corpo;
}


static const char *texto_impresso(NoAst *expressao)
{
    static char resultado[1024];
    const char *corpo = corpo_gerado(println_de(expressao));
    const char *abre = strchr(corpo, '(');
    const char *fecha = strrchr(corpo, ')');

    resultado[0] = '\0';
    if (abre != NULL && fecha != NULL && fecha > abre) {
        memcpy(resultado, abre + 1, (size_t)(fecha - abre - 1));
        resultado[fecha - abre - 1] = '\0';
    }
    return resultado;
}

static const char *expressao_gerada(NoAst *expressao)
{
    static char resultado[1024];
    const char *prefixo = "        int r = ";
    const char *corpo = corpo_gerado(decl("int", "r", expressao));
    const char *fim = strrchr(corpo, ';');

    resultado[0] = '\0';
    if (strncmp(corpo, prefixo, strlen(prefixo)) == 0 && fim != NULL) {
        memcpy(resultado, corpo + strlen(prefixo), (size_t)(fim - corpo - (long)strlen(prefixo)));
        resultado[fim - corpo - (long)strlen(prefixo)] = '\0';
    }
    return resultado;
}

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

static int test_metodo_nao_void_com_corpo_termina_com_throw(void)
{
    NoAst *corpo = ast_criar_no(NO_DECLARACAO, "r", "int", ast_criar_int(1), NULL);
    NoAst *metodo = ast_criar_no(NO_METODO, "calcular", "int", NULL, corpo);
    NoAst *programa = ast_criar_no(NO_PROGRAMA, "Exemplo", NULL, metodo, NULL);
    FILE *saida = tmpfile();
    const char *esperado =
        "using System;\n\nclass Exemplo\n{\n"
        "    public int calcular()\n    {\n"
        "        int r = 1;\n"
        "        throw new NotImplementedException();\n"
        "    }\n}\n";

    ESPERAR(saida != NULL);
    gerar_codigo_csharp(programa, saida);
    ESPERAR(strcmp(ler_e_fechar(saida), esperado) == 0);

    ast_liberar(programa);
    return 1;
}

static int test_printf_converte_formato_java_para_csharp(void)
{
    NoAst *args = ast_anexar(ast_criar_identificador("x"), ast_criar_identificador("y"));
    NoAst *printf_no = ast_criar_no(NO_PRINTF, NULL, NULL,
        ast_criar_no(NO_STRING, "\"a=%d {b}=%.2f 5%%%n\"", NULL, NULL, NULL), args);
    NoAst *sem_args = ast_criar_no(NO_PRINTF, NULL, NULL,
        ast_criar_no(NO_STRING, "\"{ok} 100%%\"", NULL, NULL, NULL), NULL);
    NoAst *metodo = ast_criar_no(NO_METODO, "main", "void", NULL, ast_anexar(printf_no, sem_args));
    NoAst *programa = ast_criar_no(NO_PROGRAMA, "Exemplo", NULL, metodo, NULL);
    FILE *saida = tmpfile();
    const char *esperado =
        "using System;\n\nclass Exemplo\n{\n"
        "    public static void Main(string[] args)\n    {\n"
        "        Console.Write(\"a={0} {{b}}={1:F2} 5%\\n\", x, y);\n"
        "        Console.Write(\"{ok} 100%\");\n"
        "    }\n}\n";

    ESPERAR(saida != NULL);
    gerar_codigo_csharp(programa, saida);
    ESPERAR(strcmp(ler_e_fechar(saida), esperado) == 0);

    ast_liberar(programa);
    return 1;
}

static int test_char_com_escape_e_emitido_como_escape_csharp(void)
{
    NoAst *decls = ast_anexar(
        ast_criar_no(NO_DECLARACAO, "a", "char", ast_criar_char('\n'), NULL),
        ast_criar_no(NO_DECLARACAO, "b", "char", ast_criar_char('\''), NULL));
    NoAst *metodo = ast_criar_no(NO_METODO, "main", "void", NULL, decls);
    NoAst *programa = ast_criar_no(NO_PROGRAMA, "Exemplo", NULL, metodo, NULL);
    FILE *saida = tmpfile();
    const char *esperado =
        "using System;\n\nclass Exemplo\n{\n"
        "    public static void Main(string[] args)\n    {\n"
        "        char a = '\\n';\n"
        "        char b = '\\'';\n"
        "    }\n}\n";

    ESPERAR(saida != NULL);
    gerar_codigo_csharp(programa, saida);
    ESPERAR(strcmp(ler_e_fechar(saida), esperado) == 0);

    ast_liberar(programa);
    return 1;
}

static int test_variavel_de_while_redeclarada_depois_e_renomeada(void)
{
    NoAst *cond = ast_criar_no(NO_BINARIO, ">", NULL, ast_criar_identificador("n"), ast_criar_int(0));
    NoAst *interna = ast_criar_no(NO_DECLARACAO, "i", "int", ast_criar_identificador("n"), NULL);
    NoAst *uso = ast_criar_no(NO_ATRIBUICAO, "=", NULL,
        ast_criar_identificador("n"), ast_criar_identificador("i"));
    NoAst *laco = ast_criar_no(NO_WHILE, NULL, NULL, cond, ast_anexar(interna, uso));
    NoAst *corpo = ast_anexar(ast_anexar(
        ast_criar_no(NO_DECLARACAO, "n", "int", ast_criar_int(3), NULL), laco),
        ast_criar_no(NO_DECLARACAO, "i", "int", ast_criar_int(5), NULL));
    NoAst *metodo = ast_criar_no(NO_METODO, "main", "void", NULL, corpo);
    NoAst *programa = ast_criar_no(NO_PROGRAMA, "Exemplo", NULL, metodo, NULL);
    FILE *saida = tmpfile();
    const char *esperado =
        "using System;\n\nclass Exemplo\n{\n"
        "    public static void Main(string[] args)\n    {\n"
        "        int n = 3;\n"
        "        while (n > 0)\n"
        "        {\n"
        "            int i_1 = n;\n"
        "            n = i_1;\n"
        "        }\n"
        "        int i = 5;\n"
        "    }\n}\n";

    ESPERAR(saida != NULL);
    gerar_codigo_csharp(programa, saida);
    ESPERAR(strcmp(ler_e_fechar(saida), esperado) == 0);

    ast_liberar(programa);
    return 1;
}


static int test_mapear_todos_os_tipos(void)
{
    ESPERAR(strcmp(mapear_tipo_csharp("int"), "int") == 0);
    ESPERAR(strcmp(mapear_tipo_csharp("long"), "long") == 0);
    ESPERAR(strcmp(mapear_tipo_csharp("short"), "short") == 0);
    ESPERAR(strcmp(mapear_tipo_csharp("float"), "float") == 0);
    ESPERAR(strcmp(mapear_tipo_csharp("double"), "double") == 0);
    ESPERAR(strcmp(mapear_tipo_csharp("char"), "char") == 0);
    ESPERAR(strcmp(mapear_tipo_csharp("void"), "void") == 0);
    ESPERAR(strcmp(mapear_tipo_csharp("boolean"), "bool") == 0);
    ESPERAR(strcmp(mapear_tipo_csharp("String"), "string") == 0);
    ESPERAR(strcmp(mapear_tipo_csharp("byte"), "sbyte") == 0);
    return 1;
}


static int test_parenteses_so_quando_necessarios(void)
{
    ESPERAR(strcmp(expressao_gerada(bin("*", bin("+", id("a"), id("b")), id("c"))), "(a + b) * c") == 0);
    ESPERAR(strcmp(expressao_gerada(bin("+", id("a"), bin("*", id("b"), id("c")))), "a + b * c") == 0);
    ESPERAR(strcmp(expressao_gerada(bin("-", bin("-", id("a"), id("b")), id("c"))), "a - b - c") == 0);
    ESPERAR(strcmp(expressao_gerada(bin("-", id("a"), bin("-", id("b"), id("c")))), "a - (b - c)") == 0);
    ESPERAR(strcmp(expressao_gerada(bin("/", id("a"), bin("*", id("b"), id("c")))), "a / (b * c)") == 0);
    ESPERAR(strcmp(expressao_gerada(bin("%", id("a"), bin("+", id("b"), id("c")))), "a % (b + c)") == 0);
    return 1;
}

static int test_parenteses_preservados_na_concatenacao_de_string(void)
{
    ESPERAR(strcmp(expressao_gerada(bin("+", texto("\"x\""), bin("+", num(1), num(2)))),
                   "\"x\" + (1 + 2)") == 0);
    ESPERAR(strcmp(expressao_gerada(bin("+", bin("+", texto("\"x\""), num(1)), num(2))),
                   "\"x\" + 1 + 2") == 0);
    ESPERAR(strcmp(expressao_gerada(bin("+", texto("\"x\""), bin("-", id("a"), id("b")))),
                   "\"x\" + (a - b)") == 0);
    return 1;
}

static int test_parenteses_em_operadores_logicos(void)
{
    ESPERAR(strcmp(expressao_gerada(bin("&&", bin("||", id("a"), id("b")), id("c"))), "(a || b) && c") == 0);
    ESPERAR(strcmp(expressao_gerada(bin("||", bin("&&", id("a"), id("b")), id("c"))), "a && b || c") == 0);
    ESPERAR(strcmp(expressao_gerada(bin("&&", id("a"), bin("||", id("b"), id("c")))), "a && (b || c)") == 0);
    ESPERAR(strcmp(expressao_gerada(un("!", bin(">", id("a"), id("b")))), "!(a > b)") == 0);
    ESPERAR(strcmp(texto_impresso(un("!", bin(">", id("a"), id("b")))), "(!(a > b)).ToString().ToLower()") == 0);
    return 1;
}

static int test_unario_sobre_binario_leva_parenteses(void)
{
    ESPERAR(strcmp(expressao_gerada(un("-", bin("+", id("a"), id("b")))), "-(a + b)") == 0);
    ESPERAR(strcmp(expressao_gerada(bin("*", un("-", id("a")), id("b"))), "-a * b") == 0);
    ESPERAR(strcmp(expressao_gerada(bin("-", id("a"), un("-", id("b")))), "a - -b") == 0);
    return 1;
}

static int test_sinal_duplicado_nao_vira_decremento(void)
{
    ESPERAR(strcmp(expressao_gerada(un("-", un("-", id("a")))), "-(-a)") == 0);
    ESPERAR(strcmp(expressao_gerada(un("+", un("+", id("a")))), "+(+a)") == 0);
    ESPERAR(strcmp(expressao_gerada(un("-", pre("--", "a"))), "-(--a)") == 0);
    ESPERAR(strcmp(expressao_gerada(un("+", pre("++", "a"))), "+(++a)") == 0);
    /* sinais diferentes nao colidem */
    ESPERAR(strcmp(expressao_gerada(un("-", un("+", id("a")))), "-+a") == 0);
    ESPERAR(strcmp(expressao_gerada(un("-", pre("++", "a"))), "-++a") == 0);
    return 1;
}


static int test_double_mantem_precisao_e_ponto_decimal(void)
{
    ESPERAR(strcmp(expressao_gerada(ast_criar_double(3.14159265358979)), "3.14159265358979") == 0);
    ESPERAR(strcmp(expressao_gerada(ast_criar_double(123456789.123)), "123456789.123") == 0);
    ESPERAR(strcmp(expressao_gerada(ast_criar_double(0.1)), "0.1") == 0);
    ESPERAR(strcmp(expressao_gerada(ast_criar_double(1.0)), "1.0") == 0);
    ESPERAR(strcmp(expressao_gerada(ast_criar_double(100.5)), "100.5") == 0);
    ESPERAR(strcmp(expressao_gerada(ast_criar_double(0.000001234)), "1.234e-06") == 0);
    return 1;
}

static int test_literais_simples(void)
{
    ESPERAR(strcmp(expressao_gerada(num(42)), "42") == 0);
    ESPERAR(strcmp(expressao_gerada(num(0)), "0") == 0);
    ESPERAR(strcmp(expressao_gerada(texto("\"a\\tb\"")), "\"a\\tb\"") == 0);
    ESPERAR(strcmp(expressao_gerada(ast_criar_char('x')), "'x'") == 0);
    ESPERAR(strcmp(expressao_gerada(ast_criar_char('\\')), "'\\\\'") == 0);
    ESPERAR(strcmp(expressao_gerada(ast_criar_char('\t')), "'\\t'") == 0);
    ESPERAR(strcmp(expressao_gerada(ast_criar_char('\0')), "'\\0'") == 0);
    return 1;
}


static int test_boolean_impresso_em_minusculas(void)
{
    ESPERAR(strcmp(texto_impresso(ast_criar_boolean(1)), "true.ToString().ToLower()") == 0);
    ESPERAR(strcmp(texto_impresso(ast_criar_boolean(0)), "false.ToString().ToLower()") == 0);
    ESPERAR(strcmp(texto_impresso(bin(">", id("a"), id("b"))), "(a > b).ToString().ToLower()") == 0);
    ESPERAR(strcmp(texto_impresso(un("!", id("a"))), "(!a).ToString().ToLower()") == 0);
    /* inteiros e strings nao sao convertidos */
    ESPERAR(strcmp(texto_impresso(bin("+", id("a"), id("b"))), "a + b") == 0);
    ESPERAR(strcmp(texto_impresso(texto("\"true\"")), "\"true\"") == 0);
    return 1;
}

static int test_variavel_boolean_usa_o_tipo_declarado(void)
{
    NoAst *cmds = ast_anexar(ast_anexar(
        decl("boolean", "ok", ast_criar_boolean(1)),
        decl("int", "n", num(1))),
        ast_anexar(println_de(id("ok")), println_de(id("n"))));
    const char *esperado =
        "        bool ok = true;\n"
        "        int n = 1;\n"
        "        Console.WriteLine(ok.ToString().ToLower());\n"
        "        Console.WriteLine(n);\n";

    ESPERAR(strcmp(corpo_gerado(cmds), esperado) == 0);
    return 1;
}

static int test_boolean_na_concatenacao_vira_texto_minusculo(void)
{
    ESPERAR(strcmp(texto_impresso(bin("+", texto("\"v=\""), ast_criar_boolean(1))),
                   "\"v=\" + true.ToString().ToLower()") == 0);
    ESPERAR(strcmp(texto_impresso(bin("+", texto("\"c=\""), bin("==", id("a"), num(1)))),
                   "\"c=\" + (a == 1).ToString().ToLower()") == 0);
    return 1;
}


static int test_identificadores_reservados_do_csharp_recebem_arroba(void)
{
    const char *esperado =
        "        int @object = 1;\n"
        "        string @string = \"a\";\n"
        "        bool @bool = true;\n"
        "        int normal = @object;\n";
    NoAst *cmds = ast_anexar(ast_anexar(ast_anexar(
        decl("int", "object", num(1)),
        decl("String", "string", texto("\"a\""))),
        decl("boolean", "bool", ast_criar_boolean(1))),
        decl("int", "normal", id("object")));

    ESPERAR(strcmp(corpo_gerado(cmds), esperado) == 0);
    return 1;
}

static int test_cifrao_em_identificador_e_substituido(void)
{
    NoAst *cmds = ast_anexar(decl("int", "$x", num(1)), decl("int", "a$b", id("$x")));
    const char *esperado =
        "        int _S_x = 1;\n"
        "        int a_S_b = _S_x;\n";

    ESPERAR(strcmp(corpo_gerado(cmds), esperado) == 0);
    return 1;
}

static int test_nome_de_classe_e_metodo_reservados(void)
{
    NoAst *metodo = ast_criar_no(NO_METODO, "event", "void", NULL, NULL);
    NoAst *programa = ast_criar_no(NO_PROGRAMA, "lock", NULL, metodo, NULL);
    FILE *saida = tmpfile();
    const char *esperado =
        "using System;\n\nclass @lock\n{\n"
        "    public void @event()\n    {\n    }\n}\n";

    ESPERAR(saida != NULL);
    gerar_codigo_csharp(programa, saida);
    ESPERAR(strcmp(ler_e_fechar(saida), esperado) == 0);
    ast_liberar(programa);
    return 1;
}


static int test_parametros_array_mantem_colchetes(void)
{
    NoAst *p1 = ast_criar_no(NO_PARAMETRO, "v", "int", NULL, NULL);
    NoAst *p2 = ast_criar_no(NO_PARAMETRO, "n", "int", NULL, NULL);
    NoAst *p3 = ast_criar_no(NO_PARAMETRO, "nomes", "String", NULL, NULL);
    NoAst *metodo;
    NoAst *programa;
    FILE *saida = tmpfile();
    const char *esperado =
        "using System;\n\nclass T\n{\n"
        "    public void f(int[] v, int n, string[] nomes)\n    {\n    }\n}\n";

    p1->valor.ival = 1;
    p3->valor.ival = 1;
    metodo = ast_criar_no(NO_METODO, "f", "void", ast_anexar(ast_anexar(p1, p2), p3), NULL);
    programa = ast_criar_no(NO_PROGRAMA, "T", NULL, metodo, NULL);

    ESPERAR(saida != NULL);
    gerar_codigo_csharp(programa, saida);
    ESPERAR(strcmp(ler_e_fechar(saida), esperado) == 0);
    ast_liberar(programa);
    return 1;
}

static int test_byte_vira_sbyte_em_parametros(void)
{
    NoAst *p = ast_criar_no(NO_PARAMETRO, "b", "byte", NULL, NULL);
    NoAst *metodo = ast_criar_no(NO_METODO, "f", "void", p, NULL);
    NoAst *programa = ast_criar_no(NO_PROGRAMA, "T", NULL, metodo, NULL);
    FILE *saida = tmpfile();

    gerar_codigo_csharp(programa, saida);
    ESPERAR(strstr(ler_e_fechar(saida), "public void f(sbyte b)") != NULL);
    ast_liberar(programa);
    return 1;
}

static int test_membros_separados_por_linha_em_branco_na_ordem_original(void)
{
    NoAst *m1 = ast_criar_no(NO_METODO, "a", "void", NULL, NULL);
    NoAst *m2 = ast_criar_no(NO_METODO, "b", "void", NULL, NULL);
    NoAst *m3 = ast_criar_no(NO_METODO, "main", "void", NULL, NULL);
    NoAst *programa = ast_criar_no(NO_PROGRAMA, "T", NULL, ast_anexar(ast_anexar(m1, m2), m3), NULL);
    FILE *saida = tmpfile();
    const char *esperado =
        "using System;\n\nclass T\n{\n"
        "    public void a()\n    {\n    }\n\n"
        "    public void b()\n    {\n    }\n\n"
        "    public static void Main(string[] args)\n    {\n    }\n}\n";

    gerar_codigo_csharp(programa, saida);
    ESPERAR(strcmp(ler_e_fechar(saida), esperado) == 0);
    ast_liberar(programa);
    return 1;
}

static int test_metodo_nao_void_vazio_e_void_com_corpo(void)
{
    NoAst *m1 = ast_criar_no(NO_METODO, "f", "double", NULL, NULL);
    NoAst *m2 = ast_criar_no(NO_METODO, "g", "void", NULL, decl("int", "x", num(1)));
    NoAst *programa = ast_criar_no(NO_PROGRAMA, "T", NULL, ast_anexar(m1, m2), NULL);
    FILE *saida = tmpfile();
    const char *esperado =
        "using System;\n\nclass T\n{\n"
        "    public double f()\n    {\n        throw new NotImplementedException();\n    }\n\n"
        "    public void g()\n    {\n        int x = 1;\n    }\n}\n";

    gerar_codigo_csharp(programa, saida);
    ESPERAR(strcmp(ler_e_fechar(saida), esperado) == 0);
    ast_liberar(programa);
    return 1;
}


static int test_print_println_sem_argumento_e_com_argumento(void)
{
    NoAst *cmds = ast_anexar(ast_anexar(
        ast_criar_no(NO_PRINTLN, NULL, NULL, NULL, NULL),
        ast_criar_no(NO_PRINT, NULL, NULL, texto("\"a\""), NULL)),
        ast_criar_no(NO_PRINTLN, NULL, NULL, texto("\"b\""), NULL));
    const char *esperado =
        "        Console.WriteLine();\n"
        "        Console.Write(\"a\");\n"
        "        Console.WriteLine(\"b\");\n";


    ESPERAR(strcmp(corpo_gerado(cmds), esperado) == 0);
    return 1;
}

static const char *printf_gerado(const char *formato, NoAst *args)
{
    return corpo_gerado(ast_criar_no(NO_PRINTF, NULL, NULL, texto(formato), args));
}

static int test_printf_especificadores_numericos_e_largura(void)
{
    ESPERAR(strcmp(printf_gerado("\"%d\"", id("x")), "        Console.Write(\"{0}\", x);\n") == 0);
    ESPERAR(strcmp(printf_gerado("\"%5d\"", id("x")), "        Console.Write(\"{0,5}\", x);\n") == 0);
    ESPERAR(strcmp(printf_gerado("\"%-5d\"", id("x")), "        Console.Write(\"{0,-5}\", x);\n") == 0);
    ESPERAR(strcmp(printf_gerado("\"%f\"", id("x")), "        Console.Write(\"{0:F6}\", x);\n") == 0);
    ESPERAR(strcmp(printf_gerado("\"%.0f\"", id("x")), "        Console.Write(\"{0:F0}\", x);\n") == 0);
    ESPERAR(strcmp(printf_gerado("\"%8.3f\"", id("x")), "        Console.Write(\"{0,8:F3}\", x);\n") == 0);
    ESPERAR(strcmp(printf_gerado("\"%x\"", id("x")), "        Console.Write(\"{0:x}\", x);\n") == 0);
    ESPERAR(strcmp(printf_gerado("\"%X\"", id("x")), "        Console.Write(\"{0:X}\", x);\n") == 0);
    return 1;
}

static int test_printf_indices_crescem_a_cada_especificador(void)
{
    NoAst *args = ast_anexar(ast_anexar(id("a"), id("b")), id("c"));

    ESPERAR(strcmp(printf_gerado("\"%d %s %c\"", args),
                   "        Console.Write(\"{0} {1} {2}\", a, b, c);\n") == 0);
    return 1;
}

static int test_printf_percentual_e_quebra_de_linha(void)
{
    ESPERAR(strcmp(printf_gerado("\"100%%\"", NULL), "        Console.Write(\"100%\");\n") == 0);
    ESPERAR(strcmp(printf_gerado("\"a%nb\"", NULL), "        Console.Write(\"a\\nb\");\n") == 0);
    ESPERAR(strcmp(printf_gerado("\"%d%%%n%d\"", ast_anexar(id("a"), id("b"))),
                   "        Console.Write(\"{0}%\\n{1}\", a, b);\n") == 0);
    return 1;
}

static int test_printf_chaves_so_sao_duplicadas_com_argumentos(void)
{
    ESPERAR(strcmp(printf_gerado("\"{x}\"", NULL), "        Console.Write(\"{x}\");\n") == 0);
    ESPERAR(strcmp(printf_gerado("\"{x}\"", id("a")), "        Console.Write(\"{{x}}\", a);\n") == 0);
    ESPERAR(strcmp(printf_gerado("\"{%d}\"", id("a")), "        Console.Write(\"{{{0}}}\", a);\n") == 0);
    return 1;
}

static int test_printf_escapes_do_literal_sao_preservados(void)
{
    ESPERAR(strcmp(printf_gerado("\"\\\"%d\\\"\"", id("a")),
                   "        Console.Write(\"\\\"{0}\\\"\", a);\n") == 0);
    ESPERAR(strcmp(printf_gerado("\"a\\\\%d\"", id("a")),
                   "        Console.Write(\"a\\\\{0}\", a);\n") == 0);
    ESPERAR(strcmp(printf_gerado("\"\\t%d\"", id("a")),
                   "        Console.Write(\"\\t{0}\", a);\n") == 0);
    return 1;
}

static int test_printf_especificador_desconhecido_ou_truncado_e_mantido(void)
{
    ESPERAR(strcmp(printf_gerado("\"%q\"", NULL), "        Console.Write(\"%q\");\n") == 0);
    ESPERAR(strcmp(printf_gerado("\"50%\"", NULL), "        Console.Write(\"50%\");\n") == 0);
    return 1;
}

static int test_printf_com_argumento_boolean_imprime_minusculo(void)
{
    ESPERAR(strcmp(printf_gerado("\"%b\"", ast_criar_boolean(1)),
                   "        Console.Write(\"{0}\", true.ToString().ToLower());\n") == 0);
    ESPERAR(strcmp(printf_gerado("\"%b\"", bin("<", id("a"), id("b"))),
                   "        Console.Write(\"{0}\", (a < b).ToString().ToLower());\n") == 0);
    return 1;
}

static NoAst *laco(NoAst *cond, NoAst *corpo)
{
    return ast_criar_no(NO_WHILE, NULL, NULL, cond, corpo);
}

static int test_lacos_irmaos_com_mesmo_nome_nao_sao_renomeados(void)
{
    NoAst *l1 = laco(bin(">", id("n"), num(0)), decl("int", "i", num(1)));
    NoAst *l2 = laco(bin(">", id("n"), num(0)), decl("int", "i", num(2)));
    NoAst *cmds = ast_anexar(ast_anexar(decl("int", "n", num(1)), l1), l2);
    const char *corpo = corpo_gerado(cmds);

    ESPERAR(strstr(corpo, "int i = 1;") != NULL);
    ESPERAR(strstr(corpo, "int i = 2;") != NULL);
    ESPERAR(strstr(corpo, "i_1") == NULL);
    return 1;
}

static int test_declaracao_externa_anterior_nao_gera_renomeacao(void)
{

    NoAst *l = laco(bin(">", id("n"), num(0)), decl("int", "k", num(1)));
    NoAst *cmds = ast_anexar(ast_anexar(decl("int", "n", num(1)), decl("int", "z", num(0))), l);
    const char *corpo = corpo_gerado(cmds);

    ESPERAR(strstr(corpo, "int k = 1;") != NULL);
    ESPERAR(strstr(corpo, "k_") == NULL);
    return 1;
}

static int test_renomeacao_vale_so_dentro_do_bloco(void)
{
    NoAst *interno = ast_anexar(decl("int", "i", num(1)),
        ast_criar_no(NO_ATRIBUICAO, "=", NULL, id("n"), id("i")));
    NoAst *l = laco(bin(">", id("n"), num(0)), interno);
    NoAst *cmds = ast_anexar(ast_anexar(ast_anexar(decl("int", "n", num(1)), l),
        decl("int", "i", num(5))), println_de(id("i")));
    const char *esperado =
        "        int n = 1;\n"
        "        while (n > 0)\n"
        "        {\n"
        "            int i_1 = 1;\n"
        "            n = i_1;\n"
        "        }\n"
        "        int i = 5;\n"
        "        Console.WriteLine(i);\n";

    ESPERAR(strcmp(corpo_gerado(cmds), esperado) == 0);
    return 1;
}

static int test_renomeacao_em_dois_niveis_de_aninhamento(void)
{
    NoAst *fundo = laco(bin(">", id("t"), num(0)), decl("int", "x", num(7)));
    NoAst *meio = ast_anexar(decl("int", "t", num(1)), fundo);
    NoAst *externo = laco(bin(">", id("m"), num(0)), meio);
    NoAst *cmds = ast_anexar(ast_anexar(ast_anexar(decl("int", "m", num(1)), externo),
        decl("int", "x", num(9))), decl("int", "t", num(8)));
    const char *corpo = corpo_gerado(cmds);

    ESPERAR(strstr(corpo, "int t_1 = 1;") != NULL);
    ESPERAR(strstr(corpo, "while (t_1 > 0)") != NULL);
    ESPERAR(strstr(corpo, "int x_2 = 7;") != NULL);
    ESPERAR(strstr(corpo, "        int x = 9;\n") != NULL);
    ESPERAR(strstr(corpo, "        int t = 8;\n") != NULL);
    return 1;
}

static int test_dois_programas_seguidos_reiniciam_o_contador_de_renomeacao(void)
{
    NoAst *l = laco(bin(">", id("n"), num(0)), decl("int", "i", num(1)));
    NoAst *cmds1 = ast_anexar(ast_anexar(decl("int", "n", num(1)), l), decl("int", "i", num(2)));
    NoAst *l2 = laco(bin(">", id("n"), num(0)), decl("int", "i", num(1)));
    NoAst *cmds2 = ast_anexar(ast_anexar(decl("int", "n", num(1)), l2), decl("int", "i", num(2)));

    ESPERAR(strstr(corpo_gerado(cmds1), "int i_1 = 1;") != NULL);
    ESPERAR(strstr(corpo_gerado(cmds2), "int i_1 = 1;") != NULL);
    return 1;
}

static int test_atribuicoes_compostas_e_incremento_como_comando(void)
{
    NoAst *cmds = ast_anexar(ast_anexar(ast_anexar(
        ast_criar_no(NO_ATRIBUICAO, "+=", NULL, id("x"), num(1)),
        ast_criar_no(NO_ATRIBUICAO, "%=", NULL, id("x"), num(2))),
        ast_criar_no(NO_POSFIXO, "++", NULL, id("x"), NULL)),
        pre("--", "x"));
    const char *esperado =
        "        x += 1;\n"
        "        x %= 2;\n"
        "        x++;\n"
        "        --x;\n";

    ESPERAR(strcmp(corpo_gerado(cmds), esperado) == 0);
    return 1;
}

static int test_while_vazio_e_com_varios_comandos_indentados(void)
{
    NoAst *interno = ast_anexar(ast_anexar(
        ast_criar_no(NO_POSFIXO, "++", NULL, id("i"), NULL),
        laco(id("a"), NULL)),
        println_de(id("i")));
    NoAst *cmds = laco(bin("<", id("i"), num(3)), interno);
    const char *esperado =
        "        while (i < 3)\n"
        "        {\n"
        "            i++;\n"
        "            while (a)\n"
        "            {\n"
        "            }\n"
        "            Console.WriteLine(i);\n"
        "        }\n";

    ESPERAR(strcmp(corpo_gerado(cmds), esperado) == 0);
    return 1;
}


static int test_raiz_nula_ou_invalida_nao_escreve_nada(void)
{
    NoAst *invalido = ast_criar_no(NO_INT, NULL, NULL, NULL, NULL);
    FILE *saida = tmpfile();

    ESPERAR(saida != NULL);
    gerar_codigo_csharp(NULL, saida);
    gerar_codigo_csharp(invalido, saida);
    ESPERAR(strcmp(ler_e_fechar(saida), "") == 0);
    ast_liberar(invalido);
    return 1;
}

static int test_saida_nula_nao_derruba_o_gerador(void)
{
    NoAst *programa = ast_criar_no(NO_PROGRAMA, "T", NULL, NULL, NULL);

    gerar_codigo_csharp(programa, NULL);
    ast_liberar(programa);
    return 1;
}

static int test_metodo_sem_tipo_ou_nome_invalida_a_geracao(void)
{
    NoAst *metodo = ast_criar_no(NO_METODO, "f", NULL, NULL, NULL);
    NoAst *programa = ast_criar_no(NO_PROGRAMA, "T", NULL, metodo, NULL);
    FILE *saida = tmpfile();

    gerar_codigo_csharp(programa, saida);
    ESPERAR(strcmp(ler_e_fechar(saida), "") == 0);
    ast_liberar(programa);
    return 1;
}

static int test_geracao_e_deterministica(void)
{
    NoAst *a = ast_criar_no(NO_METODO, "main", "void", NULL,
        ast_anexar(decl("int", "x", num(1)), println_de(id("x"))));
    NoAst *pa = ast_criar_no(NO_PROGRAMA, "T", NULL, a, NULL);
    FILE *f1 = tmpfile();
    FILE *f2 = tmpfile();
    char primeiro[1024];

    gerar_codigo_csharp(pa, f1);
    snprintf(primeiro, sizeof(primeiro), "%s", ler_e_fechar(f1));
    gerar_codigo_csharp(pa, f2);
    ESPERAR(strcmp(primeiro, ler_e_fechar(f2)) == 0);
    ast_liberar(pa);
    return 1;
}

int main(void)
{
    EXECUTAR_TESTE(test_mapear_tipo_java_para_csharp);
    EXECUTAR_TESTE(test_gerar_classe_com_main_compativel_com_csharp);
    EXECUTAR_TESTE(test_gerar_metodo_com_parametros_e_retorno_convertidos);
    EXECUTAR_TESTE(test_metodo_nao_void_com_corpo_termina_com_throw);
    EXECUTAR_TESTE(test_printf_converte_formato_java_para_csharp);
    EXECUTAR_TESTE(test_char_com_escape_e_emitido_como_escape_csharp);
    EXECUTAR_TESTE(test_variavel_de_while_redeclarada_depois_e_renomeada);
    EXECUTAR_TESTE(test_mapear_todos_os_tipos);
    EXECUTAR_TESTE(test_parenteses_so_quando_necessarios);
    EXECUTAR_TESTE(test_parenteses_preservados_na_concatenacao_de_string);
    EXECUTAR_TESTE(test_parenteses_em_operadores_logicos);
    EXECUTAR_TESTE(test_unario_sobre_binario_leva_parenteses);
    EXECUTAR_TESTE(test_sinal_duplicado_nao_vira_decremento);
    EXECUTAR_TESTE(test_double_mantem_precisao_e_ponto_decimal);
    EXECUTAR_TESTE(test_literais_simples);
    EXECUTAR_TESTE(test_boolean_impresso_em_minusculas);
    EXECUTAR_TESTE(test_variavel_boolean_usa_o_tipo_declarado);
    EXECUTAR_TESTE(test_boolean_na_concatenacao_vira_texto_minusculo);
    EXECUTAR_TESTE(test_identificadores_reservados_do_csharp_recebem_arroba);
    EXECUTAR_TESTE(test_cifrao_em_identificador_e_substituido);
    EXECUTAR_TESTE(test_nome_de_classe_e_metodo_reservados);
    EXECUTAR_TESTE(test_parametros_array_mantem_colchetes);
    EXECUTAR_TESTE(test_byte_vira_sbyte_em_parametros);
    EXECUTAR_TESTE(test_membros_separados_por_linha_em_branco_na_ordem_original);
    EXECUTAR_TESTE(test_metodo_nao_void_vazio_e_void_com_corpo);
    EXECUTAR_TESTE(test_print_println_sem_argumento_e_com_argumento);
    EXECUTAR_TESTE(test_printf_especificadores_numericos_e_largura);
    EXECUTAR_TESTE(test_printf_indices_crescem_a_cada_especificador);
    EXECUTAR_TESTE(test_printf_percentual_e_quebra_de_linha);
    EXECUTAR_TESTE(test_printf_chaves_so_sao_duplicadas_com_argumentos);
    EXECUTAR_TESTE(test_printf_escapes_do_literal_sao_preservados);
    EXECUTAR_TESTE(test_printf_especificador_desconhecido_ou_truncado_e_mantido);
    EXECUTAR_TESTE(test_printf_com_argumento_boolean_imprime_minusculo);
    EXECUTAR_TESTE(test_lacos_irmaos_com_mesmo_nome_nao_sao_renomeados);
    EXECUTAR_TESTE(test_declaracao_externa_anterior_nao_gera_renomeacao);
    EXECUTAR_TESTE(test_renomeacao_vale_so_dentro_do_bloco);
    EXECUTAR_TESTE(test_renomeacao_em_dois_niveis_de_aninhamento);
    EXECUTAR_TESTE(test_dois_programas_seguidos_reiniciam_o_contador_de_renomeacao);
    EXECUTAR_TESTE(test_atribuicoes_compostas_e_incremento_como_comando);
    EXECUTAR_TESTE(test_while_vazio_e_com_varios_comandos_indentados);
    EXECUTAR_TESTE(test_raiz_nula_ou_invalida_nao_escreve_nada);
    EXECUTAR_TESTE(test_saida_nula_nao_derruba_o_gerador);
    EXECUTAR_TESTE(test_metodo_sem_tipo_ou_nome_invalida_a_geracao);
    EXECUTAR_TESTE(test_geracao_e_deterministica);
    return resumo_testes();
}
