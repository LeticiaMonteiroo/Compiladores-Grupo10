# Planejamento do Projeto - Compiladores 1 (Java para C#)

> As tarefas abaixo correspondem aos cards do quadro Kanban da equipe (ver
> [Metodologia](metodologia.md)). A equipe organizou o trabalho por **área da
> linguagem** — cada pessoa acompanha seu escopo de tokens do léxico até a
> integração no parser — em vez de dividir por ferramenta (Flex de um lado,
> Bison de outro). O resultado efetivo de cada sprint está registrado em
> [Entrega 1 → O que foi feito](entregas/entrega1.md) para as Sprints 1 a 4 e
> em [Entrega 2 → O que foi feito](entregas/entrega2.md) para as Sprints 5 e 6,
> que também registra o planejamento da Sprint 7. As Sprints 8 e 9 foram
> organizadas para a entrega final; o planejamento e as atividades realizadas
> estão descritos abaixo e complementados em [Entrega Final](entregas/entrega-final.md).

## Sprint 1: Setup e Definição da Linguagem

**Período:** 26/Ago a 02/Set

- **[Guilherme](https://github.com/GuilhermeCarvalho2024):** Estruturar o projeto (pastas e arquivos base) e configurar o Makefile.
- **[Ígor](https://github.com/igorvdaniel):** Criar a estrutura inicial da documentação (MkDocs) e homologar a primeira entrega do ambiente.
- **[Letícia](https://github.com/LeticiaMonteiroo):** Configurar o repositório, adicionar a equipe.
- **Definição de escopo (todos):** discutir e delimitar qual subconjunto de Java o compilador vai cobrir nesta entrega, dividindo o levantamento de tokens por área:
    - **[Guilherme](https://github.com/GuilhermeCarvalho2024):** Comandos + condicionais. 
    - **[Maria Luana](https://github.com/MLuana725):** classe única, `main` e variáveis.
    - **[Maria Eduarda](https://github.com/pyramidsf):** métodos, parâmetros e retorno.
    - **[Ígor](https://github.com/igorvdaniel):** operadores.
    - **[Letícia](https://github.com/LeticiaMonteiroo):** laços e operadores.

---

## Sprint 2: Análise Léxica e Base da Gramática

**Período:** 02/Set a 09/Set

- **[Guilherme](https://github.com/GuilhermeCarvalho2024):** Revisar o `lexer.l`, consolidando as regras trazidas por cada área e removendo redundâncias.
- **[Maria Luana](https://github.com/MLuana725):** Escrever as regras léxicas do seu escopo (classe, `main`, variáveis) e integrar sua branch, resolvendo conflitos com a `main`.
- **[Maria Eduarda](https://github.com/pyramidsf):** Escrever as regras léxicas de métodos, parâmetros e retorno.
- **[Ígor](https://github.com/igorvdaniel): Escrever as regras léxicas de operadores matemáticos e relacionais. 
- **[Letícia](https://github.com/LeticiaMonteiroo):** Escrever as regras léxicas de laços + operadores lógicos.
- **Meta da sprint:** fechar o `lexer.l` reconhecendo tipos primitivos, modificadores, palavras reservadas, todos os operadores e comentários — base completa para o parser (ver [Escopo](compiladordetalhes/escopo.md)).

---

## Sprint 3: O Parser Sintático e Integração

**Período:** 09/Set a 16/Set

- **[Guilherme](https://github.com/GuilhermeCarvalho2024):** Reescrever a gramática base do `parser.y`, priorizando fechar uma versão estável e sem conflitos de Bison a tempo da entrega — o que levou a uma **redução do escopo sintático** (parâmetros, corpo de método e uso de operadores em expressões ficaram para a próxima sprint; justificativa detalhada em [Escopo](compiladordetalhes/escopo.md)).
- **[Maria Luana](https://github.com/MLuana725):** Integrar seu trabalho de léxico à nova gramática, resolvendo conflitos de merge entre sua branch e a `main`.
- **Demais integrantes:** validar se os tokens levantados nas sprints anteriores continuavam compatíveis com a gramática reduzida.

---

## Sprint 4: Homologação, Preparação e Entrega Final

**Período:** 16/Set a 23/Set

- **[Guilherme](https://github.com/GuilhermeCarvalho2024):** Criar a suíte de testes e configurar o workflow de CI para rodar os testes automaticamente.
- **[Maria Luana](https://github.com/MLuana725):** Auxiliar no preenchimento do formulário da primeira entrega. 
- **[Maria Eduarda](https://github.com/pyramidsf):** Escrever a documentação da entrega.
- **[Ígor](https://github.com/igorvdaniel):** Revisar e complementar a documentação, além de participar do preenchimento do formulário da Entrega 1.
- **[Letícia](https://github.com/LeticiaMonteiroo):** Montar os slides da apresentação da primeira entrega.

---

## Sprint 5: Evolução do Parser e Estrutura do Compilador

**Período:** 23/Set a 22/Out

Esta sprint teve duração maior que as anteriores e foi concluída em 22/Out.

- **[Maria Luana](https://github.com/MLuana725):** Inserir operações aritméticas e regras de precedência no parser.
- **[Maria Eduarda](https://github.com/pyramidsf):** Adicionar ao parser reatribuições de valores de variáveis e operações de incremento e decremento.
- **[Ígor](https://github.com/igorvdaniel):** Adicionar declaração de variável ao parser.
- **[Letícia](https://github.com/LeticiaMonteiroo):** Iniciar a criação de mais testes.
- **[Guilherme](https://github.com/GuilhermeCarvalho2024):** Iniciar a estrutura interna do compilador, incluindo árvore sintática e tabela de símbolos.

---

## Sprint 6: AST, Tabela de Símbolos e Evolução do Parser

**Período:** 23/Out a 31/Out

- **[Guilherme](https://github.com/GuilhermeCarvalho2024) e [Maria Eduarda](https://github.com/pyramidsf):** Implementar a AST para representar classes, métodos, declarações, laços, atribuições, expressões e literais, e integrá-la ao parser.
- **[Guilherme](https://github.com/GuilhermeCarvalho2024) e [Ígor](https://github.com/igorvdaniel):** Implementar a tabela de símbolos para registrar classes, métodos e variáveis.
- **[Maria Luana](https://github.com/MLuana725):** Ampliar a gramática para reconhecer laços `while`, expressões aritméticas, relacionais e lógicas e inicialização de variáveis.
- **[Letícia](https://github.com/LeticiaMonteiroo):** Ampliar a suíte com casos de comentários de linha e bloco, AST e tabela de símbolos, incluindo testes unitários executados pelo alvo `make test`.

---

## Sprint 7: Preparação da Entrega 2

**Período:** 1/Nov a 9/Nov

- **[Letícia](https://github.com/LeticiaMonteiroo):** Implementar novos casos de teste para ampliar a cobertura do compilador.
- **[Maria Eduarda](https://github.com/pyramidsf):** Confeccionar e revisar a documentação da Entrega 2.
- **Todos:** Confeccionar o formulário da Entrega 2.
- **Todos:** Preparar os slides da apresentação da Entrega 2.

---

## Sprint 8: Ampliação do Parser e Análise Semântica

**Período:** 10/Nov a 18/Nov

- **[Maria Eduarda](https://github.com/pyramidsf):** Permitiu parâmetros em métodos e implementou a validação de redeclaração de símbolos no Parser, além da estrutura base da análise semântica.
- **[Ígor](https://github.com/igorvdaniel):** Adicionou suporte léxico e nós da AST para `System.out.print` e `println`, além das regras sintáticas para esses comandos e para a assinatura `main(String[] args)`.
- **[Letícia](https://github.com/LeticiaMonteiroo), [Guilherme](https://github.com/GuilhermeCarvalho2024) e [Maria Luana](https://github.com/MLuana725):** Iniciaram as atividades de suas respectivas issues para a entrega na sprint seguinte.

---

## Sprint 9: Geração de C# e Consolidação da Entrega Final

**Período:** 19/Nov a 27/Nov

- **[Guilherme](https://github.com/GuilhermeCarvalho2024):** Adicionou escopos à tabela de símbolos e integrou o pipeline completo no `main.c`, atualizando o Makefile. Também atualizou parte da documentação.
- **[Ígor](https://github.com/igorvdaniel):** Implementou o motor de inferência de tipos, colaborou na validação de tipos em declarações, atribuições, `while` e `print`, e gerou código C# para comandos e expressões. Também atualizou o README da equipe.
- **[Letícia](https://github.com/LeticiaMonteiroo):** Adicionou parâmetros na declaração de funções.
- **[Maria Eduarda](https://github.com/pyramidsf):** Criou a estrutura base do gerador C#, a infraestrutura e o script automatizado de testes (`run_tests.sh`), reduziu o Lote 1 para 15 novos casos léxicos e sintáticos, reduziu o Lote 3 para 15 casos válidos de tradução Java → C# e finalizou a documentação da arquitetura e o roteiro de entrega.
- **[Maria Luana](https://github.com/MLuana725):** Implementou a verificação de variáveis não declaradas na AST, colaborou na validação de tipos e ficou responsável pelo Lote 2 de casos de análise semântica, também reduzindo o número do lote.

> O período entre a Entrega 2 (9/Nov) e a entrega final (27/Nov) foi dividido
> em dois blocos consecutivos de nove dias para organizar o trabalho. As
> atividades realizadas e seus responsáveis estão listados acima; o registro
> da entrega final está em [Entrega Final](entregas/entrega-final.md).
