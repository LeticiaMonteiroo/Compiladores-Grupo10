#!/usr/bin/env bash

set -uo pipefail

PROJECT_DIR="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
cd "$PROJECT_DIR"

if ! make LDLIBS= compilador; then
    echo "[FAIL] Não foi possível compilar o compilador." >&2
    exit 1
fi

saida="$(mktemp)"
dir_cs="$(mktemp -d)"
trap 'rm -f "$saida"; rm -rf "$dir_cs"' EXIT

csc=""
runtime=""
execucoes_puladas=0
if command -v mcs >/dev/null 2>&1; then
    csc="mcs"
fi
if command -v mono >/dev/null 2>&1; then
    runtime="mono"
fi

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
    base_arquivo="${arquivo%.*}"
    esperado_csharp="${base_arquivo}.cs"
    cs_gerado="$dir_cs/saida.cs"
    rm -f "$cs_gerado"

    case "$arquivo" in
        tests/validos/*)
            esperado="sucesso"
            ;;
        tests/erros_sintaticos_lexicos/*|tests/erros_semanticos/*)
            esperado="erro"
            ;;
    esac

    if [[ "$esperado" == "sucesso" ]]; then
        if ./compilador "$arquivo" "$cs_gerado" >"$saida" 2>&1; then
            codigo=0
        else
            codigo=$?
        fi
    elif ./compilador "$arquivo" >"$saida" 2>&1; then
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

    esperado_ast="${base_arquivo}.ast"

    if [[ "$passou" -eq 1 && -f "$esperado_ast" ]]; then
        while IFS= read -r linha || [[ -n "$linha" ]]; do
            linha="${linha%$'\r'}"

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
            linha="${linha%$'\r'}"

            if ! grep -Fqx -- "$linha" "$saida"; then
                passou=0
                resultado="mensagem de erro ausente: '$linha'"
                break
            fi
        done < "$esperado_erro"
    fi

    if [[ "$passou" -eq 1 && "$esperado" == "sucesso" ]]; then
        if [[ ! -s "$cs_gerado" ]]; then
            passou=0
            resultado="o compilador não gerou o arquivo C#"
        else
            esperado_cs="${base_arquivo}.cs"
            if [[ -f "$esperado_cs" ]]; then
                while IFS= read -r linha || [[ -n "$linha" ]]; do
                    linha="${linha%$'\r'}"

                    if ! grep -Fqx -- "$linha" "$cs_gerado"; then
                        passou=0
                        resultado="linha de C# ausente: '$linha'"
                        break
                    fi
                done < "$esperado_cs"
            fi
            if [[ "$passou" -eq 1 && "$arquivo" == *"_codegen_"* && -f "$esperado_csharp" ]] &&
                ! cmp -s "$esperado_csharp" "$cs_gerado"; then
                passou=0
                resultado="código C# gerado difere do esperado"
            fi
            proibido_cs="${base_arquivo}.cs.proibido"
            if [[ "$passou" -eq 1 && -f "$proibido_cs" ]]; then
                while IFS= read -r linha || [[ -n "$linha" ]]; do
                    linha="${linha%$'\r'}"

                    if grep -Fq -- "$linha" "$cs_gerado"; then
                        passou=0
                        resultado="trecho proibido presente no C#: '$linha'"
                        break
                    fi
                done < "$proibido_cs"
            fi

            if [[ "$passou" -eq 1 && -n "$csc" ]]; then
                if ! "$csc" -target:library -out:"$dir_cs/saida.dll" "$cs_gerado" >"$dir_cs/csc.log" 2>&1; then
                    passou=0
                    resultado="o C# gerado não compila: $(grep -m1 'error' "$dir_cs/csc.log")"
                fi
            fi

            esperado_saida="${base_arquivo}.saida"
            if [[ "$passou" -eq 1 && -f "$esperado_saida" ]]; then
                if [[ -z "$csc" || -z "$runtime" ]]; then
                    execucoes_puladas=$((execucoes_puladas + 1))
                elif ! "$csc" -out:"$dir_cs/saida.exe" "$cs_gerado" >"$dir_cs/csc.log" 2>&1; then
                    passou=0
                    resultado="o C# gerado não vira executável: $(grep -m1 'error' "$dir_cs/csc.log")"
                else
                    LC_ALL=C timeout 20 "$runtime" "$dir_cs/saida.exe" >"$dir_cs/execucao.txt" 2>&1
                    if ! diff -q "$esperado_saida" "$dir_cs/execucao.txt" >/dev/null; then
                        passou=0
                        resultado="saída do programa C# difere do esperado: $(diff "$esperado_saida" "$dir_cs/execucao.txt" | head -4 | tr '\n' ' ')"
                    fi
                fi
            fi
        fi
    elif [[ "$passou" -eq 1 ]]; then
        ./compilador "$arquivo" "$cs_gerado" >/dev/null 2>&1
        if [[ -e "$cs_gerado" ]]; then
            passou=0
            resultado="gerou C# para uma entrada com erro"
        fi
    fi

    if [[ "$passou" -eq 1 ]]; then
        aprovados=$((aprovados + 1))
        printf '[PASS] %s - %s\n' "$arquivo" "$resultado"
    else
        reprovados=$((reprovados + 1))
        printf '[FAIL] %s - %s\n' "$arquivo" "$resultado"
        sed 's/^/       /' "$saida"
        if [[ "$resultado" == "código C# gerado difere do esperado" ]]; then
            diff -u "$esperado_csharp" "$cs_gerado" || true
        fi
    fi
done

if [[ "$total" -eq 0 ]]; then
    echo "[FAIL] Nenhum arquivo de teste .txt ou .java foi encontrado."
    exit 1
fi

if [[ -z "$csc" ]]; then
    echo "[AVISO] mcs não encontrado: a compilação do C# gerado foi pulada."
fi
if [[ "$execucoes_puladas" -gt 0 ]]; then
    echo "[AVISO] $execucoes_puladas execução(ões) de C# puladas (precisa de mcs e mono)."
fi

printf '\nResultado: %d/%d passaram; %d falharam.\n' \
    "$aprovados" "$total" "$reprovados"

[[ "$reprovados" -eq 0 ]]