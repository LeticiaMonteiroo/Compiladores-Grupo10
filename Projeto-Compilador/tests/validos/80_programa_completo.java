public class Demo {
    public static void main(String[] args) {
        int n = 10;
        int a = 0;
        int b = 1;
        int i = 0;
        System.out.println("Fibonacci:");
        while (i < n) {
            System.out.printf("%d ", a);
            int proximo = a + b;
            a = b;
            b = proximo;
            i++;
        }
        System.out.println();
        int fat = 1;
        int k = 1;
        while (k <= 5) {
            fat *= k;
            k++;
        }
        System.out.println("5! = " + fat);
        int soma = 0;
        int j = 1;
        while (j <= 100) {
            soma += j;
            j++;
        }
        System.out.printf("soma 1..100 = %d%n", soma);
        boolean primo = true;
        int cand = 29;
        int d = 2;
        while (d * d <= cand) {
            primo = primo && cand % d != 0;
            d++;
        }
        System.out.println("29 primo? " + primo);
    }
}
