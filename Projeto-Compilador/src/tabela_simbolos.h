#ifndef TABELA_SIMBOLOS_H
#define TABELA_SIMBOLOS_H

#include <stdio.h>

typedef enum {
    SIMBOLO_CLASSE,
    SIMBOLO_METODO,
    SIMBOLO_VARIAVEL,
    SIMBOLO_PARAMETRO
} CategoriaSimbolo;

typedef enum {
    ESCOPO_GLOBAL,   
    ESCOPO_METODO,   
    ESCOPO_BLOCO     
} TipoEscopo;

typedef struct Escopo Escopo;

typedef struct Simbolo {
    char *nome;
    char *tipo;                
    CategoriaSimbolo categoria;
    int ordem;                       
    const Escopo *escopo;            
    struct Simbolo *prox;           
    struct Simbolo *prox_no_escopo; 
} Simbolo;

struct Escopo {
    int id;
    int nivel;                       
    TipoEscopo tipo;
    char *rotulo;                    
    struct Escopo *pai;
    struct Escopo *prox_escopo;      
    Simbolo *primeiro;
    Simbolo *ultimo;
};

typedef struct {
    Simbolo *inicio;                 
    Simbolo *fim;
    int total;
    Escopo *global;
    Escopo *atual;                   
    Escopo *escopos;                 
    Escopo *escopos_fim;
    int total_escopos;
} TabelaSimbolos;

TabelaSimbolos *tabela_criar(void);
void tabela_liberar(TabelaSimbolos *tabela);

Escopo *tabela_entrar_escopo(TabelaSimbolos *tabela, TipoEscopo tipo, const char *rotulo);

void tabela_sair_escopo(TabelaSimbolos *tabela);

const Escopo *tabela_escopo_atual(const TabelaSimbolos *tabela);

// 1 se inseriu e 0 se ja existe
int tabela_inserir(TabelaSimbolos *tabela, const char *nome, const char *tipo, CategoriaSimbolo categoria);

const Simbolo *tabela_buscar_conflito(const TabelaSimbolos *tabela, const char *nome, CategoriaSimbolo categoria);

const Simbolo *tabela_buscar(const TabelaSimbolos *tabela, const char *nome);

const Simbolo *tabela_buscar_em(const Escopo *escopo, const char *nome);

const Simbolo *tabela_buscar_qualquer(const TabelaSimbolos *tabela, const char *nome);

void tabela_imprimir(const TabelaSimbolos *tabela, FILE *saida);

void tabela_imprimir_escopos(const TabelaSimbolos *tabela, FILE *saida);

#endif