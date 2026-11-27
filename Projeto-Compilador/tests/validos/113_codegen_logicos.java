class CondicoesLogicas {
    public static void main() {
        boolean ativo = true;
        boolean pronto = false;
        while (ativo && !pronto || pronto) {
            pronto = true;
        }
    }
}
