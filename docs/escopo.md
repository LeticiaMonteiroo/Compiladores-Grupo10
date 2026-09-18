# Escopo da Linguagem (Java → C#)

> Este documento lista o que o compilador reconhece e traduz nesta entrega.
> Decisões sobre *por que* reduzimos o escopo estão em
> [Decisões de Projeto](decisoes.md).

| Área                     | Implementado                                                        |
| ------------------------ | --------------------------------------------------------------------|
| Tipos primitivos         | `int`, `double`, `float`, `boolean`, `char`, `long`, `String`, `void`|
| Modificadores            | `public`, `private`, `protected`, `static`                          |
| Métodos                  | Declaração com modificadores, tipo de retorno, parâmetros e corpo   |
| Parâmetros               | Lista de parâmetros tipados, separados por vírgula (`tipo nome`)    |
| Retorno                  | `return expr;` e `return;`                                          |
| Operadores aritméticos   | `+ - * / %` [preencher com o que o colega implementou]              |
| Operadores relacionais   | `== != > < >= <=` [preencher]                                       |
| Operadores lógicos       | `&& \|\| !` [preencher]                                             |
| Operadores de atribuição | `= += -= *= /= %=` [preencher]                                      |
| Incremento/decremento    | `++ --` [preencher]                                                 |

## Fora do escopo desta entrega

> Liste aqui o que **não** está implementado ainda (ex: `if/else`, laços,
> classes, arrays) — deixa claro pra quem revisa que foi uma escolha
> consciente, não esquecimento. Isso também facilita a vida de vocês na P1.

## Exemplo de código aceito

\`\`\`java
public static int soma(int a, int b) {
    return a + b;
}
\`\`\`