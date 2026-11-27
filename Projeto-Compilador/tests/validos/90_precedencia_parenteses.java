public class PrecedenciaParenteses {
    public static void main(String[] args) {
        int a = 7;
        int b = 3;
        int c = 2;
        System.out.println((a + b) * c);
        System.out.println(a - (b - c));
        System.out.println(a - b - c);
        System.out.println(a / (b * c));
        System.out.println(a * (b * c));
        System.out.println(a % (b + c));
        System.out.println(-(a + b));
        System.out.println(a - -b);
        System.out.println(- -a);
        System.out.println(-(--a));
        System.out.println(a);
        System.out.println("x" + (1 + 2));
        System.out.println("x" + 1 + 2);
        System.out.println("x" + (a - b));
    }
}
