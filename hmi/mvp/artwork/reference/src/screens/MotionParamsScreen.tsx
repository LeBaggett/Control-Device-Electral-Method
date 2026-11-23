import { ArrowLeft, ArrowLeftRight, Box as BoxIcon, Cog, Gauge, Save, Syringe, Timer } from 'lucide-react';
import { ScreenFrame } from '../components/ScreenFrame';
import { ActionButton, Card, DisplayField, IconBadge, Text, Track } from '../components/Hmi';

const params = [
  ['Velocidade JOG', Gauge, 'mm/s'],
  ['Tempo de Subida', Timer, 's'],
  ['Deslocamento', ArrowLeftRight, 'mm'],
  ['Aceleração JOG', Cog, 'mm/s²'],
  ['Acc. Injeção', Syringe, 'mm/s²'],
  ['Molde', BoxIcon, 'mm/s²'],
] as const;

export function MotionParamsScreen() {
  return (
    <ScreenFrame>
      {params.map(([label, icon, unit], i) => (
        <Card key={label} x={47 + (i % 3) * 510} y={167 + Math.floor(i / 3) * 342} w={486} h={318}>
          <IconBadge x={24} y={24} size={72} icon={icon} iconSize={40} />
          <Text x={112} y={36} text={label} size={26} weight={700} />
          <Text x={300} y={78} width={160} align="right" text={unit} size={22} weight={700} />
          <DisplayField x={24} y={116} w={436} h={92} />
          <Track x={24} y={236} w={436} h={48} />
        </Card>
      ))}

      <ActionButton x={48} y={850} w={740} h={84} label="SALVAR" icon={Save} iconSize={38} fontSize={30} />
      <ActionButton x={812} y={850} w={740} h={84} tone="navy" label="VOLTAR" icon={ArrowLeft} iconSize={38} fontSize={30} />
    </ScreenFrame>
  );
}
