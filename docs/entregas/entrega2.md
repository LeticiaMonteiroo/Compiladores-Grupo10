# Entrega 2 — O que foi feito

> Esta página registra o trabalho realizado nas Sprints 5, 6 e 7
> para a Entrega 2. O planejamento está em
> [Planejamento](../planejamento.md); o escopo atualmente reconhecido está em
> [Escopo](../compiladordetalhes/escopo.md).

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

## Sprint 6 — AST, Tabela de Símbolos e Evolução do Parser (23/Out - 31/Out)

Nesta sprint, o compilador avançou além do reconhecimento sintático e passou a
construir e exibir estruturas internas durante uma análise bem-sucedida.

- **Guilherme e Maria Eduarda:** Implementar a AST para representar classes, métodos,
  declarações, laços, atribuições, expressões e literais, e integrá-la ao
  parser para exibição após uma análise bem-sucedida.
- **Guilherme e Igor:** Implementar a tabela de símbolos para registrar classes,
  métodos e variáveis, com impressão ao final da análise. A tabela ainda não
  realiza uma análise semântica completa.
- **Maria Luana:** Ampliar a gramática para reconhecer laços `while`,
  expressões aritméticas, relacionais e lógicas e inicialização de variáveis.
- **Letícia:** Ampliar a suíte com casos de comentários de linha e bloco,
  AST e tabela de símbolos, incluindo testes unitários para essas estruturas
  e sua execução pelo alvo `make test`.

O programa continua sendo um analisador: ainda não gera código C# nem executa
os programas analisados. Os detalhes e limites atuais estão em
[Escopo](../compiladordetalhes/escopo.md).

## Sprint 7 — Preparação da Entrega 2 (1/Nov - 9/Nov)

Esta sprint reúne as atividades de implementação e preparação dos materiais
para a Entrega 2.

- **Letícia:** Implementar novos casos de teste para ampliar a cobertura
  do compilador.
- **Maria Eduarda:** Confeccionar e revisar a documentação da Entrega 2.
- **Todos:** Confeccionar o formulário da Entrega 2.
- **Todos:** Preparar os slides da apresentação da Entrega 2.

## Histórico de Versões

| Versão | Descrição                                      | Autor         | Data  |
| ------ | ---------------------------------------------- | ------------- | ----- |
| 1.0    | Criação da página com o resumo da Entrega 2    | Maria Eduarda | 22/10 |
| 1.1    | Registro das atividades da Sprint 6            | Maria Eduarda | 25/10 |
| 1.2    | Registro das atividades da Sprint 7            | Maria Eduarda | 9/11  |
