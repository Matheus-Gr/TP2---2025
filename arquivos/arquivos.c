#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../tipos.h"

int comparar_crescente(const void *a, const void *b)
{
    const Registro *regA = (const Registro *)a;
    const Registro *regB = (const Registro *)b;

    if (regA->nota < regB->nota)
        return -1;
    else if (regA->nota > regB->nota)
        return 1;
    else
        return 0;
}

int comparar_decrescente(const void *a, const void *b)
{
    return comparar_crescente(b, a);
}

void remover_espacos(char *str)
{
    int i, j = 0;
    while (str[j] == ' ')
        j++; // Ignora espaços iniciais
    for (i = 0; str[j] != '\0'; j++)
    {
        if (str[j] != ' ' || (i > 0 && str[i - 1] != ' '))
        {
            str[i++] = str[j];
        }
    }
    str[i] = '\0'; // Finaliza a string corretamente
}

void limpar_string(char *str)
{
    char *write = str;
    for (char *read = str; *read; read++)
    {
        if (*read >= 32 && *read <= 126)
        { // ASCII printable characters only
            *write = *read;
            write++;
        }
    }
    *write = '\0';
}

void salvar_binario(const char *nome_arquivo, Registro *registros, int n)
{
    FILE *arquivo = fopen(nome_arquivo, "wb");
    if (!arquivo)
    {
        printf("Erro ao criar %s\n", nome_arquivo);
        return;
    }
    fwrite(registros, sizeof(Registro), n, arquivo);
    fclose(arquivo);
    printf("Arquivo %s gerado com sucesso!\n", nome_arquivo);
}

void ler_binario(int max_registros, int situacao)
{

    const char *nome_arquivo = "";

    switch (situacao)
    {
    case 1:
        nome_arquivo = "crescente.bin";
        break;
    case 2:
        nome_arquivo = "decrescente.bin";
        break;
    case 3:
        nome_arquivo = "aleatorio.bin";
        break;
    default:
        printf("Situacao invalida! %d\n", situacao);
    }

    FILE *arquivo = fopen(nome_arquivo, "rb");
    if (!arquivo)
    {
        printf("Erro ao abrir %s\n", nome_arquivo);
        return;
    }
    Registro reg;

    int count = 0;
    printf("Arquivo: %s\n", nome_arquivo);
    while (fread(&reg, sizeof(Registro), 1, arquivo) && count < max_registros)
    {
        printf("Registro %d:\n", count + 1);
        printf("  Inscricao: %ld\n", reg.inscricao);
        printf("  Nota: %.2f\n", reg.nota);
        printf("  Estado: %s\n", reg.estado);
        printf("  Cidade: %s\n", reg.cidade);
        printf("  Curso: %s\n", reg.curso);
        printf("-----------------------------\n");
        count++;
    }
    fclose(arquivo);
}

void gerar_arquivos(int max_registros)
{
    FILE *arquivo = fopen("PROVAO.TXT", "r");
    if (!arquivo)
    {
        printf("Erro ao abrir PROVAO.TXT\n");
        return;
    }

    Registro *registros = malloc(max_registros * sizeof(Registro));
    if (!registros)
    {
        printf("Erro ao alocar memoria\n");
        fclose(arquivo);
        return;
    }

    char linha[200];
    int count = 0;

    while (fgets(linha, sizeof(linha), arquivo) && count < max_registros)
    {
        linha[strcspn(linha, "\r\n")] = '\0';

        char buf_inscricao[9] = {0};
        char buf_nota[6] = {0};
        char buf_estado[3] = {0};
        char buf_cidade[51] = {0};
        char buf_curso[31] = {0};

        strncpy(buf_inscricao, linha, 8);
        strncpy(buf_nota, linha + 9, 5);
        strncpy(buf_estado, linha + 15, 2);
        strncpy(buf_cidade, linha + 18, 50);
        strncpy(buf_curso, linha + 69, 30);

        registros[count].inscricao = atol(buf_inscricao);
        registros[count].nota = atof(buf_nota);
        strcpy(registros[count].estado, buf_estado);
        strcpy(registros[count].cidade, buf_cidade);
        strcpy(registros[count].curso, buf_curso);

        // printf("Registro %d:\n", count);
        // printf("  Inscricao: %ld\n", registros[count].inscricao);
        // printf("  Nota: %.2f\n", registros[count].nota);
        // printf("  Estado: %s\n", registros[count].estado);
        // printf("  Cidade: %s\n", registros[count].cidade);
        // printf("  Curso: %s\n", registros[count].curso);
        // printf("-----------------------------\n");
        count++;
        if (count == max_registros)
        {
            printf("Registro %d:\n", count);
            printf("  Inscricao: %ld\n", registros[count - 1].inscricao);
            printf("  Nota: %.2f\n", registros[count - 1].nota);
            printf("  Estado: %s\n", registros[count - 1].estado);
            printf("  Cidade: %s\n", registros[count - 1].cidade);
            printf("  Curso: %s\n", registros[count - 1].curso);
            printf("-----------------------------\n");
        }
    }
    fclose(arquivo);

    salvar_binario("aleatorio.bin", registros, count);
    qsort(registros, count, sizeof(Registro), comparar_crescente);
    salvar_binario("crescente.bin", registros, count);
    qsort(registros, count, sizeof(Registro), comparar_decrescente);
    salvar_binario("decrescente.bin", registros, count);

    free(registros);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Uso:\n");
        printf(" gerar: %s gerar <max_registros>\n", argv[0]);
        printf(" ler: %s ler <max_registros> <situacao>\n", argv[0]);
        return 1;
    }

    long int max = atoi(argv[2]);
    int situacao = atoi(argv[3]);

    if (strcmp(argv[1], "gerar") == 0)
    {
        gerar_arquivos(max);
    }
    else if (strcmp(argv[1], "ler") == 0 && argc == 4)
    {
        ler_binario(max, situacao);
    }
    else
    {
        printf("Opcao invalida!\n");
    }
    return 0;
}
