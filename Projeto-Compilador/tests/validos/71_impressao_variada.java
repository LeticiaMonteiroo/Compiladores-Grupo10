public class ImpressaoVariada {
    public static void main(String[] args) {
        int n = 3;
        String nome = "Ana";
        System.out.print("a");
        System.out.print(1);
        System.out.print('c');
        System.out.println();
        System.out.println("linha");
        System.out.println(n * 2);
        System.out.println("n=" + n);
        System.out.println("nome=" + nome + " n=" + n);
        System.out.printf("%d-%d%n", n, n + 1);
        printf("%s tem %d anos%n", nome, 20);
        System.out.printf("%5d|%-5d|%n", n, n);
        System.out.printf("%.2f%n", 3.14159);
        System.out.printf("%x %X%n", 255, 255);
        System.out.printf("100%%%n");
        System.out.printf("{chaves}%n");
        System.out.printf("{%d}%n", n);
        System.out.printf("%c%c%n", 'o', 'k');
        System.out.printf("%b %b%n", true, 1 > 2);
    }
}
