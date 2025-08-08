import os

# MAX_ALUNOS 471705
BASE = "arquivos.exe"
OPCAO = "gerar"
REGISTROS = 471705
SITUACAO = 3

FORCAR_GERAR = True


def compilar():
    print("[PYTHON] -> Compilando arquivos.c...")
    if os.system(f"gcc -o {BASE} *c -Wall") == 0:
        print("[PYTHON] -> Compilação bem-sucedida!")
    else:
        print("[PYTHON] -> Erro na compilação.")
        exit(1)


def executar():
    comando = f"{BASE} {OPCAO} {REGISTROS}"

    if OPCAO == "ler":
        if FORCAR_GERAR:
            print(f"[PYTHON] -> Executando: {BASE} gerar {REGISTROS}")
            os.system(f"{BASE} gerar {REGISTROS}")
        comando += f" {SITUACAO}"

    print(f"[PYTHON] -> Executando: {comando}")
    os.system(comando)


if __name__ == "__main__":
    compilar()
    executar()
