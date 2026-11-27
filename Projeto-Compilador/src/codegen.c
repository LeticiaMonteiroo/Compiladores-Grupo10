#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
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
    if (strcmp(tipo_java, "byte") == 0) {
        return "sbyte";
    }
    return tipo_java;
}

#define MAX_RENOMEACOES 256

typedef struct {
    const char *original;
    char novo[128];
} Renomeacao;

static Renomeacao renomeacoes[MAX_RENOMEACOES];
static int total_renomeacoes = 0;
static int contador_renomeacoes = 0;

typedef struct ContextoBloco {
    const NoAst *restantes;
    const struct ContextoBloco *pai;
} ContextoBloco;

static int declarado_adiante(const ContextoBloco *ctx, const char *nome)
{
    const NoAst *no;

    for (; ctx != NULL; ctx = ctx->pai) {
        for (no = ctx->restantes; no != NULL; no = no->prox) {
            if (no->tipo_no == NO_DECLARACAO && no->nome != NULL &&
                strcmp(no->nome, nome) == 0) {
                return 1;
            }
        }
    }
    return 0;
}

static const char *const RESERVADAS_CSHARP[] = {
    "abstract", "as", "base", "bool", "break", "byte", "case", "catch", "char",
    "checked", "class", "const", "continue", "decimal", "default", "delegate",
    "do", "double", "else", "enum", "event", "explicit", "extern", "false",
    "finally", "fixed", "float", "for", "foreach", "goto", "if", "implicit",
    "in", "int", "interface", "internal", "is", "lock", "long", "namespace",
    "new", "null", "object", "operator", "out", "override", "params", "private",
    "protected", "public", "readonly", "ref", "return", "sbyte", "sealed",
    "short", "sizeof", "stackalloc", "static", "string", "struct", "switch",
    "this", "throw", "true", "try", "typeof", "uint", "ulong", "unchecked",
    "unsafe", "ushort", "using", "virtual", "void", "volatile", "while", NULL
};

static int eh_reservada_csharp(const char *nome)
{
    int i;

    for (i = 0; RESERVADAS_CSHARP[i] != NULL; i++) {
        if (strcmp(RESERVADAS_CSHARP[i], nome) == 0) {
            return 1;
        }
    }
    return 0;
}

/* '$' e valido em identificadores Java, mas nao em C#. */
static void sanitizar_nome(char *destino, size_t tamanho, const char *nome)
{
    size_t usado = 0;

    for (; *nome != '\0' && usado + 4 < tamanho; nome++) {
        if (*nome == '$') {
            memcpy(destino + usado, "_S_", 3);
            usado += 3;
        } else {
            destino[usado++] = *nome;
        }
    }
    destino[usado] = '\0';
}

static void emitir_identificador(const char *nome, FILE *saida)
{
    char seguro[128];
    int i;

    for (i = total_renomeacoes - 1; i >= 0; i--) {
        if (strcmp(renomeacoes[i].original, nome) == 0) {
            fputs(renomeacoes[i].novo, saida);
            return;
        }
    }
    sanitizar_nome(seguro, sizeof(seguro), nome);
    if (eh_reservada_csharp(seguro)) {
        fputc('@', saida);
    }
    fputs(seguro, saida);
}

static void registrar_renomeacao(const char *nome)
{
    char base[96];

    if (total_renomeacoes >= MAX_RENOMEACOES) {
        return;
    }
    sanitizar_nome(base, sizeof(base), nome);
    renomeacoes[total_renomeacoes].original = nome;
    snprintf(renomeacoes[total_renomeacoes].novo,
             sizeof(renomeacoes[total_renomeacoes].novo),
             "%s_%d", base, ++contador_renomeacoes);
    total_renomeacoes++;
}


typedef struct {
    const char *nome;
    const char *tipo;
} TipoVariavel;

static TipoVariavel tipos_variaveis[MAX_RENOMEACOES];
static int total_tipos_variaveis = 0;

