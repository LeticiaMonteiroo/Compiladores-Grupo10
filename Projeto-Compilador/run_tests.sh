#!/usr/bin/env bash

set -uo pipefail

PROJECT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
cd "$PROJECT_DIR"

if ! make LDLIBS= compilador; then
    echo "[FAIL] Não foi possível compilar o compilador." >&2
    exit 1
fi

saida="$(mktemp)"
trap 'rm -f "$saida"' EXIT

total=0
aprovados=0
reprovados=0
arquivos=(
    tests/validos/*.txt
    tests/validos/*.java
    tests/erros_sintaticos_lexicos/*.txt
    tests/erros_sintaticos_lexicos/*.java
    tests/erros_semanticos/*.txt
    tests/erros_semanticos/*.java
)

for arquivo in "${arquivos[@]}"; do
    [[ -f "$arquivo" ]] || continue
    total=$((total + 1))

    case "$arquivo" in
        tests/validos/*)
            esperado="sucesso"
            ;;
        tests/erros_sintaticos_lexicos/*|tests/erros_semanticos/*)
            esperado="erro"
            ;;
    esac

    if ./compilador "$arquivo" >"$saida" 2>&1; then
        codigo=0
    else
        codigo=$?
    fi

    passou=1
    if [[ "$esperado" == "sucesso" && "$codigo" -ne 0 ]]; then
        passou=0
        resultado="esperava sucesso, compilador retornou $codigo"
    elif [[ "$esperado" == "erro" && "$codigo" -eq 0 ]]; then
        passou=0
        resultado="esperava erro, compilador aceitou a entrada"
    else
        resultado="código de saída correto"
    fi

    base_arquivo="${arquivo%.*}"
    esperado_ast="${base_arquivo}.ast"
    if [[ "$passou" -eq 1 && -f "$esperado_ast" ]]; then
        while IFS= read -r linha || [[ -n "$linha" ]]; do
            if ! grep -Fqx -- "$linha" "$saida"; then
                passou=0
                resultado="linha de AST ausente: '$linha'"
                break
            fi
        done < "$esperado_ast"
    fi

    esperado_erro="${base_arquivo}.err"
    if [[ "$passou" -eq 1 && -f "$esperado_erro" ]]; then
        while IFS= read -r linha || [[ -n "$linha" ]]; do
            if ! grep -Fqx -- "$linha" "$saida"; then
                passou=0
                resultado="mensagem de erro ausente: '$linha'"
                break
            fi
        done < "$esperado_erro"
    fi

    if [[ "$passou" -eq 1 ]]; then
        aprovados=$((aprovados + 1))
        printf '[PASS] %s - %s\n' "$arquivo" "$resultado"
    else
        reprovados=$((reprovados + 1))
        printf '[FAIL] %s - %s\n' "$arquivo" "$resultado"
        sed 's/^/       /' "$saida"
    fi
done

if [[ "$total" -eq 0 ]]; then
    echo "[FAIL] Nenhum teste .txt foi encontrado."
    exit 1
fi

printf '\nResultado: %d/%d passaram; %d falharam.\n' \
    "$aprovados" "$total" "$reprovados"

[[ "$reprovados" -eq 0 ]]
