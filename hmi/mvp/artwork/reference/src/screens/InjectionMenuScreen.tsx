import { Activity, Crosshair, StepForward } from 'lucide-react';
import { ScreenFrame } from '../components/ScreenFrame';
import { ActionButton, Card, IconBadge, Text } from '../components/Hmi';

const options = [
  ['Controle por Estimação', Activity],
  ['Controle por Passo', StepForward],
] as const;

export function InjectionMenuScreen() {
  return (
    <ScreenFrame>
      {options.map(([label, icon], i) => (
        <Card key={label} x={259 + i * 554} y={287} w={526} h={360}>
          <IconBadge x={192} y={56} size={140} icon={icon} iconSize={80} shape="rounded" />
          <Text x={0} y={240} width={524} align="center" text={label} size={36} weight={700} />
        </Card>
      ))}

      <ActionButton x={260} y={718} w={1078} h={96} tone="navy" label="CALIBRAR" icon={Crosshair} iconSize={42} fontSize={32} />
    </ScreenFrame>
  );
}
