import { MoveVertical, Scale, Syringe, Thermometer } from 'lucide-react';
import { ScreenFrame } from '../components/ScreenFrame';
import { Card, IconBadge, Text } from '../components/Hmi';

const rows = [
  ['Deslocamento', MoveVertical],
  ['Temperatura', Thermometer],
  ['Balança', Scale],
  ['Injeção', Syringe],
] as const;

export function ConfigMenuScreen() {
  return (
    <ScreenFrame>
      {rows.map(([label, icon], i) => {
        const y = 182 + i * 196;
        return (
          <Card key={label} x={310} y={y} w={980} h={150}>
            <IconBadge x={32} y={24} size={102} icon={icon} iconSize={59} shape="rounded" />
            <Text x={173} y={51} text={label} size={46} weight={700} />
          </Card>
        );
      })}
    </ScreenFrame>
  );
}
