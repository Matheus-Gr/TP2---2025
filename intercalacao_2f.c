#include "tipos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "utils.h"

#define NUM_FITAS 20       // 10 fitas de entrada + 10 fitas de saída
#define MEMORIA_INTERNA 10 // Capacidade máxima da memória interna

int compararItens(const void *a, const void *b)
{
    Item *itemA = (Item *)a;
    Item *itemB = (Item *)b;

    if (itemA->registro.nota < itemB->registro.nota)
        return -1;
    if (itemA->registro.nota > itemB->registro.nota)
        return 1;
    return 0;
}

void gerarBlocosOrdenados(const char *arquivoEntrada, int quantidade, Fita *fitas, Estatisticas *stats)
{
    FILE *entrada = inicializarArquivo(arquivoEntrada, "rb");
    Item blocoSubstituicao[MEMORIA_INTERNA]; // Memória interna para armazenar registros

    int qtdInicialParaLer;
    int qtdItensQueFaltam = quantidade;

    int fitaAtual = 0; // Índice da fita de entrada atual

    // Lê os registros do arquivo de entrada em blocos de MEMORIA_INTERNA
    while (qtdItensQueFaltam > 0)
    {
        qtdInicialParaLer = qtdItensQueFaltam >= MEMORIA_INTERNA ? MEMORIA_INTERNA : qtdItensQueFaltam;
        qtdItensQueFaltam -= qtdInicialParaLer;

        // Lê os registros do arquivo de entrada
        fread(blocoSubstituicao, sizeof(Item), qtdInicialParaLer, entrada);
        stats->numLeituras++;

        // Ordena os registros em memória interna usando qsort
        qsort(blocoSubstituicao, qtdInicialParaLer, sizeof(Item), compararItens);
        stats->numComparacoes++; // Estimativa de comparações

        // Escreve o bloco ordenado na fita de entrada atual
        fwrite(blocoSubstituicao, sizeof(Item), qtdInicialParaLer, fitas[fitaAtual].arquivo);
        stats->numEscritas++;

        // Atualiza o número de blocos e itens na fita atual
        fitas[fitaAtual].nBlocos++;
        fitas[fitaAtual].nItemsBloco = realloc(fitas[fitaAtual].nItemsBloco, fitas[fitaAtual].nBlocos * sizeof(int));
        fitas[fitaAtual].nItemsBloco[fitas[fitaAtual].nBlocos - 1] = qtdInicialParaLer;

        // Alterna para a próxima fita de entrada
        fitaAtual = (fitaAtual + 1) % 10; // 10 fitas de entrada
    }

    fclose(entrada);
}

void rewindFitas(Fita *fitas)
{
    for (int i = 0; i < NUM_2FFITAS; i++)
        rewind(fitas[i].arquivo);
}

