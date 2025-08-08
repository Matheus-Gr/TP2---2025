#include <stdio.h>
#include <time.h>
#include "tipos.h"

void inicializarEstatisticas(Estatisticas *est)
{
    est->tempoInicio = clock();
    est->numComparacoes = 0;
    est->numLeituras = 0;
    est->numEscritas = 0;
    est->tempoExecucao = 0.0;
}

void finalizarEstatisticas(Estatisticas *est)
{
    // calcula o tempo de execução
    est->tempoExecucao = (double)(clock() - est->tempoInicio) / CLOCKS_PER_SEC;
}

void mostrarEstatisticas(Estatisticas *est)
{
    printf("\n=== Estatisticas de Execucao ===\n");
    printf("Numero de comparacoes: %ld\n", est->numComparacoes);
    printf("Numero de leituras: %ld\n", est->numLeituras);
    printf("Numero de escritas: %ld\n", est->numEscritas);
    printf("Tempo de execucao: %.2f segundos\n", est->tempoExecucao);
}
