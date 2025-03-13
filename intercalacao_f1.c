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

/**
 * @brief Initializes the tape files.
 *
 * This function creates and opens the tape files that will be used during the sorting process.
 * It also sets the initial state of each tape.
 *
 * @param tapes An array of Tape structures to be initialized.
 */
void initializeTapes(Tape* tapes) {
    char filename[20];
    for (int i = 0; i < TOTAL_TAPES; i++) {
        sprintf(filename, "tape%d.txt", i);
        tapes[i].file = fopen(filename, "w+");
        tapes[i].isFinished = 0;
        tapes[i].currentValue = 0;
    }
}

/**
 * @brief Closes the tape files.
 *
 * This function closes all the tape files that were used during the sorting process.
 *
 * @param tapes An array of Tape structures to be closed.
 */
void closeTapes(Tape* tapes) {
    for (int i = 0; i < TOTAL_TAPES; i++) {
        fclose(tapes[i].file);
    }
}

/**
 * @brief Finds the minimum value in memory that is not blocked.
 *
 * This function searches through the memory array to find the smallest value
 * that is not currently blocked. Blocked values are those that are smaller
 * than the last value written to the output tape in the current run.
 *
 * @param memory The array of Record structures representing the memory.
 * @param size The size of the memory array.
 * @param stats Pointer to the statistics structure to track comparisons.
 * @return The index of the minimum value in memory, or -1 if all values are blocked.
 */
int findMinimum(Record* memory, int size, Estatisticas* stats) {
    int minIndex = -1;
    int minValue = INT_MAX;
    
    for (int i = 0; i < size; i++) {
        stats->comparacoes++; // Count comparisons
        stats->QCN++; // Incrementa a contagem de comparações entre notas
        if (!memory[i].blocked && memory[i].value < minValue) {
            minValue = memory[i].value;
            minIndex = i;
        }
    }
    return minIndex;
}

/**
 * @brief Implements the replacement selection algorithm for external sorting.
 *
 * This function reads records from an input file, sorts them using the replacement
 * selection algorithm, and writes the sorted records to a set of output tapes.
 * It then merges the sorted runs from the tapes into a single output file.
 *
 * @param input The input file to be sorted.
 * @param quantidade The number of records to sort.
 * @param stats Pointer to the statistics structure to track sorting metrics.
 */
void replacementSelection(FILE* input, int quantidade, Estatisticas* stats) {
    Tape* tapes = (Tape*)malloc(TOTAL_TAPES * sizeof(Tape));
    Record* memory = (Record*)malloc(MEMORY_SIZE * sizeof(Record));
    Registro reg;
    
    initializeTapes(tapes);
    
    // Initial load to memory
    stats->quantidade = quantidade; // Set the quantity
    
    int inputIndex = 0;
    for (int i = 0; i < MEMORY_SIZE && inputIndex < quantidade; i++) {
        if (fread(&reg, sizeof(Registro), 1, input) == 1) {
            memory[i].value = (int)(reg.nota * 100); // Convert float to int for better comparison
            memory[i].blocked = 0;
            inputIndex++;
            stats->transferencias++;
            stats->QLGB++; // Incrementa a contagem de leitura na geração de blocos
            stats->QLT++; // Incrementa a contagem de leituras totais
        }
    }
    
    int currentTape = 0;
    int lastValue = INT_MIN;
    
    while (inputIndex < quantidade || findMinimum(memory, MEMORY_SIZE, stats) != -1) {
        int minIndex = findMinimum(memory, MEMORY_SIZE, stats);
        
        if (minIndex == -1) {
            // All records blocked, start new run
            currentTape = (currentTape + 1) % INPUT_TAPES;
            lastValue = INT_MIN;
            for (int i = 0; i < MEMORY_SIZE; i++) {
                memory[i].blocked = 0;
            }
            stats->comparacoes++;
            stats->QCN++; // Incrementa a contagem de comparações entre notas
            continue;
        }

        // Write to current output tape
        fprintf(tapes[currentTape].file, "%d ", memory[minIndex].value);
        stats->transferencias++;
        stats->QEGB++; // Incrementa a contagem de escrita na geração de blocos
        stats->QET++; // Incrementa a contagem de escritas totais
        
        // Read new value from input if available
        if (inputIndex < quantidade && fread(&reg, sizeof(Registro), 1, input) == 1) {
            int newValue = (int)(reg.nota * 100);
            memory[minIndex].value = newValue;
            stats->transferencias++;
            stats->QLGB++; // Incrementa a contagem de leitura na geração de blocos
            stats->QLT++; // Incrementa a contagem de leituras totais
            stats->comparacoes++;
            stats->QCN++; // Incrementa a contagem de comparações entre notas
            
            if (newValue < lastValue) {
                memory[minIndex].blocked = 1;
                stats->movimentacoes++;
            } else {
                memory[minIndex].blocked = 0;
            }
            lastValue = newValue;
            inputIndex++;
        } else {
            memory[minIndex].blocked = 1;
        }
    }
    
    // Merge phase
    FILE* output = fopen("output.bin", "wb");
    if (output) {
        for (int i = 0; i < INPUT_TAPES; i++) {
            rewind(tapes[i].file);
            int value;
            while (fscanf(tapes[i].file, "%d ", &value) == 1) {
                reg.nota = value / 100.0f;
                fwrite(&reg, sizeof(Registro), 1, output);
                stats->transferencias++;
                stats->QET++; // Incrementa a contagem de escritas totais
            }
        }
        fclose(output);
    }
    
    closeTapes(tapes);
    free(memory);
    free(tapes);
}

/**
 * @brief Sorts an external file using the replacement selection and merge strategy.
 *
 * This function orchestrates the sorting process by calling the replacementSelection
 * function to generate sorted runs on multiple tapes, and then merges these runs
 * into a single sorted output file.
 *
 * @param arquivoEntrada The name of the input file.
 * @param quantidade The number of records to sort.
 * @param exibirResultado A flag to indicate whether to print the sorted records.
 * @param stats Pointer to the statistics structure to track sorting metrics.
 */
void intercalacao_f1(const char* arquivoEntrada, int quantidade, int exibirResultado, Estatisticas* stats) {
    FILE* input = fopen(arquivoEntrada, "rb");
    if (!input) {
        printf("Erro ao abrir arquivo de entrada\n");
        return;
    }
    
    replacementSelection(input, quantidade, stats);
    
    if (exibirResultado) {
        FILE* output = fopen("output.bin", "rb");
        if (output) {
            Registro reg;
            while (fread(&reg, sizeof(Registro), 1, output) == 1) {
                printf("%s %.2f %s %s %s\n", 
                    reg.inscricao, reg.nota,
                    reg.estado, reg.cidade, reg.curso);
            }
            fclose(output);
        }
    }
    
    fclose(input);
}
