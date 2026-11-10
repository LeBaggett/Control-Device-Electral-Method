# Página embarcada

- `index.html`: interface e JavaScript, incluindo Ethernet e N1040i com explicações de falhas observadas. O navegador executa o JavaScript; não depende de CDN/internet.
- `index_html.h`: resposta HTTP em bytes gerada por `tools/embed_web.py`. Atualizar pelo script e recompilar após alterar HTML.
- `web_files.c`: integra `/`, `/index.html`, `/api/status` e `/api/n1040` ao HTTPD LwIP. Cada resposta JSON recebe um snapshot próprio, liberado no fechamento da conexão.

Dados inválidos/antigos não são apresentados como temperatura atual. Perda de comunicação com a placa limpa os cartões de medição e sinaliza indisponibilidade. A API e a página são somente leitura.
