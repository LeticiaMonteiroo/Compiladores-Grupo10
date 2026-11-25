# Compilador Java para C# - Grupo 10
Este projeto implementa um compilador focado na tradução de um subconjunto da linguagem Java para C#, desenvolvido com as ferramentas Flex e Bison. O projeto segue uma arquitetura de desenvolvimento incremental baseada em fases semanais de análise léxica, sintática e semântica. 


## Membros


<div align="center"><table>
  <tr>
    <td align="center"><a href="https://github.com/pyramidsf"><img src="https://avatars.githubusercontent.com/u/175058013?s=400&u=3f6ba09512fe946dfcfa89737ab3b28e3cabf4cc&v=4" width="100px" style="border-radius: 50%;"><br/>Maria Eduarda Galdino <br> 231026483<br/></a></td>
    <td align="center"><a href="https://github.com/igorvdaniel"><img src="https://avatars.githubusercontent.com/u/136092030?v=4" width="100px" style="border-radius: 50%;"><br/>Ígor Veras<br> 231026385 <br/></a></td>
    <td align="center"><a href="https://github.com/GuilhermeCarvalho2024"><img src="https://avatars.githubusercontent.com/u/193830842?v=4" width="100px" style="border-radius: 50%;"><br/>Guilherme Carvalho <br>241011822<br/></a></td>
    <td align="center"><a href="https://github.com/LeticiaMonteiroo"><img src="https://avatars.githubusercontent.com/u/152661076?v=4" width="100px" style="border-radius: 50%;"><br/>Letícia Monteiro<br>231026859<br/></a></td>
    <td align="center"><a href="https://github.com/MLuana725"><img src="https://avatars.githubusercontent.com/u/186882957?v=4" width="100px" style="border-radius: 50%;"><br/>Maria Luana<br>241011448<br/></a></td>
  </tr>
</table></div>

## Como Compilar e Executar

### 1. Compilando o Compilador

Acesse o diretório do projeto e utilize o `Makefile` para gerar o executável `compilador`[cite: 5]:

```bash
cd Projeto-Compilador
make
```

*(Caso precise limpar os binários e arquivos gerados anteriormente pelo Flex/Bison, execute `make clean && make`)[cite: 5].*

---

### 2. Executando o Compilador

O compilador pode ser utilizado tanto apenas para validação (léxica, sintática e semântica) quanto para traduzir código Java para C#[cite: 2, 4]:

* **Apenas validar um arquivo Java (exibe a AST e a Tabela de Símbolos)[cite: 4]:**
  ```bash
  ./compilador caminho/do/Arquivo.java
  ```

* **Gerar o arquivo `.cs` automaticamente (com o mesmo nome da classe Java):**
  ```bash
  ./compilador caminho/do/Arquivo.java --cs
  ```

* **Gerar o arquivo `.cs` especificando o nome/caminho de saída:**
  ```bash
  ./compilador caminho/do/Arquivo.java caminho/de/saida.cs
  ```

* **Entrada via terminal (`stdin`)[cite: 4]:**
  ```bash
  ./compilador < caminho/do/Arquivo.java
  ```

---

### 3. Executando a Suíte de Testes

Para rodar todos os testes automatizados (unitários e de integração)[cite: 5]:

```bash
make test
# ou
bash run_tests.sh
```
