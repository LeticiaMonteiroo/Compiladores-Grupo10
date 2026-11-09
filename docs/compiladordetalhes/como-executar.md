# Como executar o projeto

As instruções abaixo descrevem o estado atual do compilador, que faz análise léxica e sintática de um subconjunto da linguagem Java.

## Pré-requisitos

- `make`
- Um compilador C disponível como `gcc`
- Flex (`flex`)
- A biblioteca de execução do Flex (`libfl`), usada na linkagem
- Bison (`bison`)

Para conferir se as ferramentas estão instaladas, execute `make --version`,
`gcc --version`, `flex --version` e `bison --version` no terminal.

Os comandos a seguir devem ser executados a partir da raiz do repositório, entrando primeiro na pasta do compilador:

```sh
cd Projeto-Compilador
```

## Compilar

Execute:

```sh
make
```

O Makefile gera os arquivos do lexer e do parser com Flex e Bison e compila o
executável `compilador`.

## Executar o compilador

Para iniciar a análise e digitar um programa pela entrada padrão:

```sh
make run
```

Digite ou cole o código-fonte e pressione `Ctrl+D` em uma linha vazia para indicar o fim da entrada. Por exemplo:

```java
public class Exemplo {
    public void metodo() {
    }
}
```

Também é possível fornecer um arquivo como entrada, sem iniciar o modo de digitação:

```sh
./compilador < tests/02_metodo_simples.txt
```

Quando a análise sintática é bem sucedida, o programa exibe a árvore
sintática abstrata (AST) e a tabela de símbolos. Ele ainda não traduz o código
para C# nem executa o programa de entrada.

## Testar de forma automatizada

Execute:

```sh
make test
```

O alvo compila o executável e os testes unitários de AST e tabela de símbolos,
executa esses testes e roda `tests/testes.sh` para os arquivos `.txt` da pasta
de testes.

### O que o script verifica

- Arquivos cujo nome contém `ERRO` devem ser rejeitados pelo compilador, com
  código de saída diferente de zero.
- Os demais arquivos devem ser aceitos, com código de saída zero.
- Os testes unitários verificam criação, composição, liberação e impressão da
  AST, além de inserção, busca, rejeição de nomes duplicados e impressão da
  tabela de símbolos.
- O script exibe `[OK]` ou `[FAIL]` para cada caso e, ao final, o total de
  resultados corretos.

A verificação compara os códigos de saída, não o conteúdo das mensagens produzidas. Confira o resumo e procure por `[FAIL]` no resultado de `make test`.
