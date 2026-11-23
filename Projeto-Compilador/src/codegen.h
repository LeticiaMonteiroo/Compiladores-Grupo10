#ifndef CODEGEN_H
#define CODEGEN_H

#include <stdio.h>
#include "ast.h"

void gerar_codigo_csharp(const NoAst *raiz, FILE *saida);
const char *mapear_tipo_csharp(const char *tipo_java);

#endif
