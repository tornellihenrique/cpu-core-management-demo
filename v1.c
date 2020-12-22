//
//  v1.c
//  trab-so
//
//  Created by Henrique Tornelli on 08/10/19.
//  Copyright © 2019 Henrique Tornelli. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define LINES 100
#define COLS 100

long double** readMat(char* arq);
void writeMat(char* arq, int p, long double** mat);
long double** powMat(char* arq, int n, long double** mat);
long double** multMat(char* arq, long double** m, long double** n);
void printMat(long double** mat);
long double** allocMat(int l, int c);
void printProgress(int s);

int main (void) {
    clock_t Ticks[2];
    Ticks[0] = clock();

    // INICIO

    printProgress(0);
    
    // Armazena nomes dos arquivos contendo as matrizes
    char* mArq = "m.dat";
    char* nArq = "n.dat";
    
    // Realiza as leituras das matrizes
    printProgress(1);
    long double** m = readMat(mArq);
    printProgress(2);
    long double** n = readMat(nArq);
    
//    printf("PRINTING m\n");
//    printMat(m);
//    printf("\n");
//
//    printf("PRINTING n\n");
//    printMat(n);
//    printf("\n");
    
    int powLevel = 50;
    
    // Elevando as matrizes
    printProgress(3);
    m = powMat("m", powLevel, m);
    printProgress(4);
    n = powMat("n", powLevel, n);
    
    // Multiplicando matrizes
    printProgress(5);
    long double** r = multMat("r", m, n);
    
    printProgress(-1);

    printMat(r);

    // FIM

    Ticks[1] = clock();
    double Tempo = (Ticks[1] - Ticks[0]) * 1000.0 / CLOCKS_PER_SEC;
    printf("\nTempo gasto: %g milissegundos\n", Tempo);
    
    return 0;
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
    
    if (p > 0) sprintf(arqName, "./mFiles/%s%d.dat", arq, p);
    else sprintf(arqName, "./mFiles/%s.dat", arq);
    
    FILE *f = fopen(arqName, "w");
    
    for (int i=0; i<LINES; i++) {
        for (int j=0; j<COLS; j++) {
            fprintf(f, "%.0Lf ", mat[i][j]);
        }
        fprintf(f, "\n");
    }
    
    fclose(f);
}

long double** powMat(char* arq, int n, long double** mat) {
    long double** mAux = mat;
    for (int t=2; t<=n; t++) {
        long double sum = 0;
        long double** r = allocMat(LINES, COLS);
        
        // Matrix multiplication
        for (int c=0; c<LINES; c++) {
            for (int d=0; d<COLS; d++) {
                for (int k=0; k<LINES; k++) {
                    sum = sum + mAux[c][k] * mat[k][d];
                }
                r[c][d] = sum;
                sum = 0;
            }
        }
        
        writeMat(arq, t, r);
        mAux = r;
    }
    
    return mAux;
}

long double** multMat(char* arq, long double** m, long double** n) {
    long double sum = 0;
    long double** r = allocMat(LINES, COLS);
    
    // Matrix multiplication
    for (int c=0; c<LINES; c++) {
        for (int d=0; d<COLS; d++) {
            for (int k=0; k<LINES; k++) {
                sum = sum + m[c][k] * n[k][d];
            }
            r[c][d] = sum;
            sum = 0;
        }
    }
    
    writeMat(arq, 0, r);
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
            printf("Lendo matriz M...\n\n");
            printf("[##     ] %d%c\n\n", 28, percentage);
            break;
        case 2:
            system("tput reset");
            printf("Lendo matriz N...\n\n");
            printf("[###    ] %d%c\n\n", 42, percentage);
            break;
        case 3:
            system("tput reset");
            printf("Elevando matriz M...\n\n");
            printf("[####   ] %d%c\n\n", 56, percentage);
            break;
        case 4:
            system("tput reset");
            printf("Elevando matriz N...\n\n");
            printf("[#####  ] %d%c\n\n", 70, percentage);
            break;
        case 5:
            system("tput reset");
            printf("Multiplicando matrizes...\n\n");
            printf("[###### ] %d%c\n\n", 84, percentage);
            break;
        default:
            system("tput reset");
            printf("Finalizando...\n\n");
            printf("[#######] %d%c\n\n", 100, percentage);
            break;
    }
}
