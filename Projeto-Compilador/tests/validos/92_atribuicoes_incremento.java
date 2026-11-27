public class AtribuicoesIncremento {
    public static void main(String[] args) {
        int x = 10;
        x += 5;
        x -= 3;
        x *= 2;
        x /= 5;
        x %= 3;
        System.out.println(x);
        int y = 5;
        y++;
        ++y;
        y--;
        --y;
        System.out.println(y);
        int z = y++ + ++y;
        System.out.println(z);
        System.out.println(y);
        int w = y-- - --y;
        System.out.println(w);
        System.out.println(y);
    }
}
