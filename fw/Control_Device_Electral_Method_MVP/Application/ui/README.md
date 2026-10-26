# Modelo da interface local

Apresentação e adaptação dos estados/comandos à IHM industrial. Não é um driver OLED nem um projeto DOPSoft.

Estado: estrutura inicial criada em 26/10/2026.

A DOP-107BV será configurada em `hmi/mvp`. Esta camada deve transformar os estados da aplicação em dados da interface e validar o encaminhamento de comandos. A captura TSP não pode depender da atualização de uma tela.

## Manutenção

Implementações futuras devem manter as responsabilidades desta camada e atualizar este README com os arquivos efetivamente criados. Arquivos de outras gerações não devem ser copiados sem revisão de interfaces e premissas.