static void registrar_tipo_variavel(const char *nome, const char *tipo)
{
    if (total_tipos_variaveis >= MAX_RENOMEACOES) {
        return;
    }
    tipos_variaveis[total_tipos_variaveis].nome = nome;
    tipos_variaveis[total_tipos_variaveis].tipo = tipo;
    total_tipos_variaveis++;
}

static const char *tipo_da_variavel(const char *nome)
{
    int i;

    for (i = total_tipos_variaveis - 1; i >= 0; i--) {
        if (strcmp(tipos_variaveis[i].nome, nome) == 0) {
            return tipos_variaveis[i].tipo;
        }
    }
    return NULL;
}

static int eh_booleano(const NoAst *no)
{
    const char *op;
    const char *tipo;

    if (no == NULL) {
        return 0;
    }
    op = no->nome;
    switch (no->tipo_no) {
        case NO_BOOLEAN:
            return 1;
        case NO_UNARIO:
            return op != NULL && strcmp(op, "!") == 0;
        case NO_BINARIO:
            return op != NULL &&
                (strcmp(op, "&&") == 0 || strcmp(op, "||") == 0 ||
                 strcmp(op, "==") == 0 || strcmp(op, "!=") == 0 ||
                 strcmp(op, "<") == 0 || strcmp(op, "<=") == 0 ||
                 strcmp(op, ">") == 0 || strcmp(op, ">=") == 0);
        case NO_IDENTIFICADOR:
            tipo = no->nome != NULL ? tipo_da_variavel(no->nome) : NULL;
            return tipo != NULL && strcmp(tipo, "boolean") == 0;
        default:
            return 0;
    }
}

static void emitir_char_csharp(char c, FILE *saida)
{
    switch (c) {
        case '\n': fputs("'\\n'", saida);  break;
        case '\t': fputs("'\\t'", saida);  break;
        case '\r': fputs("'\\r'", saida);  break;
        case '\b': fputs("'\\b'", saida);  break;
        case '\f': fputs("'\\f'", saida);  break;
        case '\0': fputs("'\\0'", saida);  break;
        case '\\': fputs("'\\\\'", saida); break;
        case '\'': fputs("'\\''", saida);  break;
        default:   fprintf(saida, "'%c'", c); break;
    }
}

static void emitir_formato_printf(const char *literal, int tem_argumentos, FILE *saida)
{
    size_t n = strlen(literal);
    size_t i;
    int indice = 0;

    if (n < 2) {
        fputs(literal, saida);
        return;
    }

    fputc('"', saida);
    for (i = 1; i + 1 < n; i++) {
        char c = literal[i];
        size_t j;
        int alinhar_esquerda = 0;
        int largura = 0;
        int precisao = -1;
        char conv;

        if (c == '\\' && i + 2 < n) {
            fputc(c, saida);
            fputc(literal[++i], saida);
            continue;
        }
        if (c == '{' || c == '}') {
            fputc(c, saida);
            if (tem_argumentos) {
                fputc(c, saida);
            }
            continue;
        }
        if (c != '%') {
            fputc(c, saida);
            continue;
        }

        j = i + 1;
        if (j + 1 < n && literal[j] == '-') {
            alinhar_esquerda = 1;
            j++;
        }
        while (j + 1 < n && isdigit((unsigned char)literal[j])) {
            largura = largura * 10 + (literal[j] - '0');
            j++;
        }
        if (j + 1 < n && literal[j] == '.') {
            j++;
            precisao = 0;
            while (j + 1 < n && isdigit((unsigned char)literal[j])) {
                precisao = precisao * 10 + (literal[j] - '0');
                j++;
            }
        }
        if (j + 1 >= n) {
            fputc('%', saida);
            continue;
        }

        conv = literal[j];
        if (conv == '%') {
            fputc('%', saida);
            i = j;
        } else if (conv == 'n') {
            fputs("\\n", saida);
            i = j;
        } else if (conv == 'd' || conv == 's' || conv == 'c' || conv == 'b' ||
                   conv == 'f' || conv == 'x' || conv == 'X') {
            fprintf(saida, "{%d", indice++);
            if (largura > 0) {
                fprintf(saida, ",%s%d", alinhar_esquerda ? "-" : "", largura);
            }
            if (conv == 'f') {
                fprintf(saida, ":F%d", precisao >= 0 ? precisao : 6);
            } else if (conv == 'x' || conv == 'X') {
                fprintf(saida, ":%c", conv);
            }
            fputc('}', saida);
            i = j;
        } else {
            fputc('%', saida);
        }
    }
    fputc('"', saida);
}

