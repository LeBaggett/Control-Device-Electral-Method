# Aplicação

Código próprio do dispositivo, separado do código gerado pelo CubeMX. Mantém a divisão board/drivers/core/interfaces/ui da POC.

Estado: estrutura inicial criada em 26/10/2026.

## Arquivos previstos

- `app.c/.h`: orquestração, estado da sessão e despacho de comandos.
- `config.h`: limites do protocolo e parâmetros compartilhados, depois de dimensionado o hardware.
- `board/pinout.h`: mapeamento físico central, sem replicar pinos nos drivers.
- `board/runtime_config.h`: ajustes de integração e recursos habilitados.

Esses arquivos serão implementados nas próximas etapas; esta estrutura não usa valores fictícios de corrente ou GPIO.

## Subpastas

- [board](board/README.md)
- [core](core/README.md)
- [docs](docs/README.md)
- [drivers](drivers/README.md)
- [interfaces](interfaces/README.md)
- [tests](tests/README.md)
- [ui](ui/README.md)

## Manutenção

Implementações futuras devem manter as responsabilidades desta camada e atualizar este README com os arquivos efetivamente criados. Arquivos de outras gerações não devem ser copiados sem revisão de interfaces e premissas.
