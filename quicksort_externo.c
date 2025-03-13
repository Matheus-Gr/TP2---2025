#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "quicksort_externo.h"
#include "tipos.h"
#include <limits.h>

#define INSERTION_SORT_THRESHOLD 16

/**
 * @brief Troca dois elementos Registro na memória.
 *
 * Esta função troca o conteúdo de duas estruturas Registro e incrementa
 * a contagem de movimentos na estrutura de estatísticas fornecida.
 *
 * @param a Ponteiro para o primeiro Registro.
 * @param b Ponteiro para o segundo Registro.
 * @param stats Ponteiro para a estrutura de estatísticas para rastrear os movimentos.
 */
static inline void swap(Registro* a, Registro* b, Estatisticas* stats) {
    Registro t = *a;
    *a = *b;
    *b = t;
    stats->movimentacoes++;
}

/**
 * @brief Ordena uma porção de um array usando o insertion sort.
 *
 * Esta função ordena os elementos de um array entre os índices `low` e `high`
 * usando o algoritmo insertion sort. Ela também rastreia as comparações
 * e os movimentos na estrutura de estatísticas fornecida.
 *
 * @param arr O array de elementos Registro a ser ordenado.
 * @param low O índice inicial da porção a ser ordenada.
 * @param high O índice final da porção a ser ordenada.
 * @param stats Ponteiro para a estrutura de estatísticas para rastrear as comparações e os movimentos.
 */
static inline void insertion_sort(Registro* arr, int low, int high, Estatisticas* stats) {
    for (int i = low + 1; i <= high; i++) {
        Registro key = arr[i];
        int j = i - 1;
        
        while (j >= low && arr[j].nota > key.nota) {
            arr[j + 1] = arr[j];
            j--;
            stats->comparacoes++;
            stats->movimentacoes++;
            stats->QCN++; // Incrementa a contagem de comparações entre notas
        }
        arr[j + 1] = key;
    }
}

/**
 * @brief Determina a mediana de três elementos Registro.
 *
 * Esta função seleciona a mediana do primeiro, do meio e do último elemento
 * de uma porção de um array. Ela troca os elementos para garantir que a mediana esteja
 * no índice `high` e retorna esse índice.
 *
 * @param arr O array de elementos Registro.
 * @param low O índice inicial da porção.
 * @param high O índice final da porção.
 * @param stats Ponteiro para a estrutura de estatísticas para rastrear as trocas.
 * @return O índice do elemento mediano (que é `high`).
 */
static inline int median_of_three(Registro* arr, int low, int high, Estatisticas* stats) {
    int mid = low + (high - low) / 2;
    
    if (arr[mid].nota < arr[low].nota){
        swap(&arr[low], &arr[mid], stats);
        stats->QCN++; // Incrementa a contagem de comparações entre notas
    }
    if (arr[high].nota < arr[low].nota){
        swap(&arr[low], &arr[high], stats);
        stats->QCN++; // Incrementa a contagem de comparações entre notas
    }
    if (arr[mid].nota < arr[high].nota){
        swap(&arr[mid], &arr[high], stats);
        stats->QCN++; // Incrementa a contagem de comparações entre notas
    }
        
    return high;
}

/**
 * @brief Particiona uma porção de um array para o quicksort.
 *
 * Esta função seleciona um pivô (usando a mediana de três), então reorganiza
 * o array de forma que todos os elementos menores que o pivô estejam antes dele, e
 * todos os elementos maiores que o pivô estejam depois dele.
 *
 * @param arr O array de elementos Registro.
 * @param low O índice inicial da porção.
 * @param high O índice final da porção.
 * @param stats Ponteiro para a estrutura de estatísticas para rastrear as comparações e as trocas.
 * @return O índice do elemento pivô após o particionamento.
 */
