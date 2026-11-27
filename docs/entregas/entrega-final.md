# Entrega Final — Esqueleto de registro

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

- **Guilherme:** adicionou escopos à tabela de símbolos e integrou o pipeline completo no `main.c`, atualizando o Makefile. Também atualizou parte da documentação
- **Igor:** implementou o motor de inferência de tipos, colaborou na validação de tipos em declarações, atribuições, `while` e `print` e gerou código C# para comandos e expressões. Também tualizou o README da equipe. 
- **Letícia:** adicionou parâmetros na declaração de funções.
- **Maria Eduarda:** criou a estrutura base do gerador C#, a infraestrutura e o script automatizado de testes (`run_tests.sh`), reduziu o Lote 1 para 15 novos casos léxicos e sintáticos, reduziu o Lote 3 para 15 casos válidos de tradução Java → C# e finalizou a documentação da arquitetura e o roteiro de entrega.
- **Maria Luana:** implementou a verificação de variáveis não declaradas na AST, colaborou na validação de tipos e ficou responsável pelo Lote 2 de casos de análise semântica, também reduzindo o número do lote.


## Panorama técnico desde a Entrega 2

O histórico do repositório e os artefatos atuais apontam estas frentes como
base para completar o relato. Confirmem o escopo, a autoria e o estado final de
cada item antes da apresentação:

- **Parser e AST:** suporte ampliado para parâmetros de métodos, impressão,
  representação hierárquica da AST e construções já descritas em
  [Escopo](../compiladordetalhes/escopo.md).
- **Análise semântica:** módulos e verificações para redeclaração, uso de variáveis, inferência e compatibilidade de tipos e visibilidade entre escopos.
- **Geração de C#:** estrutura de codegen para emitir código do subconjunto reconhecido, incluindo declarações, expressões, atribuições, `while` e comandos de impressão. A tradução continua parcial; registrar limitações confirmadas na versão entregue.
- **Testes e automação:** testes unitários para AST, tabela de símbolos e codegen; testes de integração; e script para executar a suíte. Na branch de trabalho desta página, há 95 casos de integração e 21 casos unitários. A meta original de 150 testes foi reduzida por limite de tempo, conforme [Decisões de Projeto](../decisoes.md).
- **Documentação e entrega:** instruções de compilação e uso, escopo do compilador, decisões, materiais de apresentação e preparação da versão/tag final. Preencher o estado efetivamente alcançado.


## Resultado final

| Área | Status na entrega |
| ---- | ----------------- |
| Análise léxica e sintática | OK |
| AST e tabela de símbolos | OK |
| Análise semântica | OK |
| Geração de C# | OK |
| Testes e integração contínua | OK |
| Documentação e materiais | OK |


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
