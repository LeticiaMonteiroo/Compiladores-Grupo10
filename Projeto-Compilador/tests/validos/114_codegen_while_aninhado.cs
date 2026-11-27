using System;

class WhileAninhado
{
    public static void Main(string[] args)
    {
        int volta = 0;
        while (volta < 2)
        {
            int interna = 0;
            while (interna < 2)
            {
                interna++;
            }
            volta++;
        }
    }
}
