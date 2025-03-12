#include "tipos.h"

void inicializarEstatisticas(Estatisticas *stats)
{
    stats->numLeituras = 0;
    stats->numEscritas = 0;
    stats->numComparacoes = 0;
    stats->inicioTempo = clock();
    stats->tempoExecucao = 0.0;
}

void finalizarEstatisticas(Estatisticas *stats)
{
    stats->tempoExecucao = (double)(clock() - stats->inicioTempo) / CLOCKS_PER_SEC;
}

void mostrarEstatisticas(Estatisticas *stats)
{
    printf("Leituras: %d\n", stats->numLeituras);
    printf("Escritas: %d\n", stats->numEscritas);
    printf("Comparacoes: %d\n", stats->numComparacoes);
    printf("Tempo de Execucao: %.2f\n", stats->tempoExecucao);
}

void inicializar2FFitas(Fita *fitas)
{
    for (int i = 0; i < NUM_2FFITAS; i++)
    {
        // Define o tipo da fita (ENTRADA ou SAIDA)
        if (i < 10)
            fitas[i].tipo = ENTRADA;
        else
            fitas[i].tipo = SAIDA;

        // Inicializa o número de blocos como 0
        fitas[i].nBlocos = 0;

        // Aloca memória para o vetor nItemsBloco (inicialmente sem blocos)
        fitas[i].nItemsBloco = malloc(0); // Vetor vazio inicialmente

        // Cria o arquivo da fita
        char nomeArquivo[20];
        // Nome do arquivo da fita
        sprintf(nomeArquivo, "fitas/fita%d.bin", i + 1);
        // Abre o arquivo para leitura e escrita
        fitas[i].arquivo = fopen(nomeArquivo, "wb+");

        // Verifica se o arquivo foi aberto corretamente
        if (fitas[i].arquivo == NULL)
        {
            printf("Erro ao abrir o arquivo da fita %d\n", i + 1);
            exit(1); // Encerra o programa em caso de erro
        }
    }
}

void freeFitas(Fita *fitas)
{
    for (int i = 0; i < NUM_2FFITAS; i++)
    {
        // Fecha o arquivo da fita, se estiver aberto
        if (fitas[i].arquivo != NULL)
        {
            fclose(fitas[i].arquivo);
            fitas[i].arquivo = NULL;
        }

        // Libera a memória alocada para nItemsBloco
        if (fitas[i].nItemsBloco != NULL)
        {
            free(fitas[i].nItemsBloco);
            fitas[i].nItemsBloco = NULL;
        }
    }
}
