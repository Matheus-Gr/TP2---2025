#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "tipos.h"

#define MEMORY_SIZE 19
#define INPUT_TAPES 19
#define TOTAL_TAPES 20

typedef struct {
    int value;
    int blocked;
} Record;

typedef struct {
    FILE* file;
    int currentValue;
    int isFinished;
} Tape;

void initializeTapes(Tape* tapes) {
    char filename[20];
    for (int i = 0; i < TOTAL_TAPES; i++) {
        sprintf(filename, "tape%d.txt", i);
        tapes[i].file = fopen(filename, "w+");
        tapes[i].isFinished = 0;
        tapes[i].currentValue = 0;
    }
}

void closeTapes(Tape* tapes) {
    for (int i = 0; i < TOTAL_TAPES; i++) {
        fclose(tapes[i].file);
    }
}

int findMinimum(Record* memory, int size) {
    int minIndex = -1;
    int minValue = INT_MAX;
    
    for (int i = 0; i < size; i++) {
        if (!memory[i].blocked && memory[i].value < minValue) {
            minValue = memory[i].value;
            minIndex = i;
        }
    }
    return minIndex;
}

void replacementSelection(FILE* input, int quantidade, Estatisticas* stats) {
    Tape* tapes = (Tape*)malloc(TOTAL_TAPES * sizeof(Tape));
    Record* memory = (Record*)malloc(MEMORY_SIZE * sizeof(Record));
    
    initializeTapes(tapes);
    
    // Initial load to memory
    int inputIndex = 0;
    int value;
    for (int i = 0; i < MEMORY_SIZE && inputIndex < quantidade; i++) {
        if (fread(&value, sizeof(int), 1, input) == 1) {
            memory[i].value = value;
            memory[i].blocked = 0;
            inputIndex++;
            stats->transferencias++;
        }
    }
    
    int currentTape = 0;
    int lastValue = INT_MIN;
    
    while (inputIndex < quantidade) {
        int minIndex = findMinimum(memory, MEMORY_SIZE);
        
        if (minIndex == -1) {
            // All records blocked, start new run
            currentTape = (currentTape + 1) % INPUT_TAPES;
            lastValue = INT_MIN;
            for (int i = 0; i < MEMORY_SIZE; i++) {
                memory[i].blocked = 0;
            }
            continue;
        }

        // Write to current output tape
        fprintf(tapes[currentTape].file, "%d ", memory[minIndex].value);
        stats->transferencias++;
        
        // Read new value from input
        if (fread(&value, sizeof(int), 1, input) == 1) {
            memory[minIndex].value = value;
            stats->transferencias++;
            if (value < lastValue) {
                memory[minIndex].blocked = 1;
            } else {
                memory[minIndex].blocked = 0;
            }
            lastValue = value;
            inputIndex++;
        } else {
            memory[minIndex].blocked = 1;
        }
    }
    
    // Output remaining records
    while (1) {
        int minIndex = findMinimum(memory, MEMORY_SIZE);
        if (minIndex == -1) break;
        fprintf(tapes[currentTape].file, "%d ", memory[minIndex].value);
        stats->transferencias++;
        memory[minIndex].blocked = 1;
    }
    
    closeTapes(tapes);
    free(memory);
    free(tapes);
}

void intercalacao_f1(const char* arquivoEntrada, int quantidade, int exibirResultado, Estatisticas* stats) {
    FILE* input = fopen(arquivoEntrada, "rb");
    if (!input) {
        printf("Erro ao abrir arquivo de entrada\n");
        return;
    }
    
    replacementSelection(input, quantidade, stats);
    
    fclose(input);
}
