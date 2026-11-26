# Como compilar, executar e testar

Estas instruções descrevem o estado atual: análise léxica, sintática e
semântica de um subconjunto de Java e geração parcial de C#. Para conhecer o
fluxo e as limitações, consulte [Escopo da Linguagem](./escopo.md).

## Pré-requisitos

- `make`
- Um compilador C disponível como `gcc`
- Flex (`flex`)
- A biblioteca de execução do Flex (`libfl`), usada na linkagem
- Bison (`bison`)

Para conferir se as ferramentas estão instaladas, execute `make --version`,
`gcc --version`, `flex --version` e `bison --version` no terminal.

Para compilar e executar o compilador, entre na pasta do projeto:

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

Para analisar um arquivo:

```sh
./compilador tests/validos/39_hello_world.java
```

Também é possível redirecionar um arquivo pela entrada padrão:

```sh
./compilador < tests/validos/39_hello_world.java
```

Se as análises sintática e semântica forem bem-sucedidas, o programa exibe a
AST e a tabela de símbolos. Para gerar C# automaticamente com o nome da classe
como nome do arquivo:

```sh
./compilador tests/validos/39_hello_world.java --cs
```

Ou informe o caminho de saída:

```sh
./compilador tests/validos/39_hello_world.java /tmp/HelloWorld.cs
```

A geração cobre apenas parte da linguagem e não executa o programa. Consulte
[Escopo da Linguagem](./escopo.md) para detalhes do que é e não é traduzido.

## Testar de forma automatizada

Execute a partir da pasta `Projeto-Compilador`:

```sh
./run_tests.sh
```

O script compila o executável e roda os casos `.txt` organizados em
`tests/validos/`, `tests/erros_sintaticos_lexicos/` e
`tests/erros_semanticos/`. Ele valida o código de saída e, quando existem
arquivos `.ast` ou `.err` com o mesmo nome-base, também confere as linhas
esperadas. O placar final informa quantos casos passaram e o script retorna
código diferente de zero se houver falhas.

Para executar também os testes unitários da AST, tabela de símbolos e codegen,
use:

```sh
make test
```

### O que os testes verificam

- Arquivos em `erros_sintaticos_lexicos/` e `erros_semanticos/` devem ser
  rejeitados, com código de saída diferente de zero.
- Arquivos em `validos/` devem ser aceitos, com código de saída zero.
- Os testes unitários verificam criação, composição, liberação e impressão da
  AST, além de inserção, busca, rejeição de nomes duplicados e impressão da
  tabela de símbolos.
- O script exibe `[OK]` ou `[FAIL]` para cada caso e, ao final, o total de
  resultados corretos.

Na revisão desta documentação, existem 44 casos de integração e 21 casos
unitários. A meta de 150 testes indicada no planejamento ainda não foi
alcançada. Os arquivos `.java` são entradas do compilador deste projeto; os
casos com erro sintático não são necessariamente programas válidos para `javac`.

A suíte compara os códigos de saída e as linhas esperadas nos arquivos `.ast`
ou `.err`, quando presentes. Confira o resumo e procure por `[FAIL]`.
