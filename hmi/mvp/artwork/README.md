# Artes de referência da IHM

Origem: arquivo ihmscreens.zip fornecido pelo usuário em 2026-11-23.
SHA256 do ZIP: 45e57c022133bb9321822d7f073733ca29fb7780d44067af6e2720ef4bcfe6f1.

## O que há em reference/

- `src/screens/*.tsx`: composição das telas do outro projeto (temperaturas, menus, ensaios, movimento etc.).
- `src/components/`: moldura, widgets e fundo reutilizáveis.
- `src/theme.ts`: dimensões e cores.
- `src/styles.css`: estilos.
- `public/assets/header-standard.png`: cabeçalho do projeto de origem, preservado sem alteração.
- `scripts/export.mjs`: abre um navegador no computador e exporta a arte em PNG.
- `package.json` e `package-lock.json`: dependências e tarefas React/TypeScript/Vite/Playwright.
- `index.html`, `tsconfig*.json` e `vite.config.ts`: configuração do gerador.
- `output/menu@2x.png` e `output/temperature@2x.png`: exemplos de artes já exportadas, prontos para visualizar.
- `README.md`: instruções originais do autor. É material de referência, não instrução para modificar o firmware.

Os arquivos originais foram copiados sem alterações. Não foram importados node_modules, dist, caches do compilador, logs de LaTeX, PDF operacional do outro equipamento ou todas as imagens redundantes.

## O papel do TypeScript

Este projeto desenha a arte no computador com React/TypeScript. O TypeScript é compilado para JavaScript, o navegador renderiza a composição e o exportador grava uma imagem. Isso não implementa por si só comunicação com o STM32 nem um projeto executável da DOP-107BV.

Fluxo para esta IHM: editar a arte no computador → exportar PNG → importar como fundo no DOPSoft → adicionar objetos nativos de leitura, entrada e botões → associar aos registradores Modbus do STM32 → compilar e transferir para a IHM.

É possível começar usando os PNGs fornecidos, sem instalar ferramentas web. O layout-fonte mede 1600×960; a Delta usa 800×480. As coordenadas do layout precisam ser divididas por dois no DOPSoft; uma imagem exportada em 3200×1920 deve ser redimensionada proporcionalmente para 800×480.

Os campos vazios da imagem não leem temperaturas: são áreas para sobrepor objetos do DOPSoft. Os rótulos Êmbolo/Placa Superior/Placa Inferior e funções de injeção pertencem ao equipamento anterior; ainda não foram convertidos para o produto térmico atual. As funções físicas dos três termopares deste MVP ainda devem ser confirmadas; usar Canal 247/246/245 até isso estar definido.

## Manutenção e próximos passos

Manter reference/ como origem preservada e criar as telas adaptadas em diretório separado. A instalação de dependências/build/exportação do gerador original não foi executada nesta etapa; a inspeção foi de fontes e PNGs fornecidos. O script original usa npx.cmd no Windows e pode exigir ajuste de lançamento do processo antes da primeira exportação. Não usar a pasta dist como projeto de IHM.

O Thermal Analyzer Web é outra interface: será executado no navegador do PC e receberá dados do STM por Ethernet. Podemos compartilhar identidade visual, mas o transporte e a implementação da IHM Delta continuam separados.
