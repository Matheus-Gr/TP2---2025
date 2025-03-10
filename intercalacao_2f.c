#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tipos.h"

// Definindo o número total de fitas (10 de entrada e 10 de saída)
#define NUM_FITAS_ENTRADA 10
#define NUM_FITAS_SAIDA 10
#define NUM_FITAS_TOTAL (NUM_FITAS_ENTRADA + NUM_FITAS_SAIDA)

int MAX_REGISTROS;  // Tamanho do bloco, deve ser definido dinamicamente

int comparar(const void *a, const void *b) {
    Registro *regA = (Registro *)a;
    Registro *regB = (Registro *)b;
    return (regA->nota > regB->nota) - (regA->nota < regB->nota);
}

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

void intercalacao_2f(const char *arquivoEntrada, int quantidade, int exibirResultado) {
    MAX_REGISTROS = quantidade;  // Definir dinamicamente o tamanho do bloco

    FILE *arquivo = fopen(arquivoEntrada, "rb");
    if (!arquivo) {
        printf("Erro ao abrir %s\n", arquivoEntrada);
        return;
    }

    // Fitas de entrada e saída
    FILE *fitasEntrada[NUM_FITAS_ENTRADA], *fitasSaida[NUM_FITAS_SAIDA];
    for (int i = 0; i < NUM_FITAS_ENTRADA; i++) {
        char nome[20];
        sprintf(nome, "arquivos/fita_entrada%d.bin", i);
        fitasEntrada[i] = fopen(nome, "wb+");
    }
    for (int i = 0; i < NUM_FITAS_SAIDA; i++) {
        char nome[20];
        sprintf(nome, "arquivos/fita_saida%d.bin", i);
        fitasSaida[i] = fopen(nome, "wb+");
    }

    distribuir_blocos(arquivo, fitasEntrada);
    fclose(arquivo);

    int numPassos = 0;
    while (numPassos < 10) {  // Definindo 10 passos de intercalação como exemplo
        intercalar_fitas(fitasEntrada, fitasSaida);
        FILE *temp[NUM_FITAS_ENTRADA];
        for (int i = 0; i < NUM_FITAS_ENTRADA; i++) {
            temp[i] = fitasEntrada[i];
            fitasEntrada[i] = fitasSaida[i];
            fitasSaida[i] = temp[i];
        }
        numPassos++;
    }

    // Copiar os dados da fita de saída final para o arquivo de resultado
    FILE *resultado = fopen("resultado_ordenado.bin", "wb");
    if (!resultado) {
        printf("Erro ao criar arquivo de saida!\n");
        return;
    }
    Registro registro;
    while (fread(&registro, sizeof(Registro), 1, fitasSaida[0])) {
        fwrite(&registro, sizeof(Registro), 1, resultado);
    }
    fclose(resultado);

    printf("Ordenacao concluida! Dados gravados em resultado_ordenado.bin\n");

    // Exibir os resultados na tela se a flag -P for passada
    if (exibirResultado) {
        rewind(resultado);  // Voltar ao início do arquivo de saída
        while (fread(&registro, sizeof(Registro), 1, resultado)) {
            printf("Inscricao: %s, Nota: %.2f\n", registro.inscricao, registro.nota);
        }
    }

    for (int i = 0; i < NUM_FITAS_ENTRADA; i++) {
        fclose(fitasEntrada[i]);
    }
    for (int i = 0; i < NUM_FITAS_SAIDA; i++) {
        fclose(fitasSaida[i]);
    }
}
