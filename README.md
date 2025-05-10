# 📊 Trabalho 1 - SSC0903: Diga-me a frequência (CAD - 2025/1)

Este projeto implementa uma solução paralela em C utilizando OpenMP para o problema de contagem de frequência de caracteres em textos, baseado no problema 1251 do URI Online Judge. O programa lê várias linhas de texto da entrada padrão (stdin) e imprime, para cada linha, a frequência de cada caractere em ordem crescente de frequência (e, em caso de empate, por valor ASCII). A entrada é fornecida via redirecionamento de fluxo e a saída é impressa na saída padrão (stdout). 

Projeto desenvolvido para a disciplina **SSC0903 - Computação de Alto Desempenho (CAD - 2025/1)** 

## ✍️ Autores
| [<img loading="lazy" src="https://avatars.githubusercontent.com/u/101420277?v=4" width=115><br><sub>Lucas Romero</sub>](https://github.com/lucaslimaromero) | [<img loading="lazy" src="https://avatars.githubusercontent.com/u/130308566?v=4" width=115><br><sub>Luciano Lopes</sub>](https://github.com/lopesluciano) | [<img loading="lazy" src="https://avatars.githubusercontent.com/u/105023846?v=4" width=115><br><sub>Marco Garcia</sub>](https://github.com/marcogarcia2) | [<img loading="lazy" src="https://avatars.githubusercontent.com/u/113638177?v=4" width=115><br><sub>Rauany Secci</sub>](https://github.com/RauanySecci) |
| :---: | :---: | :---: | :---: |

## 🧩 Descrição

Este projeto implementa um analisador de frequência de caracteres em linhas de texto, com versões **paralela** (OpenMP) e **sequencial**.  
O objetivo é comparar desempenho, explorar técnicas de paralelismo e boas práticas de programação concorrente.

## 🚀 Como Compilar

### Paralelo (OpenMP)

```sh
gcc src/diga_freq.c -o diga_freq -fopenmp
```

### Sequencial

```sh
gcc naive/diga_freq_seq.c -o diga_freq_seq
```

## 🏃 Como Executar

### Usando entrada padrão (digite linhas e pressione Ctrl+D para finalizar):

```sh
./diga_freq
```

### Usando arquivo de entrada:

```sh
./diga_freq < entrada.txt
```
## 📝 Formato da Entrada

- Cada linha será processada individualmente.
- Apenas caracteres ASCII de 32 a 127 são considerados.
- Linhas vazias são ignoradas.

## 📊 Comparação de Desempenho: Sequencial vs Paralelo

Para a entrada `Hamlet.txt` fizemos algumas execuções usando os dois algoritmos e compilamos os resultados na tabela a seguir:

| Execução | Tempo Sequencial (s) | Tempo Paralelo (s) | Aceleração (Speedup) |
|----------|-----------------------|---------------------|-----------------------|
| 1        | 0.041859              | 0.004484            | 9,34×                 |
| 2        | 0.055975              | 0.004133            | 13,55×                |
| 3        | 0.043052              | 0.003968            | 10,85×                |
| 4        | 0.043527              | 0.004434            | 9,82×                 |
| 5        | 0.040874              | 0.004096            | 9,98×                 |

**Tempo médio (sequencial):** `0.045057 s`  
**Tempo médio (paralelo):** `0.004223 s`  
**Speedup médio:** **~10,66×**

## 📚 Referências

- [OpenMP Documentation](https://www.openmp.org/)
- [Manual do GCC](https://gcc.gnu.org/onlinedocs/)
- [Documentação da linguagem C](https://en.cppreference.com/w/c)

---

## Observações Finais

- O código foi escrito para ser didático e eficiente, com comentários explicativos.
- Sinta-se à vontade para sugerir melhorias ou relatar problemas via issues ou pull requests!

---

<div align="center">
  <b>SSC0903 - Computação de Alto Desempenho</b> <br>
  <i>ICMC/USP São Carlos</i>
</div>