#include "tipos.h"

void inicializarEstatisticas(Estatisticas* stats) {
    stats->numLeituras = 0;
    stats->numEscritas = 0;
    stats->numComparacoes = 0;
    stats->inicioTempo = clock();
    stats->tempoExecucao = 0.0;
}

void finalizarEstatisticas(Estatisticas* stats) {
    stats->tempoExecucao = (double)(clock() - stats->inicioTempo) / CLOCKS_PER_SEC;
}

void mostrarEstatisticas(Estatisticas* stats) {
    printf("Leituras: %d\n", stats->numLeituras);
    printf("Escritas: %d\n", stats->numEscritas);
    printf("Comparacoes: %d\n", stats->numComparacoes);
    printf("Tempo de Execucao: %.2f\n", stats->tempoExecucao);
}
