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

Para compilar e executar os testes de C# gerado, também são necessários
`mcs`, `mono` e o comando GNU `timeout` (fornecido pelo pacote `coreutils`).
Em Debian ou Ubuntu, instale-os com:

```sh
sudo apt install mono-mcs mono-runtime coreutils
```

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

O script compila o executável e roda os casos `.txt` e `.java` organizados em
`tests/validos/`, `tests/erros_sintaticos_lexicos/` e
`tests/erros_semanticos/`. Ele valida o código de saída e, quando existem
arquivos `.ast` ou `.err` com o mesmo nome-base, também confere as linhas
esperadas.

Para um caso válido que tenha um arquivo `.cs` com o mesmo nome-base, o script
também pede ao compilador que gere o C# e compara o arquivo gerado com esse
`.cs` esperado. Esses arquivos `.cs` são referências para conferir a saída do
codegen, não entradas nem casos adicionais. O placar final informa quantos
casos passaram e o script retorna código diferente de zero se houver falhas.

Para executar também os testes unitários da AST, tabela de símbolos e codegen,
use:

```sh
make test
```

### O que os testes verificam

A suíte tem quatro camadas, todas executadas por `make test`:

| Camada | Arquivo | O que verifica |
|---|---|---|
| Unitários | `tests/test_ast.c`, `tests/test_tabela_simbolos.c`, `tests/test_codegen.c` | AST, tabela de símbolos e o gerador de C# (precedência, `printf`, escopo, identificadores, tipos, robustez) |
| Integração | `run_tests.sh` | Cada entrada de `tests/validos/`, `tests/erros_sintaticos_lexicos/` e `tests/erros_semanticos/` |
| Linha de comando | `tests/test_cli.sh` | Quando o `.cs` é (ou não é) gerado, códigos de saída, arquivos preexistentes, falhas de E/S |
| Memória | `tests/memcheck.sh` (`make memcheck`) | Valgrind em todas as entradas: sem vazamentos nem acessos inválidos |

Para cada entrada em `validos/`, o `run_tests.sh` gera o arquivo C# e confere,
quando existirem arquivos com o mesmo nome-base:

| Arquivo | Verificação |
|---|---|
| `.ast` | linhas que devem aparecer na saída do compilador |
| `.cs` | linhas que devem aparecer exatamente no C# gerado; nos casos `*_codegen_*`, o arquivo inteiro também é comparado |
| `.cs.proibido` | trechos que **não** podem aparecer no C# gerado (ex.: comentários) |
| `.saida` | saída esperada do programa C# quando executado (escrita a partir da semântica do Java) |

Com `mcs`, o C# gerado também precisa compilar; com `mono` e `timeout`, os
casos com `.saida` são executados e a saída é comparada byte a byte. Sem
`mcs`/`mono`, as etapas correspondentes são puladas com um `[AVISO]`; se esses
dois estiverem disponíveis mas faltar `timeout`, a execução dos casos `.saida`
falha. Entradas com erro léxico, sintático ou semântico não podem gerar arquivo
C#.

Na revisão desta documentação, a suíte tem 121 casos de integração, 62
verificações unitárias (7 da AST, 11 da tabela de símbolos e 44 do codegen) e
45 verificações de linha de comando. Entre as entradas válidas, 38 têm `.cs`
esperado e 15 têm execução comparada (`.saida`). Os totais podem mudar quando
os casos forem alterados; `make test` e `make memcheck` imprimem seus
resultados. A meta de 150 testes indicada no planejamento não foi
alcançada. Os arquivos `.java` são entradas do compilador deste projeto; os
casos com erro sintático não são necessariamente programas válidos para
`javac`.

### Diferenças conhecidas entre Java e o C# gerado

Estas diferenças de comportamento em tempo de execução ainda não são tratadas:

- `double` impresso por `println`: o Java escreve `1.0`; o C# escreve `1`.
  Tanto `println` quanto `printf` podem usar no C# o separador decimal da
  cultura do sistema (por exemplo, `9,5` em pt-BR), enquanto os arquivos
  `.saida` esperam ponto. Em macOS com Mono e cultura `pt_BR`, isso pode fazer
  falhar `93_tipos_primitivos` e `97_impressao_variada`, mesmo com `LC_ALL=C`
  definido pelo script.
- Modificadores `private`, `protected` e `static` de métodos que não são `main`
  não são preservados (ver `escopo.md`).
