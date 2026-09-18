# Entrega 1 — Resumo do que foi feito

> Cobre as Sprints 1 a 4 (26/Ago a 23/Set). Ver o planejado em
> [Planejamento](../planejamento.md) e o escopo coberto em [Escopo](../escopo.md).

## Sprint 1 — Setup e Definição da Linguagem
- [ ] Repositório configurado, equipe adicionada
- [ ] Ambiente Flex/Bison funcionando (Makefile base)
- [ ] Primeiros `.java` de teste criados

## Sprint 2 — Análise Léxica e Base da Gramática
- [ ] Tokens formalizados (ver [Escopo](../escopo.md))
- [ ] `lexer.l` reconhecendo palavras reservadas / operadores
- [ ] GLC inicial no `parser.y`

## Sprint 3 — Parser Sintático e Integração
- [ ] Regras sintáticas de [operadores / métodos-parâmetros-retorno] funcionais
- [ ] Integração lexer + parser compilando sem conflitos (bison -d sem warnings)
- [ ] Bugs conhecidos / pendências

## Sprint 4 — Homologação e Entrega
- [ ] Bateria de testes rodada (casos válidos e inválidos)
- [ ] `yyerror` com mensagens claras
- [ ] Slides / formulário P1 preenchidos

## Artefatos no repositório

| Componente | Localização        |
| ---------- | ------------------- |
| Lexer      | `lexer/lexer.l`     |
| Parser     | `parser/parser.y`   |
| Build      | `Makefile`           |
| Testes     | `tests/` [ajustar]  |

## Histórico de Versões

| Versão | Descrição            | Autor | Data |
| ------ | --------------------- | ----- | ---- |
| 1.0    | Criação do documento  |       |      |