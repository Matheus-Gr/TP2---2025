// tipos.h
#ifndef TIPOS_H
#define TIPOS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <time.h>
#include <locale.h>

#define MEMORIA_INTERNA 100  // Increased for better performance with small datasets
#define SMALL_INPUT_THRESHOLD 500  // Reduced threshold for switching to in-memory sort

#define NUM_FITAS 20
// #define TAMAREA 50  // Size for the area buffer

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
    Registro *registros;
    int tamanho;
} Area;

typedef struct {
    Registro registro;
} Item;

typedef struct {
    int comparacoes;
    int movimentacoes;
    int transferencias;
    int quantidade;
    double tempoGasto;
    int QLGB;
    int QEGB;
    int QLT;
    int QET;
    int QCN;
} Estatisticas;

void inicializarEstatisticas(Estatisticas* stats);
void finalizarEstatisticas(Estatisticas* stats);
void mostrarEstatisticas(Estatisticas* stats);

#endif