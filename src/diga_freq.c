// Lucas Lima Romero (13676325)
// Luciano Gonçalves Lopes Filho (13676520)
// Marco Antonio Gaspar Garcia (11833581)
// Rauany Martinez Secci (?)

/* 
    Próximos passos: 0. Implementar o merge sort paralelo

    1. Analisar se realmente é necessário o if da linha 85
    2. Reavaliar métodos de paralelização. Exemplo: o atomic é realmente a melhor opção no particionamento dos char's cada string?
    3. Podíamos implementar uma cláusula simd
    
*/

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
#include <string.h>
#include <omp.h>

#define TAM 1000
#define ASCII_RANGE 96

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


// Junta as partes ordenadas do vetor v
void merge(CharFreq *v, int inicio, int meio, int fim) {
    int n1 = meio - inicio + 1;
    int n2 = fim - meio;

    //Criar vetores temporarios para as duas metades L e R.
    CharFreq *L = malloc(n1 * sizeof(CharFreq));
    CharFreq *R = malloc(n2 * sizeof(CharFreq));

    // Copia os elementos do vetor v original para L e R.
    for (int i = 0; i < n1; i++) L[i] = v[inicio + i];
    for (int i = 0; i < n2; i++) R[i] = v[meio + 1 + i];

    // Compara e ordena os elementos de L[i] e R[j]
    int i = 0, j = 0, k = inicio;
    while (i < n1 && j < n2) {
        // se L[i] for menor ou igual copia ele para v[k]
        if (compare(&L[i], &R[j]) <= 0) 
            v[k++] = L[i++];
        // caso contrario, copia R[j] para v[k]
        else
            v[k++] = R[j++];
    }

    // Copia elementos que restarem em R ou L, se um dos vetores for maior que o outro. A ordem continua garantida pois os elementos do vetor restante estao ordenados e devem ser maiores que o maior elemento do vetor menor.
    while (i < n1) v[k++] = L[i++];
    while (j < n2) v[k++] = R[j++];

    free(L);
    free(R);
}

void mergeSortParalelo(CharFreq *v, int inicio, int fim) {
    if (inicio < fim) { // Caso Base, tamanho 1
        int meio = (inicio + fim) / 2; 

        // Criar uma tarefa para ordenar a metade esquerda (recursivamente)
        #pragma omp task shared(v)
        mergeSortParalelo(v, inicio, meio);

        // Criar uma tarefa para ordenar a metade esquerda (recursivamente)
        #pragma omp task shared(v)
        mergeSortParalelo(v, meio + 1, fim);

        #pragma omp taskwait // Espera ambas tarefas acabarem

        merge(v, inicio, meio, fim); // Junta e ordena as metades dos vetores
    }
}

char *processa_linha(const char *linha){
    int frequencias[ASCII_RANGE] = {0};
    int tam = strlen(linha);

    CharFreq charFreq[ASCII_RANGE];
    int contador = 0; // Conta quant

    // #pragma omp parallel
    // {
    //     #pragma omp single
    //     printf("Threads na região paralela: %d\n", omp_get_num_threads());
    // }

    /* A cláusula reduction foi usada para o segundo particionamento (dada uma string, vamos fatiá-la entre as threads)

    - Cada thread mantém uma cópia local do array frequencias (com 96 posições).

    Ex.: 4 threads e tam = 1000, cada thread processará 250 iterações.

    - Testar com schedule(dynamic) e medir tempo (reduziu real quase pela metade)
    */
    // double wtime = omp_get_wtime();
    #pragma omp parallel for reduction(+:frequencias[:96]) /*num_threads(4)*/ schedule(dynamic) 
    for(int i = 0; i < tam; i++){ 
        if (linha[i] >= 32 && linha[i] < 128){
            frequencias[(int)(linha[i] - 32)]++;
        }
        // printf("Número de threads: %d\n", omp_get_num_threads());
        // //Depuração: Mostra qual thread está processando cada iteração
        // printf("Thread %d processando i = %d\n", omp_get_thread_num(), i);
    }
    // wtime = omp_get_wtime() - wtime;
    // printf("Tempo: %f segundos\n", wtime);

    for(int i = 0; i < ASCII_RANGE; i++){
        if(frequencias[i] > 0){
            charFreq[contador].ascii = i + 32;
            charFreq[contador].frequencia = frequencias[i];
            contador++;
        }
    }

    /*
    
    Diferença importante do paralelo para o sequencial

    - Aqui, charFreq[i] não representa mais "o caractere de código ASCII i+32".
    - Agora, ele representa "o i-ésimo caractere com frequência > 0 da string atual".
    
    - Só importa os "contador" primeiros elementos, será neles que aplicaremos o sorting, e não em todos os 96 possíveis elementos

    */ 

    #pragma omp parallel
    {
        #pragma omp single
        mergeSortParalelo(charFreq, 0, contador - 1);
    }

    char *buffer = malloc(sizeof(char) * (contador * 10 + 1));
    char *p = buffer;

    for (int i = 0; i < contador; i++) {
        p += sprintf(p, "%d %d\n", charFreq[i].ascii, charFreq[i].frequencia);
    }

    *p = '\0';
    return buffer;
}


int main()
{
    char texto[TAM]; // Armazena temporariamente uma linha lida da entrada padrão
    int num_linhas = 0; // Conta o número de linhas processadas
    int capacidade = 1000; // Define a capacidade inicial do array que armazenará as todas as linhas lidas


    char **linhas_processadas = malloc(sizeof(char *) * capacidade); // Array que armazenará as saídas processadas de cada linha
    if (!linhas_processadas) { 
        perror("Erro ao alocar memória\n");
        exit(EXIT_FAILURE);
    }

    // Permitiremos que tarefas paralelas criem outras tarefas paralelas
    omp_set_nested(1); // Habilitando o paralelismo aninhado, uma vez que nosso particionamento de dados ocorre em duas etapas: na quebra do arquivo em linhas, e na quebra de linhas em blocos de caracteres
    omp_set_num_threads(omp_get_num_procs());

    #pragma omp parallel
    {
        #pragma omp single // A leitura é feita de forma sequencial por apenas uma thread, a qual delegará as tasks
        {
            while (fgets(texto, TAM, stdin)) // Lê uma linha do arquivo e armazena no buffer texto
            {

                texto[strcspn(texto, "\n")] = '\0'; // Eliminando a quebra de linha
                if (texto[0] == '\0') // Se não leu nada, vamos para a próxima linha
                    continue; 
                

                char *linha = strdup(texto); // Salvamos o texto temporário lido por fgets
                int indice_atual = num_linhas++; // Salva o índice da linha atual e incrementa o número de linhas lido (apenas após salvar)

                #pragma omp task firstprivate(linha, indice_atual)
                {
                    char *saida = processa_linha(linha);
                    free(linha);

                    #pragma omp critical
                    {
                        if (indice_atual >= capacidade)
                        {
                            capacidade *= 2;
                            linhas_processadas = realloc(linhas_processadas, capacidade * sizeof(char *));
                        }
                        // "Independente de quando eu terminei, se eu sou a resposta da linha 5, e vou escrever em linhas_processadas[5]".
                        
                        linhas_processadas[indice_atual] = saida;
                    }

                }
            }
        }
    }

    for (int i = 0; i < num_linhas; i++)
    {
        if(i)
            printf("\n");
        printf("%s", linhas_processadas[i]);
        free(linhas_processadas[i]);
    }

    free(linhas_processadas);
    return 0;
}