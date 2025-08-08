#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include "tipos.h"
#include "intercalacao2f.h"
#include "main.h"

int main(int argc, char *argv[])
{
    if (argc < 4)
    {
        printf("Uso: ordena <metodo> <quantidade> <situacao> [-P]\n");
        return 1;
    }

    int metodo = atoi(argv[1]);
    int quantidade = atoi(argv[2]);
    int situacao = atoi(argv[3]);
    int exibirResultado = (argc > 4 && strcmp(argv[4], "-P") == 0) ? 1 : 0;

    const char *arquivoEntrada = "";

    switch (situacao)
    {
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
        printf("Situacao invalida!\n");
        return 1;
    }

    Estatisticas estatisticas;
    inicializarEstatisticas(&estatisticas);

    switch (metodo)
    {
    case 1:
        printf("Metodo 1 - Intercalacao de 2 fitas\n");
        intercalacao_2f(arquivoEntrada, quantidade, exibirResultado,
                        &estatisticas);
        finalizarEstatisticas(&estatisticas);
        mostrarEstatisticas(&estatisticas);

        if (exibirResultado)
            lerOrdenado();

        break;
    case 2:
        printf("Metodo 2!\n");
        // Initialize statistics
        inicializarEstatisticas(&estatisticas);
        // Call the method (replace with actual method call)
        // metodo2(arquivoEntrada, quantidade, exibirResultado, &estatisticas);
        // Finalize and display statistics
        finalizarEstatisticas(&estatisticas);
        mostrarEstatisticas(&estatisticas);
        break;
    case 3:
        printf("Metodo 3!\n");
        // Initialize statistics
        inicializarEstatisticas(&estatisticas);
        // Call the method (replace with actual method call)
        // metodo3(arquivoEntrada, quantidade, exibirResultado, &estatisticas);
        // Finalize and display statistics
        finalizarEstatisticas(&estatisticas);
        mostrarEstatisticas(&estatisticas);
        break;
    default:
        printf("Metodo invalido!\n");
        return 1;
    }

    return 0;
}

void lerOrdenado()
{
    FILE *arquivo = fopen("fitas/ordenado.bin", "rb");
    if (arquivo)
    {
        Registro reg;
        while (fread(&reg, sizeof(Registro), 1, arquivo))
        {
            printf("%ld %.1f %s %s %s\n", reg.inscricao, reg.nota, reg.estado, reg.cidade, reg.curso);
        }
        fclose(arquivo);
    }
}
