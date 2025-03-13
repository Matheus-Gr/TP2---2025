import subprocess
import pandas as pd
import os
from datetime import datetime
import re

def run_sorting(method, quantity, situation, num_execucoes=20):
    print(f"\nExecutando: Método {method}, Quantidade {quantity}, Situação {situation} ({num_execucoes} execuções)")
    
    stats_acumuladas = {
        'quantidade': quantity,
        'comparacoes': 0,
        'movimentacoes': 0,
        'transferencias': 0,
        'tempo': 0.0,
        'QLGB': 0,
        'QEGB': 0,
        'QLT': 0,
        'QET': 0,
        'QCN': 0
    }
    
    for execucao in range(num_execucoes):
        print(f"\nExecução {execucao + 1}/{num_execucoes}")
        comando_base = 'ordenar.exe'
        comando = [comando_base, str(method), str(quantity), str(situation)]
        
        print(f"Executando comando: {' '.join(comando)}")
        # Use universal_newlines=True for text output and encoding specification
        result = subprocess.run(comando, capture_output=True, text=True, encoding='utf-8')
        
        print(f"Código de retorno: {result.returncode}")
        if result.stderr:
            print(f"Erro encontrado: {result.stderr}")
        
        output = result.stdout
        print(f"Saída do programa:\n{output}")
        
        stats = {
            'quantidade': quantity,
            'comparacoes': 0,
            'movimentacoes': 0,
            'transferencias': 0,
            'tempo': 0.0,
            'QLGB': 0,
            'QEGB': 0,
            'QLT': 0,
            'QET': 0,
            'QCN': 0
        }
        
        # Updated regex patterns to properly handle mangled encodings
        patterns = {
            'comparacoes': r'Quantidade de comparações entre notas \(Q\.C\.N\):\s*(\d+)',  # Matches "Quantidade de comparações entre notas (Q.C.N)"
            'movimentacoes': r'Quantidade de movimentações:\s*(\d+)',  # Matches "Quantidade de movimentações"
            'transferencias': r'Quantidade de transferências \(Q\.L\.T \+ Q\.E\.T\):\s*(\d+)',  # Matches "Quantidade de transferências (Q.L.T + Q.E.T)"
            'tempo': r'Tempo de execução \(T\.EXC\):\s*([\d.]+)',  # Matches "Tempo de execução (T.EXC)"
            'quantidade': r'Quantidade de registros:\s*(\d+)', # Matches "Quantidade de registros"
            'QLGB': r'Quantidade de leitura na geração de blocos \(Q\.L\.G\.B\):\s*(\d+)',
            'QEGB': r'Quantidade de escrita na geração de blocos \(Q\.E\.G\.B\):\s*(\d+)',
            'QLT': r'Quantidade de leituras totais \(Q\.L\.T\):\s*(\d+)',
            'QET': r'Quantidade de escritas totais \(Q\.E\.T\):\s*(\d+)',
            'QCN': r'Quantidade de comparações entre notas \(Q\.C\.N\):\s*(\d+)'
        }
        
        print("\nExtraindo estatísticas...")
        for stat, pattern in patterns.items():
            match = re.search(pattern, output, re.IGNORECASE)  # Added case insensitive flag
            if match:
                value = float(match.group(1)) if stat == 'tempo' else int(match.group(1))
                stats[stat] = value
                print(f"Encontrado {stat}: {value}")
            else:
                print(f"AVISO: Não foi possível encontrar {stat} no texto")
                print(f"Texto procurado com pattern: {pattern}")
                print(f"Saída completa do programa:\n{output}")  # Print the full output for debugging
        
        print(f"Estatisticas lidas do terminal: {stats}") # Adicionado para debug
        
        # Acumula as estatísticas
        for stat in stats_acumuladas:
            stats_acumuladas[stat] += stats[stat]
    
    # Calcula a média das estatísticas
    for stat in stats_acumuladas:
        stats_acumuladas[stat] /= num_execucoes
    
    return stats_acumuladas

