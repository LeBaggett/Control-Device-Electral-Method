# ADATA HMI Renderer

Renderizador estático de telas HMI em React/CSS/SVG. A tela-base tem proporção 5:3 (1600×960), equivalente à proporção 800×480 do DOPSoft. O exportador usa `deviceScaleFactor`, então as coordenadas permanecem fixas enquanto o PNG pode sair em alta resolução.

## Telas incluídas

- `autotest`
- `menu`
- `manual-motion`

## Debian / Ubuntu

```bash
sudo apt update
sudo apt install -y nodejs npm
npm install
npx playwright install --with-deps chromium
```

Para desenvolvimento:

```bash
npm run dev
```

Abra, por exemplo:

```text
http://localhost:5173/?screen=autotest
http://localhost:5173/?screen=menu
http://localhost:5173/?screen=manual-motion
```

## Exportar PNG

2× (3200×1920):

```bash
npm run export -- autotest --scale 2
```

1× (1600×960):

```bash
npm run export -- menu --scale 1
```

Arquivo customizado:

```bash
npm run export -- manual-motion --scale 2
npm run export -- config-menu --scale 2 --out output/movimentacao.png
```

O DOPSoft pode redimensionar o PNG para 800×480 mantendo a proporção. Para preservar melhor os detalhes, normalmente vale exportar em 2×.

## Estrutura

```text
src/components/ScreenFrame.tsx       moldura padrão + header + fundo
src/components/EngineeringBackground.tsx
src/components/Hmi.tsx               widgets reutilizáveis
src/screens/*.tsx                    composição de cada tela
src/theme.ts                         cores e dimensões globais
scripts/export.mjs                   exportação PNG via Playwright
public/assets/header-standard.png    faixa superior padronizada
```

## Coordenadas

Os componentes aceitam coordenadas absolutas:

```tsx
<DisplayField x={430} y={360} w={165} h={74} />
<ActionButton x={599} y={31} w={137} h={98} icon={ArrowUp} />
```

Assim, o layout é determinístico. Para mover ou redimensionar algo basta alterar `x`, `y`, `w` e `h`.

## Criar uma nova tela

```tsx
import { ScreenFrame } from '../components/ScreenFrame';
import { Card, DisplayField, Text } from '../components/Hmi';

export function MinhaTela() {
  return (
    <ScreenFrame>
      <Text x={500} y={170} text="Minha Tela" size={52} weight={800} />
      <Card x={300} y={280} w={1000} h={300}>
        <DisplayField x={40} y={80} w={300} h={70} />
      </Card>
    </ScreenFrame>
  );
}
```

Depois registre a tela em `src/App.tsx`.


## Tela adicional: menu de configuração

Use `?screen=config-menu` para visualizar o submenu com `Deslocamento`, `Temperatura` e `Balança`.

## Telas adicionais: configuração de Temperatura e Balança

- `?screen=temperature` — leitura, status de aquecimento e setpoint para Êmbolo, Placa Superior e Placa Inferior.
- `?screen=balance` — leitura em mV, tara, peso e fator de calibração para Placa Superior e Placa Inferior.

## Tela adicional: gerenciar lotes

`?screen=lots` — lista fixa de 4 lotes (código do material, identificador, quantidade, data/hora, num. do lote, num. do pacote) com botões de editar e excluir.

## Tela adicional: parâmetros de movimento

`?screen=motion-params` — velocidade/aceleração JOG, vel./acc. injeção e deslocamento, cada um com valor, unidade e barra, com botões salvar e voltar.

## Tela adicional: login

`?screen=login` — título Spiral Flow, campos de usuário e senha, botões de entrar e menu principal.

## Tela adicional: gerenciar ensaio

`?screen=test-list` — lista fixa de 4 ensaios (material, temperaturas, tempo, data/hora, pressão, quantidade) com botões de executar, editar (abre `test-config`) e excluir.

## Tela adicional: configurar ensaio

`?screen=test-config` — temperatura, pressão e tempo de cura, com botão de salvar.

## Tela adicional: monitoramento do teste

`?screen=test-monitor` — gráficos de temperatura e pressão (eixos com espaço para a escala), mensagem, barra de progresso e botões de reiniciar/confirmar. Sem textos: os textos são adicionados no DOPSoft.
