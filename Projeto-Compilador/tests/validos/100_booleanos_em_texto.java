public class BooleanosEmTexto {
    public static void main(String[] args) {
        boolean ativo = true;
        boolean inativo = false;
        int a = 4;
        System.out.println(ativo);
        System.out.println(inativo);
        System.out.println(a > 3);
        System.out.println("ativo=" + ativo);
        System.out.println("cmp=" + (a == 4) + "!");
        System.out.println(ativo && !inativo);
        System.out.printf("%s %s%n", ativo, a < 0);
        System.out.println(!ativo || a >= 4);
    }
}
