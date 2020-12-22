//
//  genMat.c
//  trab-so
//
//  Created by Henrique Tornelli on 30/11/19.
//  Copyright © 2019 Henrique Tornelli. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

int random_int(int min, int max);

int main (int argc, char *argv[]) {

    // ========================================== //
    
    // PARAM 1: LINES
    int l = atoi(argv[1]);
    
    // PARAM 2: COLUMNS
    int c = atoi(argv[2]);
    
    // PARAM 3: MINIMUM NUMBER
    int min = atoi(argv[3]);
    
    // PARAM 4: MAXIMUM NUMBER
    int max = atoi(argv[4]);
    
    // PARAM 5: OUTPUT FILE NAME
    char* arq = argv[5];
    
    // ========================================== //
    
    FILE *f = fopen(arq, "w");
    
    for (int i=0; i<l; i++) {
        for (int j=0; j<c; j++) {
            fprintf(f, "%d ", random_int(min, max));
        }
        fprintf(f, "\n");
    }
    
    fclose(f);
}

int random_int(int min, int max) {
   return min + rand() % (max+1 - min);
}
