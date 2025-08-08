import subprocess
import os

BASE = 'ordenar.exe'

METODO = 1
QUANTIDADE = 100
SITUACAO = 1
IMPRIMIR = '-P'


def compilar():
    print("[PYTHON] -> Compilando arquivos...")
    if os.system(f"gcc -o {BASE} *c -Wall") == 0:
        print("[PYTHON] -> Compilação bem-sucedida!")
    else:
        print("[PYTHON] -> Erro na compilação.")
        exit(1)


def executar():
    comando = f"{BASE} {METODO} {QUANTIDADE} {SITUACAO} {IMPRIMIR}"

    print(f"[PYTHON] -> Executando: {comando}")
    os.system(comando)


if __name__ == "__main__":
    compilar()
    executar()
