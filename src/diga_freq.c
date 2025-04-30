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

#define TAM 1001

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

    // Criamos o time de threads que executará cada tarefa
    #pragma omp parallel
    {
        #pragma omp single nowait // Uma única thread irá ser responsável por criar as tarefas: A thread leitora lê linha por linha
        {
            while(fgets(texto, TAM, stdin)){

                char* linha = (char*) malloc(strlen(texto) + 1); // Aloca memória para a cópia (+1 para o '\0')
                if (linha == NULL) {
                    // Trata possiveis erros na alocacao
                    perror("Erro de alocação de memória para linha");
                    // Se a alocação falhar, sair do loop de leitura. Da pra fazer isso tranquilo, pois é só a thread single que está aqui
                    break; 
                }

                // Copiar o conteúdo LIDO PELO fgets (de 'texto') para a NOVA memória alocada.
                strcpy(linha, texto);

                // Este tamanho será usado na Task antes de remover o \n.
                int tamanhoOriginal_para_task = strlen(linha);

                #pragma omp task firstprivate(linha, tamanhoOriginal_para_task)
                {
                    int tam = tamanhoOriginal_para_task; // Isso pode parecer estranho, essa mudanca de tam, mas se voce faz dentro da task, da problema, uma vez que 

                    // Preparacao da string
                    if (tam > 0 && linha[tam - 1] == '\n') { // Se há algo escrito, e na última posição tem a quebra de linha, trocamos ela pelo final de string '\0'
                        linha[tam - 1] = '\0';
                        tam--;
                    } 
            
                    // Se a linha ficou vazia após remover o \n, pular processamento.
                    if (tam == 0){
                        #pragma omp critical // Devemos garantir que apenas uma thread leia isso, já que o putchar deve ser executado apenas uma vez
                        {
                            if(!first)
                                putchar('\n');  
                            first = 0;
                        }
                        free(linha);
                    } else { // Se o tamanho não for 0, temos muito trabalho a ser feito
                        
                        // Criamos um vetor de tuplas, em que cada par é dado pelo char e sua frequencia correspondente. 96 posições referentes as possibilidades de codigo ascii das especificacoes (32 a 128)
                        CharFreq charFreq[96];
                        int i; 
                        
                        // Inicializando nossa struct que armazena os dados (char + freq)
                        for(i = 0; i < 96; i++){
                            charFreq[i].ascii = 32 + i; // Começa no 32 e vai até o código 32 + 95 = 127
                            charFreq[i].frequencia = 0; // Inicializamos as frequências todas com 0
                        }
                
                        #pragma omp parallel for shared(charFreq, linha, tam) schedule(static)       
                        for(i = 0; i < tam; i++){
                            int posicaoCorrigida = linha[i] - 32;
                            #pragma omp atomic
                            ++charFreq[posicaoCorrigida].frequencia;
                        }
                            
                        // Vamos utilizar a funcao qsort
                        qsort(charFreq, 96, sizeof(CharFreq), compare); 

                        #pragma omp critical
                        {
                            if(!first)
                                putchar('\n');
        
                            // Agora basta printar o resultado, para todos aqueles valores que de fato foram digitados
                            for(i = 0; i < 96; i++){
                                if(charFreq[i].frequencia > 0) // Ou seja, se foi digitado pelo menos uma vez, printamos
                                    printf("%d %d\n", charFreq[i].ascii, charFreq[i].frequencia);
                            } 
                            first = 0;
                        }

                        free(linha);
                    }
            
                }
            
            }   

        }
        #pragma omp taskwait
    
    }

    return 0;
}