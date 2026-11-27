# Escopo da Linguagem (Java → C#)

> Este documento descreve o subconjunto que o compilador reconhece atualmente.
> O projeto gera C# para parte das construções suportadas, mas ainda não
> implementa uma tradução completa de Java. As decisões sobre o escopo
> estão em [Decisões de Projeto](../decisoes.md).

| Área | Implementado |
| ---- | ------------ |
| Tipos | `int`, `double`, `float`, `boolean`, `char`, `long`, `short`, `byte`, `String` e `void` como tipo de retorno |
| Classe | Uma classe por programa; modificador `public` opcional |
| Métodos | Métodos com tipo de retorno e um modificador de acesso (`public`, `private` ou `protected`), com `static` opcional, ou somente `static` |
| Parâmetros | Parâmetros posicionais com tipo e nome, separados por vírgula; representados na AST |
| Declarações locais | Variáveis dos tipos listados, com ou sem inicialização |
| Atribuições | Atribuição simples (`=`) e composta (`+=`, `-=`, `*=`, `/=`, `%=`) |
| Incremento/decremento | Formas prefixas e pós-fixas (`++` e `--`), como comandos ou fatores de expressão |
| Laços | `while` com condição e bloco |
| Expressões | Literais, identificadores, parênteses, operadores aritméticos, relacionais e lógicos |
| Comentários | Comentários de linha (`//`) e de bloco (`/* ... */`), ignorados pelo lexer |
| AST | Árvore sintática abstrata construída durante o parsing e impressa ao final |
| Tabela de símbolos | Pilha de escopos (global, método e bloco do `while`); registra classe, métodos, parâmetros e variáveis; é impressa ao final, por escopo |
| Geração de C# | Saída parcial para declarações locais, atribuições, `while`, operadores e comandos de impressão suportados |

## Fluxo do compilador

1. **Análise léxica — Flex (`Projeto-Compilador/lexer/lexer.l`):** transforma
   caracteres em tokens, reconhecendo palavras-chave, identificadores,
   literais, operadores e delimitadores, e ignora espaços e comentários.
2. **Análise sintática — Bison (`Projeto-Compilador/parser/parser.y`):** valida
   a sequência de tokens segundo a gramática e constrói a AST.
3. **AST e tabela de símbolos (`src/ast.c`, `src/tabela_simbolos.c`):** a AST
   representa classes, métodos, comandos e expressões; a tabela registra
   símbolos nos escopos global, de método e de bloco `while`.
4. **Análise semântica (`src/semantico.c`):** verifica redeclarações,
   visibilidade e ordem de uso de variáveis, compatibilidade de tipos e
   condições booleanas de `while`.
5. **Geração de C# (`src/codegen.c`):** após sucesso das análises, pode emitir
   C# para os nós de AST suportados quando um caminho de saída é fornecido.
   A saída não é executada pelo compilador.

## Mapeamento de tipos e comandos

| Tipo Java reconhecido | Saída C# |
| --------------------- | -------- |
| `boolean` | `bool` |
| `String` | `string` |
| `byte` | `sbyte` |
| `short`, `int`, `long`, `float`, `double`, `char`, `void` | Mantido sem alteração |

`System.out.println` é traduzido para `Console.WriteLine`, `System.out.print`
para `Console.Write` e as formas reconhecidas de `printf` para `Console.Write`.
Um método chamado `main` é emitido como `public static void Main(string[] args)`.
Os demais métodos são emitidos como `public`; os modificadores de acesso
originais não são preservados.

## Exemplo de tradução

Entrada (`HelloWorld.java`):

```java
public class HelloWorld {
    public static void main(String[] args) {
        System.out.println("Hello, World!");
    }
}
```

Comando, executado na pasta `Projeto-Compilador`:

```sh
./compilador tests/validos/39_hello_world.java /tmp/HelloWorld.cs
```

Saída gerada:

```csharp
using System;

class HelloWorld
{
    public static void Main(string[] args)
    {
        Console.WriteLine("Hello, World!");
    }
}
```

## Operadores aceitos na sintaxe

- **Aritméticos binários:** `+`, `-`, `*`, `/`, `%`, com precedência usual
  entre adição/subtração e multiplicação/divisão/resto.
- **Aritméticos unários:** `+` e `-`.
- **Relacionais:** `==`, `!=`, `>`, `<`, `>=`, `<=`.
- **Lógicos:** `&&`, `||` e `!`.
- **Incremento e decremento:** `++` e `--`, prefixos ou pós-fixos.
- **Atribuição:** `=`, `+=`, `-=`, `*=`, `/=`, `%=` como comandos.

O lexer também reconhece operadores bit a bit (`>>`, `<<`, `>>>`, `&`, `|`,
`^`, `~`), mas eles **não são aceitos nas produções da gramática**.

## Limitações atuais

- **Geração de C#:** cobre somente os nós de AST implementados e não garante
  uma tradução completa ou compilável para qualquer programa Java.
- **Chamadas de método:** chamadas não fazem parte da gramática. Parâmetros
  são aceitos nas assinaturas e ficam no escopo do método.
- **Retorno de métodos:** `return` é reconhecido pelo lexer, mas ainda não há
  produção sintática para ele.
- **Outras estruturas de controle:** `if/else`, `for`, `do-while` e `switch`.
- **Expressões e comandos adicionais:** arrays são aceitos somente como tipo de
  parâmetro; declarações e acesso a elementos, acesso a membros, chamadas,
  operadores bit a bit e atribuições dentro de expressões não são suportados.
- **Orientação a objetos completa:** herança, interfaces, polimorfismo,
  construtores, atributos e múltiplas classes por arquivo.
- **Análise semântica:** cobre redeclaração, escopo de variáveis (método e
  bloco do `while`), uso de identificadores e compatibilidade de tipos em
  declarações, atribuições e condições. Variáveis e parâmetros não podem
  sombrear outro nome local do mesmo método. Não há verificação de retorno nem
  de chamadas, pois essas construções não existem na gramática.

## Exemplo de código aceito

```java
public class Exemplo {
    public static void main() {
        int contador = 3;
        while (contador > 0) {
            contador -= 1;
        }
    }
}
```

O parser reconhece o exemplo, constrói a AST e a tabela de símbolos e imprime
ambas. Também é possível solicitar a geração parcial de C# passando um caminho
de saída ao executável; isso não significa que o programa gerado seja executado
ou que todos os recursos de Java estejam traduzidos.
