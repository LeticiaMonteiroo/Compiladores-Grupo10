class PrecedenciaLogica {
    public static void main() {
        int valor = 1;
        boolean ativo = true;
        boolean pronto = false;
        while (valor < 10 && !pronto || ativo && valor != 0) {
            valor++;
        }
    }
}
