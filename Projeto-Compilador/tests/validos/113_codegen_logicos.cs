using System;

class CondicoesLogicas
{
    public static void Main(string[] args)
    {
        bool ativo = true;
        bool pronto = false;
        while (ativo && !pronto || pronto)
        {
            pronto = true;
        }
    }
}
