//
//  v2.c
//  trab-so
//
//  Created by Henrique Tornelli on 08/10/19.
//  Copyright © 2019 Henrique Tornelli. All rights reserved.
//

#define _GNU_SOURCE     // Necessário para configuração do processor afinity
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <sched.h>          // Biblioteca para processor afinity

#define LINES 100          // Linhas da matriz
#define COLS 100           // Colunas da matriz

#define POW 50              // Ponto de elevação

long double** readMat(char* arq);
void writeMat(char* arq, int p, long double** mat);
long double** powMatM(int n, long double** mat);
long double** powMatN(int n, long double** mat);
long double** multMat(long double** m, long double** n);
void printMat(long double** mat);
long double** allocMat(int l, int c);
void printProgress(int s);

// THREADS
#define T_COUNT 4           // Contagem de threads
#define TIO1_ID 0           // ID da thread de IO1 e número da CPU
#define TIO2_ID 1           // ID da thread de IO2 e número da CPU
#define TCPU1_ID 2          // ID da thread de CPU1 e número da CPU
#define TCPU2_ID 3          // ID da thread de CPU2 e número da CPU

void* tIO1(void* arg);      // Função da thread de IO1
void* tIO2(void* arg);      // Função da thread de IO2
void* tCPU1(void* arg);     // Função da thread de CPU1
void* tCPU2(void* arg);     // Função da thread de CPU2

// SHARED VARIABLES
long double** m;            // Matriz M
long double** n;            // Matriz N
long double** r;            // Matriz R

char* mArq = "m.dat";
char* nArq = "n.dat";

long double** mBuffer[POW];     // Buffer de M
long double** nBuffer[POW];     // Buffer de N
long double** rBuffer[POW];     // Buffer de R

int main (void) {
    clock_t Ticks[2];
    Ticks[0] = clock();

    // INICIO

    printProgress(0);

    // Aloca memória de buffers
    for (int i=2; i<=POW; i++) {
        mBuffer[i] = allocMat(LINES, COLS);
        nBuffer[i] = allocMat(LINES, COLS);
        rBuffer[i] = allocMat(LINES, COLS);
    }

    // Variáveis auxiliares para as threads
    pthread_t t[T_COUNT];

    // Passo atual das threads
    int* stIO1 = (int*) malloc(sizeof(int*));
    int* stIO2 = (int*) malloc(sizeof(int*));
    int* stCPU1 = (int*) malloc(sizeof(int*));
    int* stCPU2 = (int*) malloc(sizeof(int*));

    printProgress(1);
    *stIO1 = 1;
    pthread_create(&t[TIO1_ID], NULL, tIO1, (void*) stIO1);     // Leitura de M
    *stIO2 = 1;
    pthread_create(&t[TIO2_ID], NULL, tIO2, (void*) stIO2);     // Leitura de N

    pthread_join(t[TIO1_ID], NULL);         // Espera a TIO1 finalizar a leitura de M
    pthread_join(t[TIO2_ID], NULL);         // Espera a TIO2 finalizar a leitura de N

    printProgress(2);
    *stCPU1 = 1;
    pthread_create(&t[TCPU1_ID], NULL, tCPU1, (void*) stCPU1);      // Elevação de M
    *stCPU2 = 1;
    pthread_create(&t[TCPU2_ID], NULL, tCPU2, (void*) stCPU2);      // Elevação de N

    pthread_join(t[TCPU1_ID], NULL);        // Espera a TCPU1 finalizar a elevação de M
    pthread_join(t[TCPU2_ID], NULL);        // Espera a TCPU2 finalizar a elevação de M

    printProgress(3);
    *stIO1 = 2;
    pthread_create(&t[TIO1_ID], NULL, tIO1, (void*) stIO1);         // Escrita da elevação de M
    *stIO2 = 2;
    pthread_create(&t[TIO2_ID], NULL, tIO2, (void*) stIO2);         // Escrita da elevação de N

    pthread_join(t[TIO1_ID], NULL);
    pthread_join(t[TIO2_ID], NULL);

    printProgress(4);
    *stCPU1 = 2;
    pthread_create(&t[TCPU1_ID], NULL, tCPU1, (void*) stCPU1);      // Multiplicação das matrizes

    pthread_join(t[TCPU1_ID], NULL);

    printProgress(5);
    *stIO1 = 3;
    pthread_create(&t[TIO1_ID], NULL, tIO1, (void*) stIO1);         // Escrita da multiplicação das matrizes

    pthread_join(t[TIO1_ID], NULL);

    printProgress(-1);
    *stIO2 = 3;
    pthread_create(&t[TIO2_ID], NULL, tIO2, (void*) stIO2);         // Exibindo resultado da multiplicação das matrizes

    pthread_join(t[TIO2_ID], NULL);

    // FIM

    Ticks[1] = clock();
    double Tempo = (Ticks[1] - Ticks[0]) * 1000.0 / CLOCKS_PER_SEC;
    printf("\nTempo gasto: %g milissegundos\n", Tempo);

    return 0;
}

