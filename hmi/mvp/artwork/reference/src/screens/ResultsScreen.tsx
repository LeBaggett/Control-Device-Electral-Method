import { ArrowLeft, Award, Hourglass, Ruler, Timer } from 'lucide-react';
import { ScreenFrame } from '../components/ScreenFrame';
import { ActionButton, Card, DisplayField, IconBadge, Text } from '../components/Hmi';

const results = [
  ['Classificação', Award],
  ['Comprimento da espiral (mm)', Ruler],
  ['Tempo de gel (s)', Timer],
  ['Tempo morto (s)', Hourglass],
] as const;

export function ResultsScreen() {
  return (
    <ScreenFrame>
      <IconBadge x={48} y={162} size={64} icon={Ruler} iconSize={38} shape="rounded" />
      <Text x={132} y={171} text="Finalização e Resultados" size={44} weight={800} />

      {results.map(([label, icon], i) => (
        <Card key={label} x={47 + (i % 2) * 766} y={245 + Math.floor(i / 2) * 296} w={740} h={272}>
          <IconBadge x={28} y={28} size={72} icon={icon} iconSize={40} />
          <Text x={120} y={50} text={label} size={30} weight={700} />
          <DisplayField x={28} y={128} w={682} h={114} />
        </Card>
      ))}

      <ActionButton x={48} y={844} w={1504} h={88} tone="navy" label="VOLTAR" icon={ArrowLeft} iconSize={38} fontSize={30} />
    </ScreenFrame>
  );
}
