# Escopo da Linguagem (Java → C#)

> Este documento lista o que o compilador reconhece e traduz nesta entrega.
> Decisões sobre *por que* reduzimos o escopo estão em
> [Decisões de Projeto](../decisoes.md).

| Área                     | Implementado                                                                 |
| ------------------------ | ----------------------------------------------------------------------------|
| Tipos primitivos         | `int`, `double`, `float`, `boolean`, `char`, `long`, `short`, `byte`, `String`, `void` |
| Modificadores de classe  | `public` (opcional); classe única por arquivo                               |
| Modificadores de método  | `public`, `private`, `protected`, `static` (nesta ordem: acesso antes de `static`) |
| Métodos                  | Declaração com modificadores, tipo de retorno e nome                        |
| Parâmetros               | **Não implementado.** A gramática só aceita `()` vazio (ver "Fora do escopo") |
| Corpo do método          | **Não implementado.** Só aceita `{}` vazio — nenhuma instrução dentro       |
| Retorno                  | **Não implementado na gramática.** `return` é reconhecido pelo léxico, mas não há produção que o utilize |
| Operadores (léxico)      | Todos reconhecidos pelo scanner: aritméticos (`+ - * / %`), incremento/decremento (`++ --`), relacionais (`== != > < >= <=`), lógicos (`&& \|\| !`), atribuição (`= += -= *= /= %=`), bits (`>> << >>> & \| ^ ~`) |
| Operadores (sintaxe)     | **Não implementados.** Nenhum operador é usado em produções do parser — não há expressões binárias/unárias na gramática atual |
| Expressões               | Apenas literal isolado ou identificador isolado (regra `expressao`); essa regra ainda não é referenciada por nenhuma outra parte da gramática |
| Comentários              | Linha (`//`) e bloco (`/* */`), ignorados pelo léxico                        |

## Fora do escopo da primeira entrega

- **Orientação a objetos**: o projeto não engloba OO — sem herança, interfaces,
  polimorfismo, construtores, atributos de instância ou múltiplas classes por
  arquivo. Uma "classe" aqui é apenas um invólucro sintático para agrupar
  métodos estáticos, no espírito de uma classe utilitária.
- **Parâmetros de método**: a gramática ainda não define uma lista de
  parâmetros; `nome(tipo a, tipo b)` não é aceito, só `nome()`.
- **Corpo de método**: nenhuma instrução é aceita dentro de `{ }` — nem
  declaração de variável, nem atribuição, nem chamada de método, nem `return`.
  O corpo precisa ficar vazio.
- **Expressões compostas**: embora o léxico já reconheça todos os operadores,
  a gramática ainda não tem regras de precedência/associatividade para montar
  expressões binárias ou unárias a partir deles.
- **Estruturas de controle**: `if/else`, `for`, `while`, `do-while`, `switch`.
- **Arrays** e **declaração/uso de variáveis** dentro de métodos.
- **Tipos compostos**: classes definidas pelo usuário (além do wrapper acima),
  enums, generics.

Isso foi uma escolha consciente para fechar a primeira entrega com uma base
sólida de análise léxica/sintática (tipos, modificadores e assinatura de
método), deixando expressões, corpo de método e controle de fluxo para a
próxima etapa — não é esquecimento.

## Exemplo de código aceito

```java
public static int soma() {
}
```

