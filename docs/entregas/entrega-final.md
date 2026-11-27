# Entrega Final — Relatório de implementação

**Data de entrega:** 27/Nov  
**Período coberto:** 10/Nov a 27/Nov  
**Planejamento:** [Sprints 8 e 9](../planejamento.md)


## Sprint 8 — Parâmetros de Métodos e Front-End de Saída (10/Nov – 18/Nov)

### Objetivos planejados

- Permitir parâmetros em métodos na gramática e na AST.
- Estender o léxico, a gramática e a AST para suportar `System.out.print`, `System.out.println` e a assinatura `main(String[] args)`.
- Iniciar a análise semântica com a validação de redeclaração de símbolos e a estrutura base do Parser.


### Atividades realizadas

- **Maria Eduarda:** permitiu parâmetros em métodos e implementou a validação de redeclaração de símbolos no Parser, além da estrutura base da análise semântica.
- **Igor:** adicionou suporte léxico e nós da AST para `System.out.print` e `println`, além das regras sintáticas para esses comandos e para a assinatura `main(String[] args)`.
- **Letícia, Guilherme e Maria Luana:** Iniciaram as atividades de suas respectivas issues para a entrega na próxima sprint. 

---

## Sprint 9 — Escopos, Inferência de Tipos, Geração de Código C# e Testes (19/Nov – 27/Nov)

### Objetivos planejados

- Concluir a análise semântica com escopos, detecção de variáveis não declaradas, inferência e validação de tipos.
- Implementar o gerador de código C# a partir da AST.
- Criar a infraestrutura de testes automatizados e ampliar a cobertura de casos léxicos, sintáticos, semânticos e de tradução.
- Integrar o pipeline completo e finalizar a documentação da arquitetura.


### Atividades realizadas

- **Guilherme:** adicionou escopos à tabela de símbolos e integrou o pipeline completo no `main.c`, atualizando o Makefile. Também atualizou parte da documentação.
- **Igor:** implementou o motor de inferência de tipos, colaborou na validação de tipos em declarações, atribuições, `while` e `print` e gerou código C# para comandos e expressões. Também atualizou o README da equipe.
- **Letícia:** adicionou parâmetros na declaração de métodos.
- **Maria Eduarda:** criou a estrutura base do gerador C#, a infraestrutura e o script automatizado de testes (`run_tests.sh`), reduziu o Lote 1 para 15 novos casos léxicos e sintáticos, reduziu o Lote 3 para 15 casos válidos de tradução Java → C# e finalizou a documentação da arquitetura e o roteiro de entrega.
- **Maria Luana:** implementou a verificação de variáveis não declaradas na AST, colaborou na validação de tipos e ficou responsável pelo Lote 2 de casos de análise semântica, também reduzindo o número do lote.


## Panorama técnico desde a Entrega 2

Este panorama resume as funcionalidades registradas no código e na suíte de
testes desta versão. As atividades e atribuições individuais acima seguem o
registro do grupo:

- **Parser e AST:** suporte ampliado para parâmetros de métodos, impressão,
  representação hierárquica da AST e construções já descritas em
  [Escopo](../compiladordetalhes/escopo.md).
- **Análise semântica:** módulos e verificações para redeclaração, uso de variáveis, inferência e compatibilidade de tipos e visibilidade entre escopos.
- **Geração de C#:** o codegen emite código para o subconjunto descrito em [Escopo](../compiladordetalhes/escopo.md); não é uma tradução completa de Java.
- **Testes e automação:** a suíte contém 121 casos de integração, 62 verificações unitárias (7 da AST, 11 da tabela de símbolos e 44 do codegen) e 45 verificações de linha de comando. Há 38 referências `.cs` e 15 saídas `.saida`. A meta original de 150 testes foi reduzida por limite de tempo, conforme [Decisões de Projeto](../decisoes.md).
- **Documentação e entrega:** o repositório contém instruções de compilação e uso, escopo do compilador, decisões de projeto e registros das entregas.


## Resultado final

| Área | Status na entrega |
| ---- | ----------------- |
| Análise léxica e sintática | Implementada para o subconjunto descrito em Escopo |
| AST e tabela de símbolos | Implementadas e cobertas por testes unitários |
| Análise semântica | Implementada para redeclarações, escopos e tipos descritos em Escopo |
| Geração de C# | Parcial; cobre o subconjunto descrito em Escopo |
| Testes | 121 casos de integração, 62 verificações unitárias e 45 de linha de comando |
| CI | Workflow configurado para executar `make test` e `make memcheck` no Ubuntu |


## Artefatos da entrega

| Artefato | Localização |
| -------- | ----------- |
| Lexer | `Projeto-Compilador/lexer/lexer.l` |
| Parser | `Projeto-Compilador/parser/parser.y` |
| AST | `Projeto-Compilador/src/ast.c` e `Projeto-Compilador/src/ast.h` |
| Tabela de símbolos | `Projeto-Compilador/src/tabela_simbolos.c` e `Projeto-Compilador/src/tabela_simbolos.h` |
| Análise semântica | `Projeto-Compilador/src/semantico.c` |
| Geração de C# | `Projeto-Compilador/src/codegen.c` |
| Testes | `Projeto-Compilador/tests/` |
| Documentação | `docs/` |


## Histórico de versões

| Versão | Descrição | Autor | Data |
| ------ | --------- | ----- | ---- |
| 0.1 | Criação do esqueleto da página da Entrega Final | Maria Eduarda | 27/11 |
