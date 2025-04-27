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

5. E por fim, é possível executar todas as versões com:
   
./run.sh nosync

./run.sh sem     --prod 3 --cons 2 --buffer 10 --runtime 15

./run.sh mutex   --prod 3 --cons 2 --buffer 10 --runtime 15

./run.sh monitor --prod 3 --cons 2 --buffer 10 --runtime 15
