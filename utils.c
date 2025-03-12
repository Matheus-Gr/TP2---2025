#include "utils.h"

FILE *inicializarArquivo(const char *nomeArquivo, const char *modo)
{
    FILE *arquivo = fopen(nomeArquivo, modo);
    if (!arquivo)
    {
        perror("Erro ao abrir arquivo");
        exit(1); // Encerra o programa em caso de erro
    }
    return arquivo;
}