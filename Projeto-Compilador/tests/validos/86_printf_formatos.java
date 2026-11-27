public class PrintfFormatos {
    public static void main(String[] args) {
        int idade = 20;
        double nota = 9.5;
        String nome = "Ana";
        System.out.printf("Nome: %s, idade: %d%n", nome, idade);
        System.out.printf("Nota: %.2f | %5d | %-5d|%n", nota, idade, idade);
        System.out.printf("100%% concluido");
        System.out.printf("Chaves {x} = %d", idade);
    }
}
