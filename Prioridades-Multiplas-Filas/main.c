#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef struct {
    int data, dur, entrou;
    char prio;
} Processo;

int conta_linhas(){
    FILE *entrada;
    int conta = 1, c;

    //Abertura do arquivo de entrada
    entrada = fopen("entrada2.txt", "r");
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

int main (){
    FILE *entrada;
    int count_proc, x;

    count_proc = conta_linhas();

    entrada = fopen("entrada2.txt", "r");
    if (entrada == NULL){
        printf("Erro ao abrir arquivo.\n");
        return 0;
    }

    //Criação lista geral de processos
    Processo geral[count_proc];
    Processo atual[count_proc];

    int i = 0;
    int data, dur, ent;
    char prio;
    while(fscanf(entrada, "%d %d %d %c", &data, &dur, &ent, &prio) != -1){
        geral[i].data = data;
        geral[i].dur = dur;
        geral[i].entrou = 0;
        geral[i].prio = prio;
        i++;
    }

    for(i = 0; i < count_proc; i++){
        atual[i].data= geral[i].data;
        atual[i].dur = geral[i].dur;
        atual[i].entrou = 0;
        atual[i].prio = geral[i].prio;
        printf("P%d \t criacao: %d \t duracao: %d \t Pertencente a fila: %c\n", i, atual[i].data, atual[i].dur, atual[i].prio);
    }


    //Começo Escalonamento

    //Criacao de filas
    int filas[4][count_proc];
    int pula[4][count_proc];
    int quantum = 2;

    //inicialização filas vazias
    for(int i = 0; i < 4; i++){
        for(int j = 0; j < count_proc; j++){
            filas[i][j] = 0; //0 será para nenhum processo, 1 será para quando há processo
            pula[i][j] = 0;
        }
    }


    int tempo = 0;
    int tempo_print = 0; //variavel para print das linhas de tempo
    int break_flag = 0;
    int espera = 0; //Armazena a espera por processo
    int volta_fila=0;


    //Variaveis para calculos de Tempo Medio de Espera, Tempo Medio de vida e Numero de Trocas de Contextos
    int onde_parou[count_proc];
    int soma_espera[count_proc];
    int duracao_total = 0;      //Armazena tempo total dos processos
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

    while(tempo < duracao_total){ //tempo limitado por ser uma simulação; caso contrário, seria loop infinito sempre esperando novos processos
        for(int j = 0; j < count_proc; j++){
            //Processos entram no seu tempo (sempre na primeira fila)
            if(atual[j].data <= tempo && atual[j].entrou == 0){
                if(atual[j].prio=='R'){
                     filas[0][j] = 1;
                     atual[j].entrou = 1;
                }
                if(atual[j].prio=='S'){
                     filas[1][j] = 1;
                     atual[j].entrou = 1;
                }
                if(atual[j].prio=='I'){
                     filas[2][j] = 1;
                     atual[j].entrou = 1;
                }
                 if(atual[j].prio=='B'){
                     filas[3][j] = 1;
                     atual[j].entrou = 1;
                }
            }
        }

        //descomentar para ver comportamento das filas
        printf("\n");
        for(int i = 0; i < 4; i++){
            if(i == 0){
                printf("(Fila R) ");
            }else if(i == 1){
                printf("(Fila S) ");
            }else if(i == 2){
                printf("(Fila I) ");
            }else if(i == 3){
                printf("(Fila B) ");
            }
            for(int j = 0; j < count_proc; j++){
                printf("%d   ", filas[i][j]);
            }
            printf("\n");
        }
        printf("\n");

        int b = 0;
        int i = 0;
        int flag_achou = 0;
        for(i = 0; i < 4; i++){
            for(int j = 0; j < count_proc; j++){
                //executa se tiver processo, conforme a prioridade das filas
                if(filas[i][j] == 1 && pula[i][j] != 1){

                    if(processo_anterior != j){     //Caso processo atual seja diferente do anterior, houve troca de contexto
                        trocas = trocas + 1;
                    }

                    if(atual[j].dur - quantum <= 0){                         //Se quantum da fila é suficiente para terminar
                        filas[i][j] = 0;                                        //Esvazia posição na fila

                        tempo_print = tempo;
                        tempo = tempo + atual[j].dur;                           //Passa tempo equvalente a duração pendente

                        soma_espera[j] = soma_espera[j] + tempo_print - onde_parou[j];  //Calcula a espera do processo
                        onde_parou[j] = tempo_print + atual[j].dur;

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
                    else{                               //Se quantum da fila não é suficiente para terminar
                        filas[i][j] = 1;                //Continua no mesmo local

                        tempo_print = tempo;
                        tempo = tempo + quantum;     //Passa tempo equivalente ao quantum

                        soma_espera[j] = soma_espera[j] + tempo_print - onde_parou[j]; //Calcula a espera do processo
                        onde_parou[j] = tempo_print + quantum;

                        for(int x = 0; x < quantum; x++){                    //Imprime diagrama de tempo de execução
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

                        pula[i][j] = 1;                                      //se executou e não terminou, perde prioridade
                        atual[j].dur = atual[j].dur - quantum;               //Recalcula tempo restante para finalizar processo
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
            //sempre que pular para linha seguintee
            //printf("\nzerando todos pulos; i = %d\n", i);
            for(int k = 0; k < count_proc; k++){
                pula[i][k] = 0;
                if(filas[i][k] != 0){
                    i = i-1;
                }
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

    //printf("Media de espera: %d\n", med_espera[0]/med_espera[1]); //Calcula e imprime a média de espera
    //printf("Media de vida: %d\n", med_vida/med_espera[1]); //Calcula e imprime a média de vida
    //printf("Trocas de contexto: %d\n", trocas); //Imprime as trocas de contexto




