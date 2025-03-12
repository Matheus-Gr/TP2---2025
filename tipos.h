// tipos.h
#ifndef TIPOS_H
#define TIPOS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <time.h>
#include <locale.h>

#define MEMORIA_INTERNA 10
#define NUM_FITAS 20

typedef struct {
    char inscricao[9];
    float nota;
    char estado[3];
    char cidade[51];
    char curso[31];
} Registro;

typedef struct {
    Registro registros[MEMORIA_INTERNA];
    int tamanho;
} MemoriaInterna;

typedef struct {
    FILE* fitas[NUM_FITAS];
    int numFitasUsadas;
} Fitas;

typedef struct {
    int comparacoes;
    int transferencias;
    int quantidade;
    double tempoGasto;
} Estatisticas;

void inicializarEstatisticas(Estatisticas* stats);
void finalizarEstatisticas(Estatisticas* stats);
void mostrarEstatisticas(Estatisticas* stats);

#endif