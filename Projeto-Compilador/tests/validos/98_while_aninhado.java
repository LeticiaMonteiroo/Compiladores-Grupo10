public class WhileAninhado {
    public static void main(String[] args) {
        int i = 1;
        while (i <= 3) {
            int j = 1;
            while (j <= i) {
                System.out.print(j);
                j++;
            }
            System.out.println();
            i++;
        }
        int soma = 0;
        int k = 0;
        while (k < 5 && soma < 100) {
            soma += k * 2;
            k++;
        }
        System.out.println(soma);
    }
}
