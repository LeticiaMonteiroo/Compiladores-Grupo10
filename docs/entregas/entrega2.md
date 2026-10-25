# Entrega 2 — O que foi feito

> Esta página registra o trabalho realizado para a Entrega 2, sprint a sprint.
> Por enquanto, cobre a Sprint 5. O planejamento está em
> [Planejamento](../planejamento.md); o escopo implementado será detalhado
> conforme a entrega evoluir.

## Sprint 5 — Evolução do Parser e Estrutura do Compilador (23/Set – 22/Out)

Esta sprint teve duração maior que as anteriores e foi concluída em 22/Out.
O trabalho avançou a gramática do parser e iniciou a construção da estrutura
interna do compilador.

- **Maria Luana** inseriu operações aritméticas e regras de precedência no
  parser.
- **Maria Eduarda** adicionou ao parser reatribuições de valores de variáveis
  e operações de incremento e decremento.
- **Ígor** adicionou declaração de variável ao parser.
- **Letícia** iniciou a criação de mais testes para o compilador.
- **Guilherme** iniciou a estrutura interna do compilador, incluindo árvore
  sintática e tabela de símbolos.

## Resultado — Trabalho registrado nesta sprint

| Área                              | Status nesta sprint                                      |
| --------------------------------- | -------------------------------------------------------- |
| Operações aritméticas e precedência | Inseridas no parser                                    |
| Reatribuição de variáveis         | Adicionada ao parser                                     |
| Incremento e decremento           | Adicionados ao parser                                    |
| Declaração de variáveis           | Adicionada ao parser                                     |
| Testes                            | Criação de novos testes iniciada                         |
| Estrutura interna                 | Implementação inicial de árvore sintática e tabela de símbolos |

## Artefatos no repositório

| Componente | Localização                              |
| ---------- | ---------------------------------------- |
| Lexer      | `Projeto-Compilador/lexer/lexer.l`       |
| Parser     | `Projeto-Compilador/parser/parser.y`     |
| Build      | `Projeto-Compilador/Makefile`            |
| Testes     | `Projeto-Compilador/tests/`              |
| Docs       | `docs/` (este site)                      |

## Histórico de Versões

| Versão | Descrição                                      | Autor         | Data  |
| ------ | ---------------------------------------------- | ------------- | ----- |
| 1.0    | Criação da página com o resumo da Entrega 2    | Maria Eduarda | 22/10 |
