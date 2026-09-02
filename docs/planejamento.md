# Planejamento do Projeto - Compiladores 1 (Java para C#)

## Sprint 1: Setup e Definição da Linguagem
**Período:** 26/Ago a 02/Set

* **[Ígor](https://github.com/igorvdaniel):** Escrever o Documento Inicial definindo o escopo reduzido, estruturar o quadro Kanban/Notion da equipe e homologar a primeira entrega do ambiente.
* **[Letícia](https://github.com/LeticiaMonteiroo):** Configurar o repositório, adicionar a equipe.
* **[Guilherme](https://github.com/GuilhermeCarvalho2024):** Configurar o ambiente Flex local, criar um `.l` básico (Hello World) e criar o Makefile base.
* **[Maria Luana]():** Configurar o ambiente Bison, criar o rascunho `.y` e mapear a conexão inicial com o Flex.
* **[Maria Eduarda](https://github.com/pyramidsf):** Criar os primeiros arquivos de texto (códigos fictícios em Java) que servirão de base para os testes.

---

## Sprint 2: Análise Léxica e Base da Gramática
**Período:** 02/Set a 09/Set

* **[Ígor](https://github.com/igorvdaniel):** Refinar o documento da linguagem listando formalmente os *Tokens*.
* **[Letícia](https://github.com/LeticiaMonteiroo):** Criar os scripts de automação simples para rodar o analisador léxico contra os códigos de teste.
* **[Guilherme](https://github.com/GuilhermeCarvalho2024):** Validar se os tokens gerados fazem sentido com as regras da linguagem desenhadas até agora.
* **[Maria Luana]():** Finalizar as expressões regulares no arquivo `.l` para reconhecer todos os tokens (palavras reservadas, operadores, etc.).
* **[Maria Eduarda](https://github.com/pyramidsf):** Escrever a Gramática Livre de Contexto (GLC) e iniciar as regras correspondentes no `.y`.

---

## Sprint 3: O Parser Sintático e Integração
**Período:** 09/Set a 16/Set

* **[Ígor](https://github.com/igorvdaniel):** Redigir as respostas preliminares do Formulário P1, documentar o status da integração estrutural.
* **[Letícia](https://github.com/LeticiaMonteiroo):** Auxiliar na integração C/C++ do código Lex/Bison e resolver eventuais bugs de compilação no Makefile.
* **[Guilherme](https://github.com/GuilhermeCarvalho2024):** Implementar as regras sintáticas funcionais no Bison (ex: validação correta de blocos `if/while`).
* **[Maria Luana]():** Testar as falhas do Lex, focar na correção de bugs léxicos e validar se espaços/comentários estão sendo ignorados.
* **[Maria Eduarda](https://github.com/pyramidsf):** Rodar a integração completa (Léxica + Sintática), documentar onde o parser quebra e reportar os erros aos desenvolvedores.

---

## Sprint 4: Homologação, Preparação P1 e Entrega Final
**Período:** 16/Set a 23/Set

* **[Ígor](https://github.com/igorvdaniel):** Realizar o *Code Freeze* em 21/Set, preencher o formulário oficial, fechar os slides, organizar o pacote de arquivos finais e liderar o ensaio da apresentação (5 min).
* **[Letícia](https://github.com/LeticiaMonteiroo):** Auxiliar Ígor na montagem visual dos slides da Apresentação P1 e revisão final do README do repositório.
* **[Guilherme](https://github.com/GuilhermeCarvalho2024):** Executar a bateria final de testes homologados e registrar as evidências (prints) de sucesso para os slides.
* **[Maria Luana]():** Fazer o polimento do tratamento de erros no arquivo `.l` (ex: garantir que ele identifique a linha do erro no código).
* **[Maria Eduarda](https://github.com/pyramidsf):** Implementar a função `yyerror` no Bison para gerar mensagens de erro sintático claras ao usuário.