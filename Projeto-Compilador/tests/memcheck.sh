#!/usr/bin/env bash
# Roda o compilador sob o valgrind em todas as entradas de teste (gerando C#
# quando a entrada é válida) e falha se houver vazamento ou acesso inválido.

set -uo pipefail

PROJECT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$PROJECT_DIR" || exit 1

if ! command -v valgrind >/dev/null 2>&1; then
    echo "[AVISO] valgrind não encontrado: verificação de memória pulada."
    exit 0
fi

if ! make compilador >/dev/null; then
    echo "[FAIL] Não foi possível compilar o compilador." >&2
    exit 1
fi

saida_cs="$(mktemp --suffix=.cs)"
log="$(mktemp)"
trap 'rm -f "$saida_cs" "$log"' EXIT

total=0
problemas=0

for arquivo in tests/validos/*.txt tests/validos/*.java \
               tests/erros_sintaticos_lexicos/*.txt tests/erros_sintaticos_lexicos/*.java \
               tests/erros_semanticos/*.txt tests/erros_semanticos/*.java; do
    [[ -f "$arquivo" ]] || continue
    total=$((total + 1))

    valgrind --leak-check=full --show-leak-kinds=definite,indirect \
             --errors-for-leak-kinds=definite,indirect --error-exitcode=99 -q \
             ./compilador "$arquivo" "$saida_cs" >/dev/null 2>"$log"
    codigo=$?

    if grep -qE "definitely lost|indirectly lost|Invalid (read|write|free)|uninitialised|Mismatched" "$log" \
       || [[ "$codigo" -eq 99 ]]; then
        problemas=$((problemas + 1))
        printf '[FAIL] memcheck: %s\n' "$arquivo"
        sed 's/^/       /' "$log" | head -12
    else
        printf '[PASS] memcheck: %s\n' "$arquivo"
    fi
done

printf '\nResultado memcheck: %d/%d sem vazamentos nem erros de memória.\n' \
    "$((total - problemas))" "$total"

[[ "$problemas" -eq 0 ]]