static void emitir_indentacao(int nivel, FILE *saida)
{
    int i;
    for (i = 0; i < nivel * 4; i++) {
        fputc(' ', saida);
    }
}

static int precedencia_operador(const NoAst *no)
{
    if (no == NULL) {
        return 100;
    }
    switch (no->tipo_no) {
        case NO_INT:
        case NO_DOUBLE:
        case NO_STRING:
        case NO_CHAR:
        case NO_BOOLEAN:
        case NO_NULL:
        case NO_IDENTIFICADOR:
            return 10;
        case NO_POSFIXO:
            return 9;
        case NO_UNARIO:
        case NO_PREFIXO:
            return 8;
        case NO_BINARIO:
            if (no->nome == NULL) return 0;
            if (strcmp(no->nome, "*") == 0 || strcmp(no->nome, "/") == 0 || strcmp(no->nome, "%") == 0)
                return 7;
            if (strcmp(no->nome, "+") == 0 || strcmp(no->nome, "-") == 0)
                return 6;
            if (strcmp(no->nome, "<") == 0 || strcmp(no->nome, "<=") == 0 ||
                strcmp(no->nome, ">") == 0 || strcmp(no->nome, ">=") == 0)
                return 5;
            if (strcmp(no->nome, "==") == 0 || strcmp(no->nome, "!=") == 0)
                return 4;
            if (strcmp(no->nome, "&&") == 0)
                return 3;
            if (strcmp(no->nome, "||") == 0)
                return 2;
            return 1;
        default:
            return 0;
    }
}

static void emitir_expressao(const NoAst *no, FILE *saida);

static void emitir_como_texto(const NoAst *no, FILE *saida)
{
    if (eh_booleano(no)) {
        int parenteses = precedencia_operador(no) < 10;

        if (parenteses) {
            fputc('(', saida);
        }
        emitir_expressao(no, saida);
        if (parenteses) {
            fputc(')', saida);
        }
        fputs(".ToString().ToLower()", saida);
    } else {
        emitir_expressao(no, saida);
    }
}

static void emitir_subexpressao(const NoAst *filho, int prec_pai, int associativo_dir, FILE *saida)
{
    int prec_filho = precedencia_operador(filho);
    int precisa_parenteses = (prec_filho < prec_pai) || (associativo_dir && prec_filho == prec_pai);

    if (precisa_parenteses) {
        fputc('(', saida);
    }
    emitir_expressao(filho, saida);
    if (precisa_parenteses) {
        fputc(')', saida);
    }
}

