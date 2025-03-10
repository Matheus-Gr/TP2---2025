#include <stdio.h>
#include <stdlib.h>
#include <string.h>  // Adicionar esta linha no início
#include "intercalacao_2f.h"

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

    switch (metodo) {
        case 1:
            printf("Metodo 1 - Intercalacao de 2 fitas\n");
            intercalacao_2f(arquivoEntrada, quantidade, exibirResultado);
            break;
        case 2:
            printf("Método 2!\n");
            break;
        case 3:
            printf("Método 3!\n");
            break;
        default:
            printf("Método inválido!\n");
            return 1;
    }

    return 0;
}
