import { MoveVertical, Thermometer } from 'lucide-react';
import { ScreenFrame } from '../components/ScreenFrame';
import { Card, IconBadge, Text } from '../components/Hmi';

const rows = [
  ['Deslocamento', MoveVertical],
  ['Temperatura', Thermometer],
] as const;

export function ConfigMenu2Screen() {
  return (
    <ScreenFrame>
      {rows.map(([label, icon], i) => {
        const y = 332 + i * 248;
        return (
          <Card key={label} x={310} y={y} w={980} h={190}>
            <IconBadge x={40} y={30} size={130} icon={icon} iconSize={75} shape="rounded" />
            <Text x={219} y={62} text={label} size={58} weight={700} />
          </Card>
        );
      })}
    </ScreenFrame>
  );
}
