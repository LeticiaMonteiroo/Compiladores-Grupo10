#ifndef TESTE_UTIL_H
#define TESTE_UTIL_H

#include <stdio.h>

static int total_testes = 0;
static int testes_ok = 0;

#define ESPERAR(condicao) \
    do { \
        if (!(condicao)) { \
            printf("       condicao falsa: %s (linha %d)\n", #condicao, __LINE__); \
            return 0; \
        } \
    } while (0)

#define EXECUTAR_TESTE(funcao) \
    do { \
        total_testes++; \
        if (funcao()) { \
            testes_ok++; \
            printf("[OK]   %s -> passou (correto)\n", #funcao); \
        } else { \
            printf("[FAIL] %s -> falhou (ERRADO, deveria passar)\n", #funcao); \
        } \
    } while (0)

static int resumo_testes(void)
{
    printf("\nResultado: %d/%d casos de teste corretos.\n", testes_ok, total_testes);
    return testes_ok == total_testes ? 0 : 1;
}

static const char *ler_e_fechar(FILE *arquivo)
{
    static char buffer[2048];
    size_t lidos;

    rewind(arquivo);
    lidos = fread(buffer, 1, sizeof(buffer) - 1, arquivo);
    buffer[lidos] = '\0';
    fclose(arquivo);
    return buffer;
}

#endif