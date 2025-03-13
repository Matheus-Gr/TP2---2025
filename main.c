#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "intercalacao_2f.h"
#include "intercalacao_f1.h"
#include "tipos.h"
#include "quicksort_externo.h"
#include <locale.h>

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Uso: ordena <método> <quantidade> <situação> [-P]\n");
        return 1;
    }

    int metodo = atoi(argv[1]);
    int quantidade = atoi(argv[2]);
    int situacao = atoi(argv[3]);
    int exibirResultado = (argc > 4 && strcmp(argv[4], "-P") == 0) ? 1 : 0;

    const char* arquivoEntrada = "";

    switch (situacao) {
        case 1:
            arquivoEntrada = "arquivos/aleatorio.bin";
            break;
        case 2:
            arquivoEntrada = "arquivos/crescente.bin";
            break;
        case 3:
            arquivoEntrada = "arquivos/decrescente.bin";
            break;
        default:
            printf("Situação inválida!\n");
            return 1;
    }

    Estatisticas estatisticas;
    inicializarEstatisticas(&estatisticas);
    estatisticas.quantidade = quantidade;

    switch (metodo) {
        case 1:
            printf("Metodo 1 - Intercalacao de 2 fitas\n");
            intercalacao_2f(arquivoEntrada, quantidade, exibirResultado, &estatisticas);
            break;
        case 2:
            printf("Metodo 2 - Intercalacao f+1\n");
            intercalacao_f1(arquivoEntrada, quantidade, exibirResultado, &estatisticas);
            break;
        case 3:
            printf("Metodo 3 - Quicksort Externo\n");
            quicksort_externo(arquivoEntrada, quantidade, exibirResultado, &estatisticas);
            break;
        default:
            printf("Método inválido!\n");
            return 1;
    }

    finalizarEstatisticas(&estatisticas);
    mostrarEstatisticas(&estatisticas);
    return 0;
}
