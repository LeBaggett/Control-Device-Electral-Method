# Testes no computador

- `run_tests.py`: usa GCC host e o `fs.c` real do LwIP; verifica respostas HTTP, alocação, JSON e invariantes Ethernet/potência/cache. A serial usa stub neste teste.
- `run_n1040_tests.py`: compila os arquivos reais `modbus_read.c` e `n1040.c` com transporte simulado. Testa CRC conhecido, tamanho/endereço/função, complemento de dois, PV/status/ID, °F, exceções, timeout, erro UART, dados antigos, wrap e JSON limitado. Executa com 1 e 3 nós e verifica sintaxe JavaScript via Node.

Executar ambos na raiz da bancada, com Python 3, GCC host e Node disponíveis. Eles não validam eletricamente a UART, o DMA real, a direção RS485, o termopar ou a Ethernet física. Consulte o roteiro em ../../INTEGRACAO_N1040I.md.
