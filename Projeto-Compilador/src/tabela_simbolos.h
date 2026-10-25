#ifndef TABELA_SIMBOLOS_H
#define TABELA_SIMBOLOS_H

#include <stdio.h>

typedef enum {
    SIMBOLO_CLASSE,
    SIMBOLO_METODO,
    SIMBOLO_VARIAVEL
} CategoriaSimbolo;

typedef struct Simbolo {
    char *nome;
    char *tipo;                
    CategoriaSimbolo categoria;
    struct Simbolo *prox;
} Simbolo;

typedef struct {
    Simbolo *inicio;
    Simbolo *fim;
    int total;
} TabelaSimbolos;

TabelaSimbolos *tabela_criar(void);
void tabela_liberar(TabelaSimbolos *tabela);

// 1 se inseriu e 0 se ja existe
int tabela_inserir(TabelaSimbolos *tabela, const char *nome, const char *tipo, CategoriaSimbolo categoria);

const Simbolo *tabela_buscar(const TabelaSimbolos *tabela, const char *nome);

void tabela_imprimir(const TabelaSimbolos *tabela, FILE *saida);

#endif