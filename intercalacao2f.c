#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>
#include <float.h>
#include "tipos.h"

// Fase 1: Inicialização das fitas
static void inicializarFitas(Fita *fitas, int numFitas)
{
    for (int i = 0; i < numFitas; i++)
    {
        sprintf(fitas[i].nome, "fitas/fita%d.bin", i + 1);
        fitas[i].arquivo = NULL;
        fitas[i].ativo = 1;
    }
}

static void abrirFitasParaEscrita(Fita *fitas, int inicio, int fim)
{
    for (int i = inicio; i < fim; i++)
    {
        fitas[i].arquivo = fopen(fitas[i].nome, "wb");
    }
}

static void abrirFitasParaLeitura(Fita *fitas, int inicio, int fim)
{
    for (int i = inicio; i < fim; i++)
    {
        if (fitas[i].arquivo)
            fclose(fitas[i].arquivo);
        fitas[i].arquivo = fopen(fitas[i].nome, "rb");
    }
}

static void fecharFitas(Fita *fitas, int inicio, int fim)
{
    for (int i = inicio; i < fim; i++)
    {
        if (fitas[i].arquivo)
        {
            fclose(fitas[i].arquivo);
            fitas[i].arquivo = NULL;
        }
    }
}

static int encontrarMenorNota(Registro *buffer, int *congelados, int tam)
{
    float menorNota = FLT_MAX;
    int indiceMenor = -1;

    for (int i = 0; i < tam; i++)
    {
        if (!congelados[i] && buffer[i].nota < menorNota)
        {
            menorNota = buffer[i].nota;
            indiceMenor = i;
        }
    }

    return indiceMenor;
}

// Fase 2: Geração dos blocos ordenados usando seleção por substituição
static void gerarBlocosOrdenados(const char *arquivo_entrada, Fita *fitas, int quantidade, Estatisticas *est)
{
    FILE *entrada = fopen(arquivo_entrada, "rb");
    if (!entrada)
    {
        printf("Erro ao abrir arquivo de entrada: %s", arquivo_entrada);
        return;
    }

    // Abrir fitas de saída (0 a 9)
    abrirFitasParaEscrita(fitas, 0, NUM_FITAS / 2);

    // Verificar se as fitas foram abertas corretamente
    for (int i = 0; i < NUM_FITAS / 2; i++)
    {
        if (!fitas[i].arquivo)
        {
            printf("Erro ao abrir fita %d para escrita", i);
            fclose(entrada);
            return;
        }
    }

    Registro buffer[TAM_BUFFER];
    int congelados[TAM_BUFFER] = {0};
    int fitaAtual = 0;
    float ultimaNota = -1;
    int registrosLidos = 0;
    int registrosProcessados = 0;

    // Carregar buffer inicial
    for (int i = 0; i < TAM_BUFFER && registrosLidos < quantidade; i++)
    {
        if (fread(&buffer[i], sizeof(Registro), 1, entrada))
        {
            est->numLeituras++;
            registrosLidos++;
        }
    }

    while (registrosProcessados < quantidade)
    {
        int indiceMenor = encontrarMenorNota(buffer, congelados, TAM_BUFFER);

        // Se todos estão congelados ou buffer vazio
        if (indiceMenor == -1)
        {
            // Mudar para próxima fita
            fitaAtual = (fitaAtual + 1) % (NUM_FITAS / 2);
            memset(congelados, 0, sizeof(congelados));
            ultimaNota = -1;
            continue;
        }

        // Escrever na fita atual
        if (fwrite(&buffer[indiceMenor], sizeof(Registro), 1, fitas[fitaAtual].arquivo) != 1)
        {
            printf("Erro ao escrever na fita %d", fitaAtual);
            break;
        }
        est->numEscritas++;
        registrosProcessados++;

        // Tentar ler próximo registro
        if (registrosLidos < quantidade)
        {
            if (fread(&buffer[indiceMenor], sizeof(Registro), 1, entrada))
            {
                est->numLeituras++;
                registrosLidos++;
                if (buffer[indiceMenor].nota < ultimaNota)
                {
                    congelados[indiceMenor] = 1;
                }
            }
            else
            {
                congelados[indiceMenor] = 1;
            }
        }
        else
        {
            congelados[indiceMenor] = 1;
        }

        if (buffer[indiceMenor].nota >= ultimaNota)
        {
            ultimaNota = buffer[indiceMenor].nota;
        }
    }

    // Garantir que todos os dados foram escritos
    for (int i = 0; i < NUM_FITAS / 2; i++)
    {
        fflush(fitas[i].arquivo);
    }

    fecharFitas(fitas, 0, NUM_FITAS / 2);
    fclose(entrada);

    printf("Blocos gerados: %d registros processados", registrosProcessados);
}

