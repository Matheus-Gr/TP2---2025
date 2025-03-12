// tipos.h
#ifndef TIPOS_H
#define TIPOS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>
#include <stdbool.h>

#define MEMORIA_INTERNA 10
#define NUM_2FFITAS 20

typedef struct
{
    char inscricao[9];
    float nota;
    char estado[3];
    char cidade[51];
    char curso[31];
} Registro;

typedef enum
{
    ENTRADA,
    SAIDA
} TipoFita;
typedef struct
{
    TipoFita tipo;
    FILE *arquivo;
    int nBlocos;
    int *nItemsBloco;
} Fita;

typedef struct
{
    bool x;
    Registro registro;
} Item;

typedef struct
{
    int numLeituras;
    int numEscritas;
    int numComparacoes;
    clock_t inicioTempo;  // Tempo inicial
    double tempoExecucao; // Tempo total de execução
} Estatisticas;

typedef struct
{
    int qtdItensLidos; // Quantidade de itens lidos do bloco atual
    Registro dadoLido; // Último registro lido da fita
    bool fitaAtiva;    // Indica se a fita ainda tem dados para processar
} Intercalacao;

void inicializarEstatisticas(Estatisticas *stats);
void finalizarEstatisticas(Estatisticas *stats);
void mostrarEstatisticas(Estatisticas *stats);
void inicializar2FFitas(Fita *fitas);
void freeFitas(Fita *fitas);

#endif