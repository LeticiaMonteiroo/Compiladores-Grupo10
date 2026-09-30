# Decisões de Projeto

> Esta página reúne as escolhas técnicas e organizacionais do grupo e a
> justificativa por trás delas. O que foi de fato implementado está em
> [Escopo](compiladordetalhes/escopo.md); como isso se distribuiu nas sprints está em
> [Planejamento](planejamento.md) e [Entrega 1](entregas/entrega1.md).

## Linguagens e ferramentas

O compilador traduz um subconjunto de **Java** para **C#**, construído com **Flex** (análise léxica) e **Bison** (análise sintática), conforme o material da disciplina de Compiladores 1.

## Redução de escopo: sem orientação a objetos

O projeto não contempla os pilares de OO de Java: sem herança, interfaces, polimorfismo, construtores ou atributos de instância, e sem suporte a múltiplas classes por arquivo. Uma "classe", no escopo atual, é tratada como um **invólucro sintático único** para agrupar métodos estáticos — no espírito de uma classe utilitária, não de um objeto propriamente dito. O modificador `public` da classe é opcional.

Essa foi uma escolha consciente para viabilizar uma base sólida de análise léxica e sintática (tipos, modificadores e assinatura de método) dentro do prazo da primeira entrega, deixando expressões, corpo de método, parâmetros e controle de fluxo para as próximas sprints — não é uma lacuna por esquecimento (ver detalhes em [Escopo → Fora do escopo desta entrega](compiladordetalhes/escopo.md#fora-do-escopo-da-primeira-entrega)).

## Divisão do trabalho por área de escopo da linguagem

Em vez de dividir o trabalho por ferramenta (uma pessoa cuidando só do Flex, outra só do Bison), a equipe optou por dividir por **área da linguagem**: cada integrante ficou responsável por um conjunto de tokens — do levantamento no léxico até a tentativa de integração no parser:

- Guilherme: comandos + condicionais
- Maria Luana: classe única, `main` e variáveis
- Maria Eduarda: métodos, parâmetros e retorno
- Ígor: operadores matemáticos e relacionais
- Letícia: laços e operadores

Essa divisão funcionou bem para o levantamento de tokens (Sprint 2) e para a escrita das regras léxicas (Sprint 3), mas gerou mais conflitos de merge do que o esperado no momento de integrar tudo num único `parser.y` (Sprint 4), já que cada área havia evoluído sua parte da gramática de forma relativamente independente.

## Redução do escopo sintático na Sprint 4

Diante desses conflitos de integração, a decisão da equipe (conduzida por Guilherme) foi reescrever a gramática base do `parser.y` priorizando fechar uma versão **estável e sem conflitos de Bison** a tempo da entrega, em vez de tentar mesclar todas as regras sintáticas já desenvolvidas por área. Como consequência, ficaram de fora da gramática desta entrega — embora os tokens já existam no léxico — parâmetros de método, corpo de método com instruções, `return` dentro de uma produção, e o uso de operadores em expressões binárias/unárias.

## Metodologia de trabalho

A equipe adotou Kanban para acompanhar as tarefas (colunas "Backlog", "In progress", "In review" e "Done"), com o trabalho de cada sprint dividido por área de escopo da linguagem entre os integrantes, em vez de por ferramenta (ver [Metodologia](metodologia.md) e [Planejamento](planejamento.md)).

## Histórico de Versões

| Versão | Descrição                        | Autor         | Data  |
| ------ | --------------------------------- | ------------- | ----- |
| 1.0    | Criação da página de decisões     | Maria Eduarda | 23/09 |