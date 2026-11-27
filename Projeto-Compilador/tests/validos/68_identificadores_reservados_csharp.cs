class @lock
    public void @event()
        string @string = "ola";
        int @object = 1;
        int @ref = 2;
        int @params = 3;
        int @in = 4;
        int @is = 5;
        bool @bool = true;
        int _S_dolar = 7;
        int a_S_b = 8;
        int _ok = 9;
        int soma = @object + @ref + @params + @in + @is;
        Console.WriteLine(@string);
        Console.WriteLine(_S_dolar + a_S_b + _ok);
        Console.WriteLine(@bool.ToString().ToLower());
