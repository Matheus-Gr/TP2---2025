#include "tipos.h"
#include <stdio.h>
#include <time.h>

static clock_t inicio;

void inicializarEstatisticas(Estatisticas* stats) {
    stats->comparacoes = 0;
    stats->transferencias = 0;
    stats->quantidade = 0;
    stats->tempoGasto = 0.0;
    inicio = clock();
}

void finalizarEstatisticas(Estatisticas* stats) {
    clock_t fim = clock();
    stats->tempoGasto = ((double)(fim - inicio)) / CLOCKS_PER_SEC;
}

void mostrarEstatisticas(Estatisticas* stats) {
    printf("\nEstatísticas:\n");
    printf("Quantidade de elementos: %d\n", stats->quantidade);
    printf("Comparações: %d\n", stats->comparacoes);
    printf("Transferências: %d\n", stats->transferencias);
    printf("Tempo gasto: %.2f segundos\n", stats->tempoGasto);
}
