#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_REGISTROS 10
#define NUM_FITAS 10

typedef struct {
    long inscricao;
    float nota;
    char estado[3];
    char cidade[51];
    char curso[31];
} Registro;

int comparar(const void *a, const void *b) {
    Registro *regA = (Registro *)a;
    Registro *regB = (Registro *)b;
    return (regA->nota > regB->nota) - (regA->nota < regB->nota);
}

void gerar_blocos(FILE *arquivo, FILE *fitas[]) {
    Registro buffer[MAX_REGISTROS];
    int i = 0, fita_atual = 0;

    while (fscanf(arquivo, "%ld %f %2s %50s %30s", &buffer[i].inscricao, &buffer[i].nota, buffer[i].estado, buffer[i].cidade, buffer[i].curso) == 5) {
        i++;
        if (i == MAX_REGISTROS) {
            qsort(buffer, MAX_REGISTROS, sizeof(Registro), comparar);
            for (int j = 0; j < MAX_REGISTROS; j++) {
                fwrite(&buffer[j], sizeof(Registro), 1, fitas[fita_atual]);
            }
            fita_atual = (fita_atual + 1) % NUM_FITAS;
            i = 0;
        }
    }

    if (i > 0) {
        qsort(buffer, i, sizeof(Registro), comparar);
        for (int j = 0; j < i; j++) {
            fwrite(&buffer[j], sizeof(Registro), 1, fitas[fita_atual]);
        }
    }
}

void intercalar_fitas(FILE *fitasEntrada[], FILE *fitasSaida[]) {
    Registro registros[NUM_FITAS];
    int ativo[NUM_FITAS];
    for (int i = 0; i < NUM_FITAS; i++) {
        if (fread(&registros[i], sizeof(Registro), 1, fitasEntrada[i])) {
            ativo[i] = 1;
        } else {
            ativo[i] = 0;
        }
    }

    int fitaSaidaAtual = 0;
    while (1) {
        int menorIdx = -1;
        for (int i = 0; i < NUM_FITAS; i++) {
            if (ativo[i] && (menorIdx == -1 || comparar(&registros[i], &registros[menorIdx]) < 0)) {
                menorIdx = i;
            }
        }
        if (menorIdx == -1) break;
        fwrite(&registros[menorIdx], sizeof(Registro), 1, fitasSaida[fitaSaidaAtual]);
        if (!fread(&registros[menorIdx], sizeof(Registro), 1, fitasEntrada[menorIdx])) {
            ativo[menorIdx] = 0;
        }
        fitaSaidaAtual = (fitaSaidaAtual + 1) % NUM_FITAS;
    }
}

void salvar_ordenado(FILE *fitaFinal) {
    FILE *arquivoOrdenado = fopen("PROVAO_ORDENADO.TXT", "w");
    if (!arquivoOrdenado) {
        printf("Erro ao criar PROVAO_ORDENADO.TXT\n");
        return;
    }
    Registro reg;
    while (fread(&reg, sizeof(Registro), 1, fitaFinal)) {
        fprintf(arquivoOrdenado, "%ld %.2f %s %s %s\n", reg.inscricao, reg.nota, reg.estado, reg.cidade, reg.curso);
    }
    fclose(arquivoOrdenado);
}

void intercalacao_2f(int quantidade, int situacao) {
    FILE *arquivo = fopen("PROVAO.TXT", "r");
    if (!arquivo) {
        printf("Erro ao abrir PROVAO.TXT\n");
        return;
    }

    FILE *fitasEntrada[NUM_FITAS], *fitasSaida[NUM_FITAS];
    for (int i = 0; i < NUM_FITAS; i++) {
        char nome[20];
        sprintf(nome, "fita%d.bin", i);
        fitasEntrada[i] = fopen(nome, "wb+");
        fitasSaida[i] = fopen(nome, "wb+");
    }

    gerar_blocos(arquivo, fitasEntrada);
    fclose(arquivo);

    int numPassos = 0;
    while (numPassos < 10) {
        intercalar_fitas(fitasEntrada, fitasSaida);
        FILE *temp[NUM_FITAS];
        for (int i = 0; i < NUM_FITAS; i++) {
            temp[i] = fitasEntrada[i];
            fitasEntrada[i] = fitasSaida[i];
            fitasSaida[i] = temp[i];
        }
        numPassos++;
    }

    salvar_ordenado(fitasEntrada[0]);

    for (int i = 0; i < NUM_FITAS; i++) {
        fclose(fitasEntrada[i]);
        fclose(fitasSaida[i]);
    }
    printf("Ordenação concluída!\n");