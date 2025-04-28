#!/bin/bash

# Configurações
RUNTIME=15
BUFFER_SIZE=10
PRODUCERS=3
CONSUMERS=2

# Função para executar uma versão e extrair métricas
run_version() {
    local version=$1
    echo "Executando $version por $RUNTIME segundos..."
    
    ./prodcons_$version --runtime $RUNTIME --buffer $BUFFER_SIZE --prod $PRODUCERS --cons $CONSUMERS > $version.log 2>&1
    
    # Extrai métricas do log
    throughput_prod=$(grep "Throughput (producao)" $version.log | awk '{print $3}')
    throughput_cons=$(grep "Throughput (consumo)" $version.log | awk '{print $3}')
    latency=$(grep "Latencia media" $version.log | awk '{print $3}')
    buffer_usage=$(grep "Utilizacao do buffer" $version.log | awk '{print $4}' | tr -d '%')

    
    printf "| %-8s | %8.2f | %8.2f | %8.2f | %8.2f%% |\n" \
        $version $throughput_prod $throughput_cons $latency $buffer_usage
}

# Cabeçalho da tabela
echo "+----------+----------+----------+----------+----------+"
echo "| Versão   | Prod/s   | Cons/s   | Latência | Buffer   |"
echo "+----------+----------+----------+----------+----------+"

# Executa todas as versões
run_version nosync
run_version sem
run_version mutex
run_version monitor

# Rodapé da tabela
echo "+----------+----------+----------+----------+----------+"