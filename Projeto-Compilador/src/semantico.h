#ifndef SEMANTICO_H
#define SEMANTICO_H

#include "ast.h"
#include "tabela_simbolos.h"

void semantico_registrar_redeclaracao(int linha, const char *nome);
void semantico_registrar_falha_insercao(int linha, const char *nome);
const char *inferir_tipo_expressao(const NoAst *no, const TabelaSimbolos *tabela);
int analisar_semantica(const NoAst *raiz, const TabelaSimbolos *tabela);

#endif