static int partition(Registro* arr, int low, int high, Estatisticas* stats) {
    int pivot_idx = median_of_three(arr, low, high, stats);
    Registro pivot = arr[pivot_idx];
    int i = low - 1;
    int j = high + 1;
    
    while (1) {
        do {
            i++;
            stats->comparacoes++;
            stats->QCN++; // Incrementa a contagem de comparações entre notas
        } while (arr[i].nota < pivot.nota);
        
        do {
            j--;
            stats->comparacoes++;
            stats->QCN++; // Incrementa a contagem de comparações entre notas
        } while (arr[j].nota > pivot.nota);
        
        if (i >= j) {
            stats->comparacoes++;
            stats->QCN++; // Incrementa a contagem de comparações entre notas
            return j;
        }
        
        swap(&arr[i], &arr[j], stats);
        stats->movimentacoes++;
    }
}

/**
 * @brief Implementa o algoritmo quicksort recursivamente.
 *
 * Esta função ordena uma porção de um array usando o algoritmo quicksort.
 * Ela particiona o array e então ordena recursivamente os sub-arrays.
 * Para sub-arrays pequenos, ela muda para o insertion sort para melhor desempenho.
 *
 * @param arr O array de elementos Registro.
 * @param low O índice inicial da porção.
 * @param high O índice final da porção.
 * @param stats Ponteiro para a estrutura de estatísticas para rastrear as comparações e os movimentos.
 */
static void quickSort(Registro* arr, int low, int high, Estatisticas* stats) {
    while (low < high) {
        if (high - low < INSERTION_SORT_THRESHOLD) {
            insertion_sort(arr, low, high, stats);
            break;
        }
        
        int pi = partition(arr, low, high, stats);
        
        if (pi - low < high - pi) {
            quickSort(arr, low, pi, stats);
            low = pi + 1;
        } else {
            quickSort(arr, pi + 1, high, stats);
            high = pi;
        }
    }
}

/**
 * @brief Ordena um arquivo externo usando o algoritmo quicksort.
 *
 * Esta função lê elementos Registro de um arquivo binário, ordena-os usando
 * o algoritmo quicksort e escreve os elementos ordenados de volta em outro arquivo binário.
 * Ele usa a ordenação na memória e rastreia estatísticas como comparações, movimentos,
 * e transferências de arquivos.
 *
 * @param arquivo_entrada O nome do arquivo binário de entrada.
 * @param quantidade A quantidade de elementos Registro para ordenar.
 * @param exibir Um sinalizador para indicar se deve imprimir os elementos ordenados.
 * @param stats Ponteiro para a estrutura de estatísticas para rastrear as métricas de ordenação.
 */
void quicksort_externo(const char* arquivo_entrada, int quantidade, int exibir, Estatisticas* stats) {
    // Set the quantity at the start
    stats->quantidade = quantidade;

    // Single allocation for all data
    Registro* buffer = (Registro*)malloc(quantidade * sizeof(Registro));
    if (!buffer) return;
    
    FILE* entrada = fopen(arquivo_entrada, "rb");
    if (!entrada) {
        free(buffer);
        return;
    }
    
    size_t lidos = fread(buffer, sizeof(Registro), quantidade, entrada);
    stats->transferencias += lidos;
    stats->QLT += lidos; // Incrementa a contagem de leituras totais
    fclose(entrada);
    
    quickSort(buffer, 0, lidos - 1, stats);
    
    FILE* saida = fopen("arquivos/saida.bin", "wb");
    if (saida) {
        fwrite(buffer, sizeof(Registro), lidos, saida);
        stats->transferencias += lidos;
        stats->QET += lidos; // Incrementa a contagem de escritas totais
        
        if (exibir) {
            for (size_t i = 0; i < lidos; i++) {
                printf("%s %.2f %s %s %s\n", 
                    buffer[i].inscricao, buffer[i].nota,
                    buffer[i].estado, buffer[i].cidade, buffer[i].curso);
            }
        }
        fclose(saida);
    }
    
    free(buffer);
}
