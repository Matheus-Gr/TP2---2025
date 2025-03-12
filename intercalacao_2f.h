#ifndef INTERCALACAO_2F_H
#define INTERCALACAO_2F_H

#include "tipos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int compararItens(const void *a, const void *b);

void gerarBlocosOrdenados(const char *arquivoEntrada, int quantidade, Fita *fitas, Estatisticas *stats);

void intercalacao_2f(const char *arquivoEntrada, int quantidade, int exibirResultado, Estatisticas *stats);

#endif