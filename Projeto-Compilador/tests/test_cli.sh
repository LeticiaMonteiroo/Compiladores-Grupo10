#!/usr/bin/env bash
# Testes da interface de linha de comando (pipeline do main.c):
# quando o arquivo C# é gerado, quando NÃO é, e quais códigos de saída são usados.

set -uo pipefail

PROJECT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
COMPILADOR="$PROJECT_DIR/compilador"
VALIDOS="$PROJECT_DIR/tests/validos"

if [[ ! -x "$COMPILADOR" ]]; then
    echo "[FAIL] compilador não encontrado: execute 'make' primeiro." >&2
    exit 1
fi

trabalho="$(mktemp -d)"
trap 'rm -rf "$trabalho"' EXIT
cd "$trabalho" || exit 1

total=0
aprovados=0

# verifica "descrição" comando...   (o comando precisa retornar 0 para passar)
verifica() {
    local descricao="$1"
    shift
    total=$((total + 1))
    if "$@"; then
        aprovados=$((aprovados + 1))
        printf '[PASS] cli: %s\n' "$descricao"
    else
        printf '[FAIL] cli: %s\n' "$descricao"
    fi
}

# executa e guarda codigo de saida / stdout / stderr
executa() {
    codigo=0
    "$COMPILADOR" "$@" >"$trabalho/stdout.txt" 2>"$trabalho/stderr.txt" || codigo=$?
}

arquivo_inexistente() { [[ ! -e "$1" ]]; }
arquivo_nao_vazio()   { [[ -s "$1" ]]; }

# ---------- entradas ----------
cat > "$trabalho/Valido.java" <<'EOF'
public class Valido {
    public static void main(String[] args) {
        int x = 1;
        System.out.println(x);
    }
}
EOF
cat > "$trabalho/ErroSintaxe.java" <<'EOF'
public class ErroSintaxe {
    public static void main(String[] args) {
        int x = ;
    }
}
EOF
cat > "$trabalho/ErroSemantico.java" <<'EOF'
public class ErroSemantico {
    public static void main(String[] args) {
        x = 1;
    }
}
EOF
cat > "$trabalho/ErroLexico.java" <<'EOF'
public class ErroLexico {
    public static void main(String[] args) {
        int x = 1 @;
    }
}
EOF
cat > "$trabalho/lock.java" <<'EOF'
public class lock {
    public static void main(String[] args) {
    }
}
EOF

# ---------- geração bem-sucedida ----------
executa Valido.java --cs
verifica "--cs sai com código 0" test "$codigo" -eq 0
verifica "--cs cria <NomeDaClasse>.cs no diretório atual" arquivo_nao_vazio Valido.cs
verifica "--cs informa o arquivo gerado" grep -q "Arquivo C# gerado com sucesso: Valido.cs" stdout.txt
verifica "arquivo gerado começa com 'using System;'" test "$(head -n 1 Valido.cs)" = "using System;"
verifica "arquivo gerado termina com '}' e quebra de linha" test "$(tail -n 1 Valido.cs)" = "}"
verifica "arquivo gerado não tem linhas com CRLF" bash -c "! grep -q $'\\r' Valido.cs"

executa Valido.java saida_explicita.cs
verifica "caminho explícito sai com código 0" test "$codigo" -eq 0
verifica "caminho explícito cria o arquivo pedido" arquivo_nao_vazio saida_explicita.cs
verifica "caminho explícito gera o mesmo conteúdo que --cs" cmp -s Valido.cs saida_explicita.cs

mkdir -p subdir
executa Valido.java subdir/dentro.cs
verifica "caminho em subdiretório existente funciona" arquivo_nao_vazio subdir/dentro.cs

executa Valido.java saida_explicita.cs
verifica "gerar de novo é determinístico (mesmo conteúdo)" cmp -s Valido.cs saida_explicita.cs

echo "conteudo antigo" > velho.cs
executa Valido.java velho.cs
verifica "arquivo de saída existente é sobrescrito em caso de sucesso" cmp -s Valido.cs velho.cs

executa lock.java --cs
verifica "classe com nome reservado do C# gera lock.cs com @lock" grep -q "^class @lock$" lock.cs

# ---------- sem geração ----------
rm -f ./*.cs
executa Valido.java
verifica "sem segundo argumento: código 0" test "$codigo" -eq 0
verifica "sem segundo argumento: nenhum .cs é criado" bash -c "! ls ./*.cs >/dev/null 2>&1"
verifica "sem segundo argumento: imprime a AST" grep -q "Arvore sintatica abstrata" stdout.txt
verifica "sem segundo argumento: imprime a tabela de símbolos" grep -q "Tabela de simbolos" stdout.txt

executa < "$trabalho/Valido.java"
verifica "entrada por stdin válida: código 0" test "$codigo" -eq 0
verifica "entrada por stdin: nenhum .cs é criado" bash -c "! ls ./*.cs >/dev/null 2>&1"

"$COMPILADOR" < "$trabalho/ErroSintaxe.java" >/dev/null 2>&1
verifica "entrada por stdin inválida: código 1" test "$?" -eq 1

# ---------- erros: nada é gerado, código 1 ----------
for caso in ErroSintaxe ErroSemantico ErroLexico; do
    rm -f "$caso.cs" ./*.cs
    executa "$caso.java" "$caso.cs"
    verifica "$caso: código de saída 1" test "$codigo" -eq 1
    verifica "$caso: nenhum arquivo C# gerado" arquivo_inexistente "$caso.cs"
    verifica "$caso: mensagem de compilação abortada em stderr" grep -q "Compilacao abortada" stderr.txt
    verifica "$caso: não anuncia geração de C#" bash -c "! grep -q 'gerado com sucesso' stdout.txt"

    rm -f ./*.cs
    executa "$caso.java" --cs
    verifica "$caso com --cs: nenhum .cs criado" bash -c "! ls ./*.cs >/dev/null 2>&1"
done

echo "conteudo que deve ser preservado" > preservado.cs
executa ErroSintaxe.java preservado.cs
verifica "erro não apaga nem altera arquivo de saída preexistente" \
    test "$(cat preservado.cs)" = "conteudo que deve ser preservado"

executa ErroSemantico.java
verifica "erro semântico mostra a mensagem em stderr" grep -q "Erro semantico" stderr.txt
executa ErroLexico.java
verifica "erro léxico mostra linha e caractere em stderr" grep -q "Erro lexico (linha 3): caractere invalido '@'" stderr.txt

# ---------- falhas de E/S ----------
executa nao_existe.java
verifica "arquivo de entrada inexistente: código 1" test "$codigo" -eq 1
verifica "arquivo de entrada inexistente: mensagem em stderr" test -s stderr.txt

executa Valido.java diretorio_inexistente/saida.cs
verifica "destino em diretório inexistente: código 1" test "$codigo" -eq 1
verifica "destino em diretório inexistente: mensagem em stderr" test -s stderr.txt
verifica "destino em diretório inexistente: não anuncia sucesso" bash -c "! grep -q 'gerado com sucesso' stdout.txt"

if [[ -w /dev/full ]]; then
    executa Valido.java /dev/full
    verifica "falha de escrita (disco cheio): código 1" test "$codigo" -eq 1
    verifica "falha de escrita (disco cheio): não anuncia sucesso" bash -c "! grep -q 'gerado com sucesso' stdout.txt"
fi

printf '\nResultado cli: %d/%d passaram; %d falharam.\n' "$aprovados" "$total" "$((total - aprovados))"

[[ "$aprovados" -eq "$total" ]]