static int lerProximoRegistro(FILE *arquivo, Registro *reg, Estatisticas *est)
{
    if (fread(reg, sizeof(Registro), 1, arquivo))
    {
        est->numLeituras++;
        return 1;
    }
    return 0;
}

// Fase 3: Intercalação dos blocos
static void intercalarBlocos(Fita *fitas, const char *arquivo_saida, int quantidade, Estatisticas *est)
{
    int fitasEntrada = 0;
    int fitasSaida = NUM_FITAS / 2;
    int numPassadas = 0;

    while (1)
    {
        // Abrir fitas para leitura e escrita
        abrirFitasParaLeitura(fitas, fitasEntrada, fitasEntrada + NUM_FITAS / 2);
        abrirFitasParaEscrita(fitas, fitasSaida, fitasSaida + NUM_FITAS / 2);

        FILE *saida = fitas[fitasSaida].arquivo;
        Registro registros[NUM_FITAS / 2];
        int fitasAtivas = 0;

        // Ler primeiro registro de cada fita
        for (int i = fitasEntrada; i < fitasEntrada + NUM_FITAS / 2; i++)
        {
            if (lerProximoRegistro(fitas[i].arquivo, &registros[i - fitasEntrada], est))
            {
                fitasAtivas++;
            }
        }

        // Intercalar registros
        while (fitasAtivas > 0)
        {
            // Encontrar menor registro
            float menorNota = FLT_MAX;
            int fitaMenor = -1;

            for (int i = 0; i < NUM_FITAS / 2; i++)
            {
                if (fitas[i + fitasEntrada].arquivo && registros[i].nota < menorNota)
                {
                    menorNota = registros[i].nota;
                    fitaMenor = i;
                }
            }

            if (fitaMenor == -1)
                break;

            // Escrever menor registro
            fwrite(&registros[fitaMenor], sizeof(Registro), 1, saida);
            est->numEscritas++;

            // Ler próximo registro da fita
            if (!lerProximoRegistro(fitas[fitaMenor + fitasEntrada].arquivo,
                                    &registros[fitaMenor], est))
            {
                fitasAtivas--;
            }
        }

        fecharFitas(fitas, fitasEntrada, fitasEntrada + NUM_FITAS / 2);
        fecharFitas(fitas, fitasSaida, fitasSaida + NUM_FITAS / 2);

        // Trocar fitas de entrada e saída
        int temp = fitasEntrada;
        fitasEntrada = fitasSaida;
        fitasSaida = temp;

        numPassadas++;
        if (numPassadas >= 2)
            break; // Ajuste conforme necessário
    }

    // Renomear última fita para arquivo de saída final
    rename(fitas[fitasEntrada].nome, arquivo_saida);
}

void intercalacao_2f(const char *nomeArquivo, int quantidade, int exibirResultado, Estatisticas *est)
{

    // Inicializar fitas
    Fita fitas[NUM_FITAS];
    inicializarFitas(fitas, NUM_FITAS);

    // Gerar blocos ordenados
    gerarBlocosOrdenados(nomeArquivo, fitas, quantidade, est);

    // Intercalar blocos
    char arquivo_final[100];
    sprintf(arquivo_final, "fitas/ordenado.bin");
    intercalarBlocos(fitas, arquivo_final, quantidade, est);

    // Limpar arquivos temporários
    for (int i = 0; i < NUM_FITAS; i++)
    {
        if (strcmp(fitas[i].nome, arquivo_final) != 0)
        {
            // remove(fitas[i].nome);
        }
    }
}
