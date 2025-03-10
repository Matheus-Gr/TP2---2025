#ifndef INTERCALACAO_2F_H
#define INTERCALACAO_2F_H

#include "tipos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

void intercalacao_2f(const char* arquivoEntrada, int quantidade, int exibirResultado, Estatisticas* stats);
void intercalarBlocos(Fitas* fitas, Estatisticas* stats);
void gerarBlocosOrdenados(const char* arquivoEntrada, int quantidade, Fitas* fitas, Estatisticas* stats);
int compararRegistros(const void* a, const void* b);
void escreverRegistro(FILE* arquivo, Registro reg);
Registro lerRegistro(FILE* arquivo);
void printResultadoOrdenacao();

#endif