static void emitir_expressao(const NoAst *no, FILE *saida)
{
    int prec;

    if (no == NULL) {
        return;
    }

    prec = precedencia_operador(no);

    switch (no->tipo_no) {
        case NO_INT:
            fprintf(saida, "%d", no->valor.ival);
            break;

        case NO_DOUBLE: {
            char buf[64];
            int precisao;
            for (precisao = 1; precisao <= 17; precisao++) {
                snprintf(buf, sizeof(buf), "%.*g", precisao, no->valor.dval);
                if (strtod(buf, NULL) == no->valor.dval) {
                    break;
                }
            }
            fputs(buf, saida);
            if (strchr(buf, '.') == NULL && strchr(buf, 'e') == NULL && strchr(buf, 'E') == NULL) {
                fputs(".0", saida);
            }
            break;
        }

        case NO_STRING:
            fputs(no->nome ? no->nome : "\"\"", saida);
            break;

        case NO_CHAR:
            emitir_char_csharp(no->valor.cval, saida);
            break;

        case NO_BOOLEAN:
            fputs(no->valor.ival ? "true" : "false", saida);
            break;

        case NO_NULL:
            fputs("null", saida);
            break;

        case NO_IDENTIFICADOR:
            if (no->nome) {
                emitir_identificador(no->nome, saida);
            }
            break;

        case NO_UNARIO: {
            const NoAst *filho = no->a;
            int sinal_repetido = no->nome != NULL && filho != NULL &&
                (no->nome[0] == '-' || no->nome[0] == '+') &&
                (filho->tipo_no == NO_UNARIO || filho->tipo_no == NO_PREFIXO) &&
                filho->nome != NULL && filho->nome[0] == no->nome[0];

            fputs(no->nome ? no->nome : "", saida);
            if (sinal_repetido) {
                fputc('(', saida);
                emitir_expressao(filho, saida);
                fputc(')', saida);
            } else {
                emitir_subexpressao(filho, prec, 0, saida);
            }
            break;
        }

        case NO_PREFIXO:
            fputs(no->nome ? no->nome : "", saida);
            emitir_expressao(no->a, saida);
            break;

        case NO_POSFIXO:
            emitir_expressao(no->a, saida);
            fputs(no->nome ? no->nome : "", saida);
            break;

        case NO_BINARIO: {
            
            int nao_associativo = 1;
            int concatena = no->nome != NULL && strcmp(no->nome, "+") == 0;

            if (concatena && eh_booleano(no->a)) {
                emitir_como_texto(no->a, saida);
            } else {
                emitir_subexpressao(no->a, prec, 0, saida);
            }
            fprintf(saida, " %s ", no->nome ? no->nome : "");
            if (concatena && eh_booleano(no->b)) {
                emitir_como_texto(no->b, saida);
            } else {
                emitir_subexpressao(no->b, prec, nao_associativo, saida);
            }
            break;
        }

        default:
            break;
    }
}

static void emitir_comandos(
    const NoAst *no,
    int nivel,
    const ContextoBloco *pai,
    FILE *saida
);

static void emitir_bloco(
    const NoAst *bloco,
    int nivel,
    const ContextoBloco *pai,
    FILE *saida
)
{
    emitir_indentacao(nivel, saida);
    fputs("{\n", saida);

    if (bloco != NULL) {
        emitir_comandos(bloco->a, nivel + 1, pai, saida);
    }

    emitir_indentacao(nivel, saida);
    fputs("}\n", saida);
}

