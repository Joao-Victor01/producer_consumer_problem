-- Projeto The Producer-Consumer Problem

Este repositório contém diversas implementações do problema Produtor-Consumidor em C, permitindo comparar abordagens de sincronização diferentes (sem sincronização, semáforos+mutex, mutex+condição e monitor simulado) e coletar métricas de desempenho.

-- Do que se trata o projeto

O objetivo é demonstrar e comparar quatro versões de uma solução para o problema clássico do produtor-consumidor:

Versão nosync: sem nenhum mecanismo de sincronização.

Versão sem: usando semáforos e mutex para controlar acesso ao buffer.

Versão mutex: usando mutex e variáveis de condição POSIX.

Versão monitor: uso de um monitor simulado com mutex e variáveis de condição internas.

Cada versão executa produtores e consumidores que interagem com um buffer circular, registra métricas (throughput de produção/consumo, latência média e utilização do buffer) e exibe relatórios ao final.

-- Imagem da interface

Não há interface gráfica neste projeto. A interação é feita via linha de comando e os resultados são exibidos no terminal.

-- Como compilar e executar

1. O comando de execução pode variar conforme queira, esses são os significados dos parâmetros:
    3 threads produtoras (--prod 3)

    2 threads consumidoras (--cons 2)

    Tamanho do buffer igual a 10 (--buffer 10)

    Execução total de 20 segundos (--runtime 20)

2. Antes de executar o programa no Ambiente Linux é necessário baixar as bibliotecas essencias para rodar (gcc, make e outros utilitários) 

$ sudo apt update 

$ sudo apt install build essential

3. Agora podemos utilizar do MakeFile para compilar facilmente nosso programa, seguindo os comandos:

make clean (apenas para garantir que não há executáveis no diretório)

make all (monta os executáveis necessários)

4. Utilizamos do script localizado em run.sh para executarmos cada versão do programa, então para habilitar o nosso script no ambiente:

chmod +x run.sh

5. Da mesma forma utilizamos do compare_all.sh para facilmente visualizar e comparar todas as métricas:

chmod +x compare_all.sh, e depois:

./compare_all.sh

6. E por fim, é possível executar todas as versões com:
   
./run.sh nosync --prod 3 --cons 2 --buffer 10 --runtime 15

./run.sh sem     --prod 3 --cons 2 --buffer 10 --runtime 15

./run.sh mutex   --prod 3 --cons 2 --buffer 10 --runtime 15

./run.sh monitor --prod 3 --cons 2 --buffer 10 --runtime 15


-- Quem fez o que?
João Victor Monteiro de Oliveira - Buffer cirular; Versão NoSync, Versão Semáforo
Cássio Vittori - Configurações e ajustes para rodar o projeto em Linux, Implementação e comparação de métricas
Henrique Bandeira - MutexCond, Monitor, Monitor Simulado
