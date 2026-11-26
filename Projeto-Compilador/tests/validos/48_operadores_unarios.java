class OperadoresUnarios {
    public static void main() {
        int valor = -2;
        int outro = +valor;
        valor++;
        ++valor;
        valor--;
        --valor;
        while (valor++ < 10 && ++outro >= 0) {
            outro = --valor;
        }
    }
}
