#ifndef TIPOS_H
#define TIPOS_H

#include <stdio.h>
#include <time.h>

#define TAM_BUFFER 10
#define NUM_FITAS 20

// Add enum definition
typedef enum
{
    ENTRADA,
    SAIDA
} TipoFita;

typedef struct
{
    long int inscricao;
    float nota;
    char estado[3];
    char cidade[51];
    char curso[31];
} Registro;

typedef struct
{
    long numComparacoes;
    long numLeituras;
    long numEscritas;
    clock_t tempoInicio;
    double tempoExecucao;
} Estatisticas;

typedef struct
{
    FILE *arquivo;
    char nome[20];
    int ativo;
} Fita;

// Funções de estatísticas
void inicializarEstatisticas(Estatisticas *est);
void finalizarEstatisticas(Estatisticas *est);
void mostrarEstatisticas(Estatisticas *est);

// Add function declarations to fix implicit declarations
void intercalacao_2f(const char *nomeArquivo, int quantidade, int exibirResultado, Estatisticas *est);

#endif
