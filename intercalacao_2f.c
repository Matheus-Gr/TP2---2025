#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tipos.h"

// Definindo o número total de fitas (10 de entrada e 10 de saída)
#define NUM_FITAS_ENTRADA 10
#define NUM_FITAS_SAIDA 10
#define NUM_FITAS_TOTAL (NUM_FITAS_ENTRADA + NUM_FITAS_SAIDA)

int MAX_REGISTROS;  // Tamanho do bloco, deve ser definido dinamicamente

// Função de comparação
int comparar(const void *a, const void *b) {
    Registro *regA = (Registro *)a;
    Registro *regB = (Registro *)b;
    return (regA->nota > regB->nota) - (regA->nota < regB->nota);
}

// Função para distribuir blocos nas fitas
void distribuir_blocos(FILE *arquivo, FILE *fitas[]) {
    Registro *buffer = (Registro *)malloc(MAX_REGISTROS * sizeof(Registro));  // Usando memória dinâmica
    int i = 0, fita_atual = 0;

    while (fread(&buffer[i], sizeof(Registro), 1, arquivo)) {
        i++;
        if (i == MAX_REGISTROS) {
            qsort(buffer, MAX_REGISTROS, sizeof(Registro), comparar);
            fwrite(buffer, sizeof(Registro), MAX_REGISTROS, fitas[fita_atual]);
            fita_atual = (fita_atual + 1) % NUM_FITAS_ENTRADA;  // Usar apenas as fitas de entrada
            i = 0;
        }
    }

    if (i > 0) {
        qsort(buffer, i, sizeof(Registro), comparar);
        fwrite(buffer, sizeof(Registro), i, fitas[fita_atual]);
    }

    free(buffer);  // Liberar memória alocada
}

// Função para intercalar fitas
void intercalar_fitas(FILE *fitasEntrada[], FILE *fitasSaida[]) {
    Registro registros[NUM_FITAS_ENTRADA];
    int ativo[NUM_FITAS_ENTRADA];
    for (int i = 0; i < NUM_FITAS_ENTRADA; i++) {
        if (fread(&registros[i], sizeof(Registro), 1, fitasEntrada[i])) {
            ativo[i] = 1;
        } else {
            ativo[i] = 0;
        }
    }

    int fitaSaidaAtual = 0;
    while (1) {
        int menorIdx = -1;
        for (int i = 0; i < NUM_FITAS_ENTRADA; i++) {
            if (ativo[i] && (menorIdx == -1 || comparar(&registros[i], &registros[menorIdx]) < 0)) {
                menorIdx = i;
            }
        }
        if (menorIdx == -1) break;
        fwrite(&registros[menorIdx], sizeof(Registro), 1, fitasSaida[fitaSaidaAtual]);
        if (!fread(&registros[menorIdx], sizeof(Registro), 1, fitasEntrada[menorIdx])) {
            ativo[menorIdx] = 0;
        }
        fitaSaidaAtual = (fitaSaidaAtual + 1) % NUM_FITAS_SAIDA;  // Usar apenas as fitas de saída
    }
}

// Função principal de ordenação externa
void intercalacao_2f(const char *arquivoEntrada, int quantidade, int exibirResultado) {
    // Definir o tamanho do bloco com base na quantidade de registros
    MAX_REGISTROS = quantidade;

    FILE *arquivo = fopen(arquivoEntrada, "rb");
    if (!arquivo) {
        printf("Erro ao abrir %s\n", arquivoEntrada);
        return;
    }

    FILE *fitasEntrada[NUM_FITAS_TOTAL], *fitasSaida[NUM_FITAS_TOTAL];
    for (int i = 0; i < NUM_FITAS_TOTAL; i++) {
        char nome[20];
        sprintf(nome, "fitas/fita%d.bin", i + 1);
        fitasEntrada[i] = fopen(nome, "wb+");
        fitasSaida[i] = fopen(nome, "wb+");
    }

    distribuir_blocos(arquivo, fitasEntrada);
    fclose(arquivo);

    int numPassos = 0;
    while (numPassos < 10) {
        intercalar_fitas(fitasEntrada, fitasSaida);
        FILE *temp[NUM_FITAS_TOTAL];
        for (int i = 0; i < NUM_FITAS_TOTAL; i++) {
            temp[i] = fitasEntrada[i];
            fitasEntrada[i] = fitasSaida[i];
            fitasSaida[i] = temp[i];
        }
        numPassos++;
    }

    if (exibirResultado == 1) {
        // Exibir resultados da ordenação
        printf("Resultado da ordenacao:\n");
        Registro registro;
        // Leia os registros da última fita de saída
        FILE *fitaResultado = fitasSaida[0];  // Considerando que a fita 0 contém o resultado
        while (fread(&registro, sizeof(Registro), 1, fitaResultado)) {
            printf("Inscricao: %s, Nota: %.2f\n", registro.inscricao, registro.nota);
        }
    }

    printf("Ordenacao concluida!\n");

    for (int i = 0; i < NUM_FITAS_TOTAL; i++) {
        fclose(fitasEntrada[i]);
        fclose(fitasSaida[i]);
    }
}
