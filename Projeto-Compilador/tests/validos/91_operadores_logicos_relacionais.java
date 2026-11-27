public class LogicosRelacionais {
    public static void main(String[] args) {
        int a = 7;
        int b = 3;
        boolean t1 = !(a > b && b > 5);
        boolean t2 = (a > b || b > 5) && a == 7;
        boolean t3 = a > b && (b > 5 || a != 0);
        boolean t4 = !!(a >= 7);
        boolean t5 = (a < b) == false;
        boolean t6 = a < b;
        boolean t7 = !(a > b);
        boolean t8 = a == b || b == 3 && a == 0;
        System.out.println(t1);
        System.out.println(t2);
        System.out.println(t3);
        System.out.println(t4);
        System.out.println(t5);
        System.out.println(t6);
        System.out.println(t7);
        System.out.println(t8);
    }
}
