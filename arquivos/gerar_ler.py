import os

# Definição dos parâmetros
OPCAO = "ler"  # Opções: "gerar" ou "ler"
ARQUIVO = "decrescente.bin"  # Apenas necessário se OPCAO for "ler"


def compilar():
    print("Compilando arquivos.c...")
    if os.system("gcc -o gerar.exe ./arquivos.c") == 0:
        print("Compilação bem-sucedida!")
    else:
        print("Erro na compilação.")
        exit(1)


def executar():
    comando = f"gerar.exe {OPCAO}"
    if OPCAO == "ler":
        comando += f" {ARQUIVO}"
    print(f"Executando: {comando}")
    os.system(comando)


if __name__ == "__main__":
    compilar()
    executar()
