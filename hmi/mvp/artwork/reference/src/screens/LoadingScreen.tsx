import { Box, CircleCheck, OctagonX } from 'lucide-react';
import { ScreenFrame } from '../components/ScreenFrame';
import { ActionButton, Card, IconBadge, Text } from '../components/Hmi';
export function LoadingScreen() {
  return <ScreenFrame>
    <IconBadge x={48} y={162} size={64} icon={Box} shape="rounded" />
    <Text x={132} y={171} text="Carregamento" size={44} weight={800} />
    <Card x={259} y={270} w={1080} h={500}>
      <IconBadge x={480} y={38} size={120} icon={Box} iconSize={68} shape="rounded" />
      <Text x={48} y={204} width={982} align="center" text="Coloque o material e feche a porta" size={36} weight={800} />
      <Text x={48} y={278} width={982} align="center" text="Confirme para continuar o ensaio" size={26} weight={500} />
      <ActionButton x={48} y={360} w={475} h={92} label="Interromper" tone="red" icon={OctagonX} iconSize={36} />
      <ActionButton x={555} y={360} w={475} h={92} label="Confirmar" tone="green" icon={CircleCheck} iconSize={36} />
    </Card>
  </ScreenFrame>;
}
