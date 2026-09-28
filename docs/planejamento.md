# Planejamento do Projeto - Compiladores 1 (Java para C#)

> As tarefas abaixo correspondem aos cards do quadro Kanban da equipe (ver
> [Metodologia](metodologia.md)). A equipe organizou o trabalho por **área da
> linguagem** — cada pessoa acompanha seu escopo de tokens do léxico até a
> integração no parser — em vez de dividir por ferramenta (Flex de um lado,
> Bison de outro). O resultado efetivo de cada sprint está registrado em
> [Entrega 1 → O que foi feito](entregas/entrega1.md).

## Sprint 1: Setup e Definição da Linguagem

**Período:** 26/Ago a 02/Set

- **[Guilherme](https://github.com/GuilhermeCarvalho2024):** Estruturar o projeto (pastas e arquivos base) e configurar o Makefile.
- **[Ígor](https://github.com/igorvdaniel):** Criar a estrutura inicial da documentação (MkDocs) e homologar a primeira entrega do ambiente.
- **[Letícia](https://github.com/LeticiaMonteiroo):** Configurar o repositório, adicionar a equipe.
- **Definição de escopo (todos):** discutir e delimitar qual subconjunto de Java o compilador vai cobrir nesta entrega, dividindo o levantamento de tokens por área:
    - **[Guilherme](https://github.com/GuilhermeCarvalho2024):** Comandos + condicionais
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
- **Meta da sprint:** fechar o `lexer.l` reconhecendo tipos primitivos, modificadores, palavras reservadas, todos os operadores e comentários — base completa para o parser (ver [Escopo](escopo.md)).

---

## Sprint 3: O Parser Sintático e Integração

**Período:** 09/Set a 16/Set

- **[Guilherme](https://github.com/GuilhermeCarvalho2024):** Reescrever a gramática base do `parser.y`, priorizando fechar uma versão estável e sem conflitos de Bison a tempo da entrega — o que levou a uma **redução do escopo sintático** (parâmetros, corpo de método e uso de operadores em expressões ficaram para a próxima sprint; justificativa detalhada em [Escopo](escopo.md)).
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