// Função para intercalar os blocos das fitas de entrada e gerar o arquivo final ordenado
void intercalarBlocos(Fita *fitas, Estatisticas *stats)
{
    enum TipoFita tipoFitaLeitura = ENTRADA;
    int entrada = 0, saida = 10; // 10 fitas de entrada e 10 de saída

    // Marca qual será a fita de saída que receberá o bloco resultante da passada atual da intercalação
    int fitaSaida, passada;

    // Verificando a quantidade total de blocos gerados inicialmente
    int qtdBlocos = 0;
    for (int i = entrada; i < entrada + 10; i++)
    {
        qtdBlocos += fitas[i].nBlocos;
    }

    // Executando até que tenhamos somente um bloco
    while (qtdBlocos > 1)
    {
        // Reinicia as fitas para o início
        for (int i = 0; i < NUM_FITAS; i++)
        {
            rewind(fitas[i].arquivo);
        }

        // Verificando a quantidade de fitas com blocos que irão participar da passada inicial
        int qtdFitas = 0;
        for (int i = entrada; i < entrada + 10; i++)
        {
            if (fitas[i].nBlocos > 0)
            {
                qtdFitas++;
            }
        }

        passada = 1;

        // Executa todas as passadas de intercalação nas fitas de entrada
        while (qtdFitas > 0)
        {
            // Gerando os registros que terão os dados da intercalação
            Intercalacao *dadosIntercalacao = (Intercalacao *)malloc(qtdFitas * sizeof(Intercalacao));
            for (int i = 0; i < qtdFitas; i++)
            {
                dadosIntercalacao[i].qtdItensLidos = 0;
                dadosIntercalacao[i].fitaAtiva = true;
            }

            // Calculando qual será a fita de saída, sendo que ela pode ocupar as dez e retornar para a primeira
            fitaSaida = ((saida + passada - 1) % 10) + saida;

            // Lendo o primeiro registro de cada bloco
            for (int i = 0; i < qtdFitas; i++)
            {
                fread(&dadosIntercalacao[i].dadoLido, sizeof(Registro), 1, fitas[entrada + i].arquivo);
                dadosIntercalacao[i].qtdItensLidos++;
                stats->numLeituras++;
            }

            int posicaoMenorNota;
            int qtdDadosEscritos = 0;

            // Representa 1 passada
            // Lendo todos os dados e escrevendo o que tiver a menor nota
            while (true)
            {
                // Verifica se todos os dados dos blocos foram lidos
                bool todosLidos = true;
                for (int i = 0; i < qtdFitas; i++)
                {
                    if (dadosIntercalacao[i].fitaAtiva)
                    {
                        todosLidos = false;
                        break;
                    }
                }
                if (todosLidos)
                    break;

                // Procurando o menor registro entre as fitas ativas
                posicaoMenorNota = -1;
                for (int i = 0; i < qtdFitas; i++)
                {
                    if (dadosIntercalacao[i].fitaAtiva)
                    {
                        if (posicaoMenorNota == -1 || dadosIntercalacao[i].dadoLido.nota < dadosIntercalacao[posicaoMenorNota].dadoLido.nota)
                        {
                            posicaoMenorNota = i;
                        }
                    }
                }
                stats->numComparacoes++;

                // Escrevendo o item de menor nota na fita de saída
                fwrite(&dadosIntercalacao[posicaoMenorNota].dadoLido, sizeof(Registro), 1, fitas[fitaSaida].arquivo);
                qtdDadosEscritos++;
                stats->numEscritas++;

                // Desativando a fita caso todos os seus itens já tenham sido lidos
                if (dadosIntercalacao[posicaoMenorNota].qtdItensLidos == fitas[entrada + posicaoMenorNota].nItemsBloco[passada - 1])
                {
                    dadosIntercalacao[posicaoMenorNota].fitaAtiva = false;
                    // Decrementando o número de blocos daquela fita
                    fitas[entrada + posicaoMenorNota].nBlocos--;
                }
                // Lendo o próximo item da fita onde o registro retirado é proveniente, já que a fita ainda estiver ativa
                else
                {
                    fread(&dadosIntercalacao[posicaoMenorNota].dadoLido, sizeof(Registro), 1, fitas[entrada + posicaoMenorNota].arquivo);
                    dadosIntercalacao[posicaoMenorNota].qtdItensLidos++;
                    stats->numLeituras++;
                }
            }

            passada++;

            // Atualizando o vetor que contém a quantidade de itens em cada bloco
            fitas[fitaSaida].nBlocos++;
            fitas[fitaSaida].nItemsBloco = realloc(fitas[fitaSaida].nItemsBloco, fitas[fitaSaida].nBlocos * sizeof(int));
            fitas[fitaSaida].nItemsBloco[fitas[fitaSaida].nBlocos - 1] = qtdDadosEscritos;

            free(dadosIntercalacao);

            // Verificando a quantidade de fitas com blocos que irão participar da próxima passada
            qtdFitas = 0;
            for (int i = entrada; i < entrada + 10; i++)
            {
                if (fitas[i].nBlocos > 0)
                {
                    qtdFitas++;
                }
            }
        }

        // Se a fita atual for de leitura, a próxima será de saída e vice-versa
        tipoFitaLeitura = tipoFitaLeitura == ENTRADA ? SAIDA : ENTRADA;

        // Definindo quais fitas serão as de entrada e saída
        if (tipoFitaLeitura == ENTRADA)
        {
            entrada = 0;
            saida = 10;
        }
        else
        {
            entrada = 10;
            saida = 0;
        }

        // Calculando a quantidade de blocos
        qtdBlocos = 0;
        for (int i = entrada; i < entrada + 10; i++)
        {
            qtdBlocos += fitas[i].nBlocos;
        }
    }

    // Copia saída final para o arquivo que se quer ordenar
    FILE *arqDestino = fopen("arquivo_ordenado.bin", "wb");
    if (arqDestino == NULL)
    {
        printf("Não foi possível abrir o arquivo onde os dados serão salvos\n");
        return;
    }

    // Lendo 20 dados, no máximo, e já escrevendo no arquivo de saída
    Registro buffer[20];
    size_t bytesLidos;

    stats->numLeituras++;
    while ((bytesLidos = fread(buffer, sizeof(Registro), 20, fitas[fitaSaida].arquivo)) > 0)
    {
        stats->numLeituras++;
        fwrite(buffer, sizeof(Registro), bytesLidos, arqDestino);
        stats->numEscritas++;
    }

    fclose(arqDestino);
}

void intercalacao_2f(const char *arquivoEntrada, int quantidade, int exibirResultado, Estatisticas *stats)
{
    Fita fitas[NUM_2FFITAS];
    inicializar2FFitas(fitas);
    printf("FITAS INICIADAS\n");

    gerarBlocosOrdenados(arquivoEntrada, quantidade, fitas, stats);
    printf("GERACAO DE BLOCOS CONCLUIDA\n");

    rewindFitas(fitas);

    intercalarBlocos(fitas, stats);
    printf("INTERCALACAO DE BLOCOS CONCLUIDA\n");

    freeFitas(fitas);
    printf("FITAS LIBERADAS\n");
}
