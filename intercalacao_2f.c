#include "tipos.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#define NUM_FITAS 20  // 10 fitas de entrada + 10 fitas de saída
#define MEMORIA_INTERNA 10  // Capacidade máxima da memória interna


// Função para ler um registro do arquivo binário
Registro lerRegistro(FILE* arquivo) {
    Registro reg;
    if (fread(&reg, sizeof(Registro), 1, arquivo) != 1) {
        reg.inscricao[0] = '\0'; // Registro inválido
        reg.nota = -1;
    }
    return reg;
}

// Função para escrever um registro no arquivo binário
void escreverRegistro(FILE* arquivo, Registro reg) {
    fwrite(&reg, sizeof(Registro), 1, arquivo);
}

// Função de comparação para qsort (ordena por nota)
int compararRegistros(const void* a, const void* b) {
    Registro* regA = (Registro*)a;
    Registro* regB = (Registro*)b;
    if (regA->nota < regB->nota) return -1;
    if (regA->nota > regB->nota) return 1;
    return 0;
}

// Função para gerar blocos ordenados nas fitas de entrada
void gerarBlocosOrdenados(const char* arquivoEntrada, int quantidade, Fitas* fitas, Estatisticas* stats) {
    FILE* entrada = fopen(arquivoEntrada, "rb");
    if (!entrada) {
        perror("Erro ao abrir arquivo de entrada");
        exit(1);
    }

    MemoriaInterna memoria;
    memoria.tamanho = 0;
    int fitaAtual = 0; // Alterna entre as 10 fitas de entrada

    while (quantidade > 0) {
        while (memoria.tamanho < MEMORIA_INTERNA && quantidade > 0) {
            Registro reg = lerRegistro(entrada);
            if (reg.nota == -1) break; // Fim do arquivo
            memoria.registros[memoria.tamanho++] = reg;
            quantidade--;
            stats->numLeituras++;
        }

        // Ordenar registros na memória
        qsort(memoria.registros, memoria.tamanho, sizeof(Registro), compararRegistros);

        // Escrever o bloco ordenado na fita de saída atual
        for (int i = 0; i < memoria.tamanho; i++) {
            escreverRegistro(fitas->fitas[fitaAtual], memoria.registros[i]);
            stats->numEscritas++;
        }

        fitaAtual = (fitaAtual + 1) % (NUM_FITAS / 2); // Alterna entre as 10 fitas de entrada
        memoria.tamanho = 0; // Limpa a memória interna
    }

    fclose(entrada);
}

// Função para intercalar blocos ordenados entre as fitas de entrada e saída
void intercalarBlocos(Fitas* fitas, Estatisticas* stats) {
    int numFitasEntrada = NUM_FITAS / 2; // 10 fitas de entrada
    int numFitasSaida = NUM_FITAS / 2;   // 10 fitas de saída

    while (1) {
        int todasVazias = 1;

        // Alternar entre fitas de entrada e saída
        for (int i = 0; i < numFitasEntrada; i++) {
            FILE* fitaEntrada1 = fitas->fitas[i];
            FILE* fitaEntrada2 = fitas->fitas[i + numFitasEntrada];
            FILE* fitaSaida = fitas->fitas[i % numFitasSaida];

            // Reposicionar fitas de entrada para o início
            fseek(fitaEntrada1, 0, SEEK_SET);
            fseek(fitaEntrada2, 0, SEEK_SET);

            // Limpar fita de saída
            fclose(fitaSaida);
            fitaSaida = fopen("fitas/fita_saida_temp.bin", "w+b");
            if (!fitaSaida) {
                perror("Erro ao limpar fita de saída");
                exit(1);
            }

            Registro reg1 = lerRegistro(fitaEntrada1);
            Registro reg2 = lerRegistro(fitaEntrada2);

            while (reg1.nota != -1 || reg2.nota != -1) {
                if (reg1.nota != -1 && (reg2.nota == -1 || reg1.nota <= reg2.nota)) {
                    escreverRegistro(fitaSaida, reg1);
                    reg1 = lerRegistro(fitaEntrada1);
                } else {
                    escreverRegistro(fitaSaida, reg2);
                    reg2 = lerRegistro(fitaEntrada2);
                }
                stats->numLeituras++;
                stats->numEscritas++;
                stats->numComparacoes++;
            }

            fseek(fitaSaida, 0, SEEK_END);
            if (ftell(fitaSaida) > 0) todasVazias = 0;

            fclose(fitaSaida);
        }

        if (todasVazias) break; // Se todas as fitas de entrada estiverem vazias, parar a intercalação
    }
}

void printResultadoOrdenacao() {
    FILE* fitaSaida = fopen("fitas/fita0.bin", "rb");
    if (!fitaSaida) {
        perror("Erro ao abrir fita de saida");
        return;
    }

    printf("\nResultado da Ordenacao:\n");
    printf("------------------------------------------------------------\n");
    printf("Inscricao | Nota  | Estado | Cidade                      | Curso\n");
    printf("------------------------------------------------------------\n");

    Registro reg;
    while (1) {
        reg = lerRegistro(fitaSaida);
        if (reg.nota == -1) break;
        printf("%-9s | %5.2f | %-6s | %-26s | %-30s\n",
               reg.inscricao, reg.nota, reg.estado, reg.cidade, reg.curso);
    }

    printf("------------------------------------------------------------\n");
    fclose(fitaSaida);
}

// Função principal para intercalação balanceada de dois caminhos (2f)
void intercalacao_2f(const char* arquivoEntrada, int quantidade, int exibirResultado, Estatisticas* stats) {
    Fitas fitas;
    fitas.numFitasUsadas = 0;

    // Inicializar as 20 fitas
    for (int i = 0; i < NUM_FITAS; i++) {
        char nomeFita[50];
        sprintf(nomeFita, "fitas/fita%d.bin", i);
        fitas.fitas[i] = fopen(nomeFita, "w+b");
        if (!fitas.fitas[i]) {
            perror("Erro ao criar fita");
            exit(1);
        }
    }
    printf("FITAS INICIADAS\n");

    // Gerar blocos ordenados nas fitas de entrada
    gerarBlocosOrdenados(arquivoEntrada, quantidade, &fitas, stats);
    printf("GERACAO DE BLOCOS CONCLUIDA\n");

    // Intercalar os blocos ordenados até obter o arquivo final ordenado
    intercalarBlocos(&fitas, stats);
    printf("INTERCALACAO DE BLOCOS CONCLUIDA\n");

    // Fechar fitas
    for (int i = 0; i < NUM_FITAS; i++) {
        fclose(fitas.fitas[i]);
    }

    // Exibir o resultado final, se solicitado
    if (exibirResultado) {
        printResultadoOrdenacao();
    }
}
