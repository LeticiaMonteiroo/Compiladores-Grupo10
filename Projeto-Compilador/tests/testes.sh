COMPILADOR="../compilador"
TOTAL=0
OK=0

for arquivo in *.txt; do
    TOTAL=$((TOTAL+1))
    "$COMPILADOR" < "$arquivo" > /tmp/saida.log 2>&1
    codigo=$?

    if [[ "$arquivo" == *ERRO* ]]; then
        esperado="falhar"
        [[ $codigo -ne 0 ]] && resultado="falhou (correto)" && passou=1 || { resultado="passou (ERRADO, deveria falhar)"; passou=0; }
    else
        esperado="passar"
        [[ $codigo -eq 0 ]] && resultado="passou (correto)" && passou=1 || { resultado="falhou (ERRADO, deveria passar)"; passou=0; }
    fi

    if [[ $passou -eq 1 ]]; then
        OK=$((OK+1))
        echo "[OK]   $arquivo -> $resultado"
    else
        echo "[FAIL] $arquivo -> $resultado"
        sed 's/^/       /' /tmp/saida.log
    fi
done

echo
echo "Resultado: $OK/$TOTAL casos de teste corretos."