def create_excel_tables():
    print("\nIniciando criação das tabelas...")
    methods = [2, 3]  # intercalacao_f1, quicksort_externo
    quantities = [100, 200, 2000, 20000, 200000, 471705]
    situations = [1, 2, 3]  # aleatorio, crescente, decrescente
    
    method_names = {
        1: "Intercalação 2f",
        2: "Intercalação f+1",
        3: "Quicksort Externo"
    }
    
    situation_names = {
        1: "Aleatório",
        2: "Crescente",
        3: "Decrescente"
    }
    
    # Create DataFrames for each metric with normalized names
    results = {
        'Comparações': pd.DataFrame(),
        'Movimentações': pd.DataFrame(),
        'Transferências': pd.DataFrame(),
        'Tempo': pd.DataFrame(),
        'QLGB': pd.DataFrame(),
        'QEGB': pd.DataFrame(),
        'QLT': pd.DataFrame(),
        'QET': pd.DataFrame(),
        'QCN': pd.DataFrame()
    }
    
    metric_mapping = {
        'Comparações': 'comparacoes',
        'Movimentações': 'movimentacoes',
        'Transferências': 'transferencias',
        'Tempo': 'tempo',
        'QLGB': 'QLGB',
        'QEGB': 'QEGB',
        'QLT': 'QLT',
        'QET': 'QET',
        'QCN': 'QCN'
    }
    
    for method in methods:
        for situation in situations:
            data = []
            for quantity in quantities:
                stats = run_sorting(method, quantity, situation)
                data.append(stats)
            
            # Create column name
            col_name = f"{method_names[method]} - {situation_names[situation]}"
            
            # Add data to each DataFrame
            for sheet_name, metric_key in metric_mapping.items():
                results[sheet_name][col_name] = [d[metric_key] for d in data]
    
    # Set index for all DataFrames
    for df in results.values():
        df.index = quantities
        df.index.name = 'Quantidade'
    
    # Create Excel writer
    timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
    excel_file = f'resultados_{timestamp}.xlsx'
    
    with pd.ExcelWriter(excel_file, engine='xlsxwriter') as writer:
        # Write each metric to a different sheet
        for sheet_name, df in results.items():
            df.to_excel(writer, sheet_name=sheet_name)
        
        # Auto-adjust columns width
        for sheet_name in writer.sheets:
            worksheet = writer.sheets[sheet_name]
            for idx, col in enumerate(results[sheet_name].columns):
                series = results[sheet_name][col]
                max_len = max(
                    series.astype(str).map(len).max(),
                    len(str(col))
                ) + 2
                worksheet.set_column(idx + 1, idx + 1, max_len)

if __name__ == '__main__':
    print("Iniciando processo de geração de tabelas...")
    print("\nVerificando existência do compilador GCC...")
    gcc_check = subprocess.run(['gcc', '--version'], capture_output=True, text=True)
    if gcc_check.returncode != 0:
        print("ERRO: GCC não encontrado no sistema!")
        exit(1)
    
    print("\nCompilando o programa...")
    compile_command = ['gcc', 'main.c', 'intercalacao_2f.c', 'intercalacao_f1.c', 
                      'quicksort_externo.c', 'tipos.c', '-o', 'ordenar.exe', '-lm']
    print(f"Comando de compilação: {' '.join(compile_command)}")
    
    compile_result = subprocess.run(compile_command, capture_output=True, text=True)
    if compile_result.returncode != 0:
        print("ERRO na compilação:")
        print(compile_result.stderr)
        exit(1)
    print("Compilação bem sucedida!")
    
    print("\nVerificando existência do executável...")
    if not os.path.exists('ordenar.exe'):
        print("ERRO: Executável não foi gerado!")
        exit(1)
    
    print("\nGerando tabelas...")
    try:
        create_excel_tables()
        print("Tabelas geradas com sucesso!")
    except Exception as e:
        print(f"ERRO durante a geração das tabelas: {str(e)}")
