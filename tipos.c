#include "tipos.h"
#include <stdio.h>
#include <time.h>

// Add a timespec variable for higher resolution timing
static struct timespec inicio;

void inicializarEstatisticas(Estatisticas* stats) {
    stats->comparacoes = 0;
    stats->movimentacoes = 0;
    stats->transferencias = 0;
    stats->quantidade = 0;
    stats->tempoGasto = 0.0;
    stats->QLGB = 0;
    stats->QEGB = 0;
    stats->QLT = 0;
    stats->QET = 0;
    stats->QCN = 0;
    
    // Initialize high precision timer
    clock_gettime(CLOCK_MONOTONIC, &inicio);
}

void finalizarEstatisticas(Estatisticas* stats) {
    struct timespec fim;
    clock_gettime(CLOCK_MONOTONIC, &fim);
    
    // Calculate elapsed time in seconds with nanosecond precision
    stats->tempoGasto = (fim.tv_sec - inicio.tv_sec) + 
                       (fim.tv_nsec - inicio.tv_nsec) / 1e9;
}

void mostrarEstatisticas(Estatisticas* stats) {
    printf("\n=== Estatísticas da Ordenação ===\n");
    printf("Quantidade de registros: %d\n", stats->quantidade);
    printf("Quantidade de comparações entre notas (Q.C.N): %d\n", stats->comparacoes);
    printf("Quantidade de movimentações: %d\n", stats->movimentacoes);
    printf("Quantidade de transferências (Q.L.T + Q.E.T): %d\n", stats->transferencias);
    printf("Quantidade de leitura na geração de blocos (Q.L.G.B): %d\n", stats->QLGB);
    printf("Quantidade de escrita na geração de blocos (Q.E.G.B): %d\n", stats->QEGB);
    printf("Quantidade de leituras totais (Q.L.T): %d\n", stats->QLT);
    printf("Quantidade de escritas totais (Q.E.T): %d\n", stats->QET);
    printf("Tempo de execução (T.EXC): %.3f segundos\n", stats->tempoGasto);
    printf("===============================\n");
}