void* tIO1(void* arg) {
    cpu_set_t cpuset;
    int cpu = TIO1_ID;         // Define qual CPU irá usar

    CPU_ZERO(&cpuset);
    CPU_SET(cpu, &cpuset);
    sched_setaffinity(0, sizeof(cpuset), &cpuset);

    int* s = (int*) arg;

    // Realiza a verificação de qual passo a thread deve executar
    
    if (*s == 1) {
        // Ler M
        m = readMat(mArq);

        return NULL;
    }

    if (*s == 2) {
        // Escrever a elevação de M

        for (int i=2; i<=POW; i++)
            writeMat("m", i, mBuffer[i]);
        
        return NULL;
    }

    if (*s == 3) {
        // Escrever o resultado das multiplicações

        writeMat("r", 0, r);
    }
}

void* tIO2(void* arg) {
    cpu_set_t cpuset;
    int cpu = TIO2_ID;         // Define qual CPU irá usar

    CPU_ZERO(&cpuset);
    CPU_SET(cpu, &cpuset);
    sched_setaffinity(0, sizeof(cpuset), &cpuset);

    int* s = (int*) arg;

    // Realiza a verificação de qual passo a thread deve executar

    if (*s == 1) {
        // Ler N
        n = readMat(nArq);
        
        return NULL;
    }

    if (*s == 2) {
        // Escrever a elevação de N

        for (int i=2; i<=POW; i++)
            writeMat("n", i, nBuffer[i]);
        
        return NULL;
    }

    if (*s == 3) {
        // Exibir a multiplicação

        printMat(r);

        return NULL;
    }
}

void* tCPU1(void* arg) {
    cpu_set_t cpuset;
    int cpu = TCPU1_ID;         // Define qual CPU irá usar

    CPU_ZERO(&cpuset);
    CPU_SET(cpu, &cpuset);
    sched_setaffinity(0, sizeof(cpuset), &cpuset);

    int* s = (int*) arg;

    // Realiza a verificação de qual passo a thread deve executar

    if (*s == 1) {
        // Elevar M
        m = powMatM(POW, m);
        
        return NULL;
    }

    if (*s == 2) {
        // Multiplicar matrizes
        r = multMat(m, n);

        return NULL;
    }
}

void* tCPU2(void* arg) {
    cpu_set_t cpuset;
    int cpu = TCPU2_ID;         // Define qual CPU irá usar

    CPU_ZERO(&cpuset);
    CPU_SET(cpu, &cpuset);
    sched_setaffinity(0, sizeof(cpuset), &cpuset);

    int* s = (int*) arg;

    // Realiza a verificação de qual passo a thread deve executar

    if (*s == 1) {
        // Elevar N
        n = powMatN(POW, n);
        
        return NULL;
    }
}