static void emitir_comandos(const NoAst *no, int nivel, const ContextoBloco *pai, FILE *saida)
{
    int renomeacoes_do_bloco = total_renomeacoes;
    int tipos_do_bloco = total_tipos_variaveis;

    for (; no != NULL; no = no->prox) {
        ContextoBloco ctx;

        ctx.restantes = no->prox;
        ctx.pai = pai;

        switch (no->tipo_no) {
            case NO_DECLARACAO:
                if (pai != NULL && declarado_adiante(pai, no->nome)) {
                    registrar_renomeacao(no->nome);
                }
                emitir_indentacao(nivel, saida);
                fprintf(saida, "%s ", mapear_tipo_csharp(no->tipo));
                emitir_identificador(no->nome, saida);
                if (no->a != NULL) {
                    fputs(" = ", saida);
                    emitir_expressao(no->a, saida);
                }
                registrar_tipo_variavel(no->nome, no->tipo);
                fputs(";\n", saida);
                break;

            case NO_ATRIBUICAO:
                emitir_indentacao(nivel, saida);
                emitir_expressao(no->a, saida);
                fprintf(saida, " %s ", no->nome ? no->nome : "=");
                emitir_expressao(no->b, saida);
                fputs(";\n", saida);
                break;

            case NO_PREFIXO:
            case NO_POSFIXO:
                emitir_indentacao(nivel, saida);
                emitir_expressao(no, saida);
                fputs(";\n", saida);
                break;

            case NO_WHILE:
                emitir_indentacao(nivel, saida);
                fputs("while (", saida);
                emitir_expressao(no->a, saida);
                fputs(")\n", saida);
                emitir_indentacao(nivel, saida);
                fputs("{\n", saida);
                emitir_comandos(no->b, nivel + 1, &ctx, saida);
                emitir_indentacao(nivel, saida);
                fputs("}\n", saida);
                break;

            case NO_IF:
                emitir_indentacao(nivel, saida);
                fputs("if (", saida);
                emitir_expressao(no->a, saida);
                fputs(")\n", saida);
                emitir_bloco(no->b, nivel, saida);
                if (no->c != NULL) {
                    emitir_indentacao(nivel, saida);
                    fputs("else\n", saida);
                    emitir_bloco(no->c, nivel, saida);
                }
                break;

            case NO_PRINTLN:
                emitir_indentacao(nivel, saida);
                fputs("Console.WriteLine(", saida);
                if (no->a != NULL) {
                    emitir_como_texto(no->a, saida);
                }
                fputs(");\n", saida);
                break;

            case NO_PRINT:
                emitir_indentacao(nivel, saida);
                fputs("Console.Write(", saida);
                if (no->a != NULL) {
                    emitir_como_texto(no->a, saida);
                }
                fputs(");\n", saida);
                break;

            case NO_PRINTF: {
                const NoAst *arg;
                emitir_indentacao(nivel, saida);
                fputs("Console.Write(", saida);
                if (no->a != NULL && no->a->tipo_no == NO_STRING && no->a->nome != NULL) {
                    emitir_formato_printf(no->a->nome, no->b != NULL, saida);
                } else {
                    emitir_expressao(no->a, saida);
                }
                for (arg = no->b; arg != NULL; arg = arg->prox) {
                    fputs(", ", saida);
                    emitir_como_texto(arg, saida);
                }
                fputs(");\n", saida);
                break;
            }

            default:
                break;
        }
    }

    total_renomeacoes = renomeacoes_do_bloco;
    total_tipos_variaveis = tipos_do_bloco;
}

static void emitir_parametros(const NoAst *parametro, FILE *saida)
{
    int primeiro = 1;

    for (; parametro != NULL; parametro = parametro->prox) {
        if (!primeiro) {
            fputs(", ", saida);
        }
        fprintf(saida, "%s%s ", mapear_tipo_csharp(parametro->tipo),
                parametro->valor.ival ? "[]" : "");
        emitir_identificador(parametro->nome, saida);
        primeiro = 0;
    }
}

static void emitir_metodo(const NoAst *metodo, FILE *saida)
{
    int eh_main = metodo->nome != NULL && strcmp(metodo->nome, "main") == 0;
    const NoAst *parametro;

    if (eh_main) {
        fputs("    public static void Main(string[] args)\n", saida);
    } else {
        fprintf(saida, "    public %s ", mapear_tipo_csharp(metodo->tipo));
        emitir_identificador(metodo->nome, saida);
        fputc('(', saida);
        emitir_parametros(metodo->a, saida);
        fputs(")\n", saida);
    }

    fputs("    {\n", saida);
    total_tipos_variaveis = 0;
    for (parametro = metodo->a; parametro != NULL; parametro = parametro->prox) {
        registrar_tipo_variavel(parametro->nome, parametro->valor.ival ? "array" : parametro->tipo);
    }
    emitir_comandos(metodo->b, 2, NULL, saida);
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

    total_renomeacoes = 0;
    contador_renomeacoes = 0;
    total_tipos_variaveis = 0;

    fputs("using System;\n\n", saida);
    fputs("class ", saida);
    emitir_identificador(raiz->nome, saida);
    fputs("\n{\n", saida);
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