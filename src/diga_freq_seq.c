// Lucas Lima Romero (13676325)
// Luciano Gonçalves Lopes Filho (13676520)
// Marco Antonio Gaspar Garcia (11833581)
// Rauany Martinez Secci (13721217)

/* 

Compilar:
    gcc diga_freq.c -o diga_freq -fopenmp

Executar:
    ./diga_freq

Executar com arquivo de entrada:
    ./diga_freq < entrada.txt

*/

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define TAM 1002

// Estrutura que armazena o par: DADO e sua FREQUENCIA correspondente (numero de vezes que apareceu)
typedef struct{
    int ascii;
    int frequencia;
} CharFreq;

// Usamos int nas duas, e não char e unsigned int por exemplo, para facilitar a utilização em "compare"
int compare(const void *primeiro, const void *segundo){
    // Fazemos o cast daquilo que foi enviado pelo compare
    CharFreq *p = (CharFreq *) primeiro;
    CharFreq *s = (CharFreq *) segundo;

    // Primeiros ordenamos pela frequencia (do menor pro maior)
    return p->frequencia != s->frequencia ? p->frequencia - s->frequencia : p->ascii - s->ascii;
    
    // Se forem diferentes, a frequencia basta para diferencia-los. Se nao (se forem iguais) retornamos a diferenca entre os ascii (que sempre existira, por obviedade!)
}

int main(){
    char texto[TAM]; // Entrada do usuario: max 1000 caracteres + '\0'
    int first = 1;

    double wtime = omp_get_wtime();

    while(fgets(texto, TAM, stdin)){
        CharFreq charFreq[96]; // Criamos um vetor de tuplas, em que cada par é dado pelo char e sua frequencia correspondente. 96 posições referentes as possibilidades de codigo ascii das especificacoes (32 a 128)
        int i; 
        char *charAtual; // Variavel auxiliar que nos ajudara a percorrer nossa string 

        // Inicializando nossa struct que armazena os dados (char + freq)
        for(i = 0; i < 96; i++){
            charFreq[i].ascii = 32 + i; // Começa no 32 e vai até o código 32 + 95 = 127
            charFreq[i].frequencia = 0; // Inicializamos as frequências todas com 0
        }

        // Contabilizando a frequencia de cada simbolo ascii
        for(charAtual = texto; *charAtual != '\n'; charAtual++){
            ++charFreq[*charAtual - 32].frequencia; 
        } // Codigo altamente elegante que pega o ascii correspondente a cada char da string e vai na posicao associada a esse ascii (transladada de 32, ou seja, na posicao 0 temos o ascii 32, que é o ' ' espaco) e incrementa a frequencia (ou seja, apareceu +1 vez)
        
        // Vamos utilizar a funcao qsort, padrao da stdlib para ordenar nosso vetor de maneira inteligente
        qsort(charFreq, 96, sizeof(CharFreq), compare); // Vetor, num de elementos, tamanho do elemento, funcao de comparacao que define a ordem dos elementos
        // Essa func de comparacao retorna negativo se o primeiro elemento for menor que o segundo, zeor se forem iguais e positivo se o primeiro for maior que o segundo

        if(!first)
            putchar('\n');

        // Agora basta printar o resultado, para todos aqueles valores que de fato foram digitados
        for(i = 0; i < 96; i++){
            if(charFreq[i].frequencia > 0) // Ou seja, se foi digitado pelo menos uma vez, printamos
                printf("%d %d\n", charFreq[i].ascii, charFreq[i].frequencia);
        }

        first = 0;
    }

    wtime = omp_get_wtime() - wtime;
    // printf("Tempo total: %lf segundos\n", wtime);

    return 0;
}