long double** readMat(char* arq) {
    FILE* f = fopen(arq, "r");
    
    long double** m = (long double**) malloc (LINES * sizeof(long double*));
    for (int i=0; i<LINES; i++) {
        m[i] = (long double*) malloc (COLS * sizeof(long double));
        for (int j=0; j<COLS; j++) {
            fscanf(f, "%Lf", &m[i][j]);
        }
        fscanf(f, "\n");
    }
    
    fclose(f);
    
    return m;
}

void writeMat(char* arq, int p, long double** mat) {
    char* arqName = (char*) malloc (10 * sizeof(char));
    
    if (p > 0) sprintf(arqName, "%s%d.dat", arq, p);
    else sprintf(arqName, "%s.dat", arq);
    
    FILE *f = fopen(arqName, "w");
    
    for (int i=0; i<LINES; i++) {
        for (int j=0; j<COLS; j++) {
            fprintf(f, "%.0Lf ", mat[i][j]);
        }
        fprintf(f, "\n");
    }
    
    fclose(f);
}

long double** powMatM(int n, long double** mat) {
    long double** mAux = mat;
    for (int t=2; t<=n; t++) {
        long double sum = 0;
        
        for (int c=0; c<LINES; c++) {
            for (int d=0; d<COLS; d++) {
                for (int k=0; k<LINES; k++) {
                    sum = sum + mAux[c][k] * mat[k][d];
                }
                mBuffer[t][c][d] = sum;
                sum = 0;
            }
        }

        mAux = mBuffer[t];
    }
    
    return mAux;
}

long double** powMatN(int n, long double** mat) {
    long double** mAux = mat;
    for (int t=2; t<=n; t++) {
        long double sum = 0;
        
        for (int c=0; c<LINES; c++) {
            for (int d=0; d<COLS; d++) {
                for (int k=0; k<LINES; k++) {
                    sum = sum + mAux[c][k] * mat[k][d];
                }
                mBuffer[t][c][d] = sum;
                sum = 0;
            }
        }

        mAux = mBuffer[t];
    }
    
    return mAux;
}

long double** multMat(long double** m, long double** n) {
    long double sum = 0;
    long double** r = allocMat(LINES, COLS);
    
    for (int c=0; c<LINES; c++) {
        for (int d=0; d<COLS; d++) {
            for (int k=0; k<LINES; k++) {
                sum = sum + m[c][k] * n[k][d];
            }
            r[c][d] = sum;
            sum = 0;
        }
    }

    return r;
}

void printMat(long double** mat) {
    for (int i=0; i<LINES; i++) {
        for (int j=0; j<COLS; j++) {
            printf("%.0Lf ", mat[i][j]);
        }
        printf("\n");
    }
}

long double** allocMat(int l, int c) {
    long double** x = (long double**) malloc (l * sizeof(long double*));
    for (int i=0; i<l; i++) {
        x[i] = (long double*) malloc (c * sizeof(long double));
    }

    return x;
}

void printProgress(int s) {
    char percentage = '%';
    switch (s) {
        case 0:
            system("tput reset");
            printf("Iniciando programa...\n\n");
            printf("[       ] %d%c\n\n", 14, percentage);
            break;
        case 1:
            system("tput reset");
            printf("Lendo matrizes...\n\n");
            printf("[##     ] %d%c\n\n", 28, percentage);
            break;
        case 2:
            system("tput reset");
            printf("Elevando matrizes...\n\n");
            printf("[###    ] %d%c\n\n", 42, percentage);
            break;
        case 3:
            system("tput reset");
            printf("Escrevendo matrizes...\n\n");
            printf("[####   ] %d%c\n\n", 56, percentage);
            break;
        case 4:
            system("tput reset");
            printf("Multiplicando matrizes...\n\n");
            printf("[#####  ] %d%c\n\n", 70, percentage);
            break;
        case 5:
            system("tput reset");
            printf("Escrevendo resultado...\n\n");
            printf("[###### ] %d%c\n\n", 84, percentage);
            break;
        default:
            system("tput reset");
            printf("Finalizando...\n\n");
            printf("[#######] %d%c\n\n", 100, percentage);
            break;
    }
}
