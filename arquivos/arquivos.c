#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../tipos.h"

#define MAX_ALUNOS 471705

int comparar_crescente(const void *a, const void *b) {
    Registro *regA = (Registro *)a;
    Registro *regB = (Registro *)b;
    return (regA->nota > regB->nota) - (regA->nota < regB->nota);
}

int comparar_decrescente(const void *a, const void *b) {
    return comparar_crescente(b, a);
}

void remover_espacos(char *str) {
    int i, j = 0;
    while (str[j] == ' ') j++; // Ignora espaços iniciais
    for (i = 0; str[j] != '\0'; j++) {
        if (str[j] != ' ' || (i > 0 && str[i - 1] != ' ')) {
            str[i++] = str[j];
        }
    }
    str[i] = '\0'; // Finaliza a string corretamente
}

void salvar_binario(const char *nome_arquivo, Registro *registros, int n) {
    FILE *arquivo = fopen(nome_arquivo, "wb");
    if (!arquivo) {
        printf("Erro ao criar %s\n", nome_arquivo);
        return;
    }
    fwrite(registros, sizeof(Registro), n, arquivo);
    fclose(arquivo);
    printf("Arquivo %s gerado com sucesso!\n", nome_arquivo);
}

void ler_binario(int situacao) {

    const char* nome_arquivo = "";

    switch (situacao) {
        case 1:
            nome_arquivo = "aleatorio.bin";
            break;
        case 2:
            nome_arquivo = "crescente.bin";
            break;
        case 3:
            nome_arquivo = "decrescente.bin";
            break;
        default:
            printf("Situacao invalida!\n");
    }


    FILE *arquivo = fopen(nome_arquivo, "rb");
    if (!arquivo) {
        printf("Erro ao abrir %s\n", nome_arquivo);
        return;
    }
    Registro reg;
    while (fread(&reg, sizeof(Registro), 1, arquivo)) {
        printf("%s %.1f %s %s %s\n", reg.inscricao, reg.nota, reg.estado, reg.cidade, reg.curso);
    }
    fclose(arquivo);
}

void gerar_arquivos() {
    FILE *arquivo = fopen("PROVAO.TXT", "r");
    if (!arquivo) {
        printf("Erro ao abrir PROVAO.TXT\n");
        return;
    }

    Registro *registros = malloc(MAX_ALUNOS * sizeof(Registro));
    if (!registros) {
        printf("Erro ao alocar memória\n");
        fclose(arquivo);
        return;
    }

    int count = 0;
    while (fscanf(arquivo, "%8s %f %2s %50c %30c", registros[count].inscricao, &registros[count].nota,
                  registros[count].estado, registros[count].cidade, registros[count].curso) == 5) {
        registros[count].inscricao[8] = '\0';
        registros[count].estado[2] = '\0';
        registros[count].cidade[50] = '\0';
        registros[count].curso[30] = '\0';
        
        remover_espacos(registros[count].cidade);
        remover_espacos(registros[count].curso);

        count++;
        if (count >= MAX_ALUNOS) break;
    }
    fclose(arquivo);

    salvar_binario("aleatorio.bin", registros, count);
    qsort(registros, count, sizeof(Registro), comparar_crescente);
    salvar_binario("crescente.bin", registros, count);
    qsort(registros, count, sizeof(Registro), comparar_decrescente);
    salvar_binario("decrescente.bin", registros, count);

    free(registros);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Uso: %s <opcao> [arquivo]\n", argv[0]);
        printf("Opcoes:\n");
        printf("  gerar - Gera os arquivos binários a partir do PROVAO.TXT\n");
        printf("  ler <arquivo> - Lê e imprime o conteúdo de um arquivo binário\n");
        return 1;
    }
    
    int situacao = atoi(argv[2]);

    if (strcmp(argv[1], "gerar") == 0) {
        gerar_arquivos();
    } else if (strcmp(argv[1], "ler") == 0 && argc == 3) {
        ler_binario(situacao);
    } else {
        printf("Opção inválida!\n");
    }
    return 0;
}
