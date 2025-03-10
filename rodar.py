import subprocess
import os

comando_base = 'ordenar.exe'

metodo = str(1)  # Método 1 (intercalacao_2f)
quantidade = str(100)  # Quantidade de registros
situacao = str(1)  # Situação (1, 2 ou 3)
imprimir = '-P'  # Parâmetro opcional para imprimir resultados

comando_compilacao = [
    'gcc',
    'main.c',
    'intercalacao_2f.c',
    '-o', comando_base,
    '-lm',
    '-Wall'
]

print(f"Compilando com o comando: {' '.join(comando_compilacao)}")
resultado_compilacao = subprocess.run(comando_compilacao)

if resultado_compilacao.returncode != 0:
    print("Erro na compilação. Verifique os arquivos .c.")
    exit(1)

comando_execucao = [comando_base, metodo, quantidade, situacao]
if imprimir:
    comando_execucao.append(imprimir)

print(f"Executando com o comando: {' '.join(comando_execucao)}")
resultado_execucao = subprocess.run(comando_execucao)

if resultado_execucao.returncode != 0:
    print("Erro na execução. Verifique os parâmetros.")
    exit(1)

# print(f"Removendo o executável: {comando_base}")
# os.remove(comando_base)
