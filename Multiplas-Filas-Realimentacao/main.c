#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef struct {
    int data, dur, entrou;
} Processo;

int conta_linhas(){
    FILE *entrada;
    int conta = 1, c;

    //Abertura do arquivo de entrada
    entrada = fopen("entrada.txt", "r");
    if (entrada == NULL){
        printf("Erro ao abrir arquivo.\n");
        return 0;
    }

    //Contagem da quantidade de linhas (processos)
    for (c = getc(entrada); c != EOF; c = getc(entrada))
        if (c == '\n') // Incrementa conta se existe linha nova
            conta = conta + 1;
    printf("Existem %d processos a serem executados.\n\n", conta);

    fclose(entrada);
    return conta;
}

//Implementação funções para ordenação
void troca(Processo *a, Processo *b)
{
    Processo temp = *a;
    *a = *b;
    *b = temp;
}

// A function to implement bubble sort
void bubbleSort(Processo arr[], int n)
{
   int i, j;
   for (i = 0; i < n-1; i++)
       for (j = 0; j < n-i-1; j++)
           if (arr[j].data > arr[j+1].data)
              troca(&arr[j], &arr[j+1]);
}

int main (){
    FILE *entrada;
    int count_proc, x;

    count_proc = conta_linhas();

    entrada = fopen("entrada.txt", "r");
    if (entrada == NULL){
        printf("Erro ao abrir arquivo.\n");
        return 0;
    }

    //Criação lista geral de processos e lista de processos a ser manipulada
    Processo geral[count_proc];
    Processo atual[count_proc];

    int i = 0;
    int data, dur, prio;
    while(fscanf(entrada, "%d %d %d", &data, &dur, &prio) != -1){ //Leitura do arquivo de entrada
        geral[i].data = data;
        geral[i].dur = dur;
        geral[i].entrou = 0;
        i++;
    }

    bubbleSort(geral, count_proc); //Ordena lista em ordem de data criação

    for(i = 0; i < count_proc; i++){
        atual[i].data = geral[i].data;
        atual[i].dur = geral[i].dur;
        atual[i].entrou = 0;
        printf("P%d \t criacao: %d \t duracao: %d\n", i, atual[i].data, atual[i].dur);
    }


    //COMEÇO ESCALONAMENTO

    //Criacao de filas
    int filas[8][count_proc];
    /*
            P0 P1 P2 P3 P4 P5 P6 P7 ... PN
    fila 0  x  x  x  x  x  x  x  x      x       quantum = 1
    fila 1  x  x  x  x  x  x  x  x      x       quantum = 2
    fila 2  x  x  x  x  x  x  x  x      x       quantum = 4
    fila 3  x  x  x  x  x  x  x  x      x       quantum = 8
    fila 4  x  x  x  x  x  x  x  x      x       quantum = 16
    fila 5  x  x  x  x  x  x  x  x      x       quantum = 32
    fila 6  x  x  x  x  x  x  x  x      x       quantum = 64
    fila 7  x  x  x  x  x  x  x  x      x       quantum = 128

        onde x é existe (1), não existe (0).
    */
    int quantum[] = {1, 2, 4, 8, 16, 32, 64, 128};

    //Inicialização filas vazias
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < count_proc; j++){
            filas[i][j] = 0; //0 será para nenhum processo, 1 será para quando há processo
        }
    }

    int tempo = 0;              //Controle do tempo da simulação
    int tempo_print = 0;        //Variavel para print das linhas de tempo
    int duracao_total = 0;      //Armazena tempo total dos processos
    int break_flag = 0;

    //Variaveis para calculos de Tempo Medio de Vida, Tempo Medio de Espera e Numero de Trocas de Contextos
    int onde_parou[count_proc];
    int soma_espera[count_proc];
    for(int i = 0; i < count_proc; i++){
        onde_parou[i] = geral[i].data;
        soma_espera[i] = 0;
    }
    float med_vida=0;
    int trocas=0;               //Contabiliza trocas de contexto
    int processo_anterior = -1; //Variavel para controle de troca de contexto

    //Soma a duracao de todos processos
    for(int i = 0; i < count_proc; i++){
        duracao_total = duracao_total + geral[i].dur;
    }

    printf("\ntempo\tP0  P1  P2  P3  P4  P5  P6  P7  P8  P9  P10 P11 \n");
    while(tempo < duracao_total){                                            //tempo limitado por ser uma simulação; caso contrário, seria loop infinito sempre esperando novos processos

        //Processos entram no seu tempo (sempre na primeira fila)
        for(int j = 0; j < count_proc; j++){
            if(atual[j].data <= tempo && atual[j].entrou == 0){         //se sim, processo entra na fila 0
                filas[0][j] = 1;
                atual[j].entrou = 1;
            }
        }

        //descomentar para ver comportamento das filas
        /*printf("\n");
        for(int i = 0; i < 8; i++){
            printf("(Fila %d) ", i);
            for(int j = 0; j < count_proc; j++){
                printf("%d ", filas[i][j]);
            }
            printf("\n");
        }
        printf("\n");*/

        for(int i = 0; i < 8; i++){                 //Caminha pelas filas
            for(int j = 0; j < count_proc; j++){    //Caminha pelos processos
                if(filas[i][j] == 1){               //Se processo existe

                    if(processo_anterior != j){     //Caso processo atual seja diferente do anterior, houve troca de contexto
                        trocas = trocas + 1;
                    }

                    if(atual[j].dur - quantum[i] <= 0){                         //Se quantum da fila é suficiente para terminar
                        filas[i][j] = 0;                                        //Esvazia posição na fila

                        tempo_print = tempo;
                        tempo = tempo + atual[j].dur;                           //Passa tempo equvalente a duração pendente

                        soma_espera[j] = soma_espera[j] + tempo_print - onde_parou[j];  //Calcula a espera do processo
                        onde_parou[j] = tempo + atual[j].dur;

                        med_vida=med_vida+(tempo-atual[j].data);                //Calcula a vida do processo e adiciona ao vetor de vida

                        for(int x = 0; x < atual[j].dur; x++){                  //Imprime diagrama de tempo de execução
                            //imprime as linhas de tempos
                            printf("%d - %d\t", tempo_print, tempo_print+1);
                            tempo_print++;
                            for(int h = 0; h < j; h++){
                                printf("--  ");
                            }
                            printf("##  ");
                            for(int h = 0; h < count_proc - j - 1; h++){
                                printf("--  ");
                            }
                            printf("\n");
                        }
                        atual[j].dur = 0;
                    }
                    else{                               //Se quantum da fila não é suficiente para terminar, então desce a fila (i+1)
                        filas[i][j] = 0;
                        filas[i+1][j] = 1;

                        soma_espera[j] = soma_espera[j] + tempo_print - onde_parou[j]; //Calcula a espera do processo
                        onde_parou[j] = tempo + quantum[i];

                        tempo_print = tempo;
                        tempo = tempo + quantum[i];     //Passa tempo equivalente ao quantum

                        for(int x = 0; x < quantum[i]; x++){                    //Imprime diagrama de tempo de execução
                            //imprime as linhas de tempos
                            printf("%d - %d\t", tempo_print, tempo_print+1);
                            tempo_print++;
                            for(int h = 0; h < j; h++){
                                printf("--  ");
                            }
                            printf("##  ");
                            for(int h = 0; h < count_proc - j - 1; h++){
                                printf("--  ");
                            }
                            printf("\n");
                        }

                        atual[j].dur = atual[j].dur - quantum[i];               //Recalcula tempo restante para finalizar processo
                    }

                    processo_anterior = j; //Guarda processo executado no momento

                    //break para colunas
                    break_flag = 1;
                    break; //sai
                }
            }
            //break para linhas
            if(break_flag == 1){
                break_flag = 0;
                break;
            }
        }
    }

    float espera_med = 0;
    for(int i = 0; i < count_proc; i++){
        espera_med = espera_med + soma_espera[i];
    }
    espera_med = espera_med/count_proc;
    printf("\nMedia de espera: %f\n", espera_med); //Imprime a média de espera
    printf("Media de vida: %f\n", med_vida/count_proc); //Calcula e imprime a média de vida
    printf("Trocas de contexto: %d\n", trocas); //Imprime as trocas de contexto

}

