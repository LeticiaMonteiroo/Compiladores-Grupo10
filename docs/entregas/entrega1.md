# Entrega 1 — O que foi feito

> Cobre as Sprints 1 a 4 (26/Ago a 23/Set). O planejado por sprint está em
> [Planejamento](../planejamento.md); o detalhamento do escopo realmente
> implementado está em [Escopo](../escopo.md). Esta página é o meio-termo
> entre os dois: conta o que a equipe efetivamente fez, sprint a sprint.

## Sprint 1 — Setup e Definição da Linguagem (26/Ago – 02/Set)

- **Guilherme** montou a estrutura inicial do projeto (pastas e arquivos base).
- **Igor** fez a documentação inicial (estrutura do MkDocs, página Home).
- A definição do escopo da linguagem foi um esforço coletivo: **Maria Luana,
  Maria Eduarda, Igor e Letícia** participaram da discussão de quais recursos
  de Java o compilador cobriria nesta primeira entrega.
- Como parte dessa definição, cada integrante ficou responsável por levantar
  os tokens de uma área da linguagem:
    - **Maria Luana** — classe única, `main` e variáveis
    - **Maria Eduarda** — métodos, parâmetros e retorno
    - **Igor** — operadores
    - **Letícia** — laços e operadores

## Sprint 2 — Análise Léxica e Base da Gramática (02/Set – 09/Set)

- **Guilherme** revisou o `lexer.l`, removendo redundâncias entre as regras
  que cada área da linguagem foi trazendo.
- **Maria Luana** escreveu as regras léxicas do seu escopo (classe, `main`,
  variáveis) e ajudou a resolver um conflito entre sua branch e a `main`
  durante a integração desse trabalho.
- Os demais tokens levantados na Sprint 1 (métodos/parâmetros/retorno,
  operadores, laços) foram incorporados ao `lexer.l` nesta fase, formando a
  base léxica que hoje reconhece: tipos primitivos, modificadores,
  `return` como palavra reservada, todos os operadores aritméticos,
  relacionais, lógicos, de atribuição e de bits, e comentários de linha/bloco
  (ver tabela completa em [Escopo](../escopo.md)).

## Sprint 3 — Parser Sintático e Integração (09/Set – 16/Set)

- **Guilherme** reescreveu a gramática básica do `parser.y`, **reduzindo o
  escopo** para fechar uma base sintática estável e sem conflitos de bison a
  tempo da entrega.
- **Maria Luana** resolveu conflitos de merge entre sua branch e a `main`
  durante essa integração.
- Como consequência dessa redução de escopo, ficaram **fora da gramática
  desta entrega** (embora os tokens já existam no léxico): parâmetros de
  método, corpo de método com instruções, `return` dentro de uma produção, e
  o uso de operadores em expressões binárias/unárias. Isso está documentado
  em detalhe, com a justificativa, em
  [Escopo → Fora do escopo desta entrega](../escopo.md#fora-do-escopo-desta-entrega).
- O que ficou de pé e funcional: declaração de método com modificadores
  (`public`/`private`/`protected`/`static`), tipo de retorno e nome —
  aceitando `nome()` com corpo `{}` vazio.

## Sprint 4 — Homologação e Preparação da Entrega 1 (16/Set – 23/Set)

- **Guilherme** criou a suíte de testes e configurou o workflow de CI para
  rodar os testes automaticamente a cada push.
- **Maria Eduarda** ficou responsável pela documentação desta entrega
  (páginas de Escopo, Planejamento e este resumo).
- **Maria Luana** auxiliou no preenchimento do formulário. 
- **Igor** ajudou na revisão e complementação da documentação, além do preenchimento do formulário.
- **Letícia** montou os slides da apresentação da primeira entrega.


## Resultado — Escopo alcançado nesta entrega

| Área                             | Status                                             |
| --------------------------------- | --------------------------------------------------- |
| Tipos primitivos e modificadores  | Implementado (léxico + sintaxe)                  |
| Assinatura de método              | Implementado (`() { }` vazios)                   |
| Parâmetros de método              | Tokens levantados, gramática pendente            |
| Corpo de método / `return`        | `return` reconhecido no léxico, sem produção ainda |
| Operadores                        | Todos reconhecidos no léxico, sem uso na gramática |
| Comentários                       | Implementado                                     |

Detalhamento completo, com o que ficou de fora e por quê, em [Escopo](../escopo.md).

## Artefatos no repositório

| Componente | Localização                  |
| ---------- | ----------------------------- |
| Lexer      | `Projeto-Compilador/lexer/lexer.l`               |
| Parser     | `Projeto-Compilador/parser/parser.y`             |
| Build      | `Projeto-Compilador/Makefile`                    |
| Testes     | `Projeto-Compilador/tests/` (com workflow de CI) |
| Docs       | `docs/` (este site)           |

## Histórico de Versões

| Versão | Descrição                                      | Autor          | Data      |
| ------ | ----------------------------------------------- | -------------- | --------- |
| 1.0    | Criação da página com o resumo da Entrega 1     | Maria Eduarda  | 23/09     |