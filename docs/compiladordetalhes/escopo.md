# Escopo da Linguagem (Java → C#)

> Este documento descreve o subconjunto que o analisador reconhece atualmente.
> O projeto ainda não traduz o programa de Java para C#; ele realiza análise
> léxica e sintática e exibe estruturas internas. As decisões sobre o escopo
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
| Tabela de símbolos | Registra classe, métodos e variáveis; é impressa ao final |

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

- **Tradução para C#:** o programa ainda não gera código C# nem executa o
  programa analisado.
- **Chamadas de método:** chamadas não fazem parte da gramática. Parâmetros
  são aceitos nas assinaturas, mas ainda não têm validação semântica de escopo
  ou uso.
- **Retorno de métodos:** `return` é reconhecido pelo lexer, mas ainda não há
  produção sintática para ele.
- **Outras estruturas de controle:** `if/else`, `for`, `do-while` e `switch`.
- **Expressões e comandos adicionais:** arrays, acesso a membros, chamadas,
  operadores bit a bit e atribuições dentro de expressões.
- **Orientação a objetos completa:** herança, interfaces, polimorfismo,
  construtores, atributos e múltiplas classes por arquivo.
- **Análise semântica:** não há verificação de tipos, escopo de variáveis ou
  uso de identificadores. A tabela atual é uma estrutura de registro, não uma
  validação semântica completa.

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
ambas. Isso não significa que o código seja traduzido ou executado.
