import { Gauge, MoveHorizontal, OctagonX, Route, Weight } from 'lucide-react';
import type { LucideIcon } from 'lucide-react';
import { ScreenFrame } from '../components/ScreenFrame';
import { ActionButton, Card, DisplayField, IconBadge, Text, Track } from '../components/Hmi';

function CardTitle({ icon, title, unit, w }: { icon: LucideIcon; title: string; unit?: string; w: number }) {
  return (
    <>
      <IconBadge x={20} y={18} size={56} icon={icon} iconSize={32} />
      <Text x={90} y={32} text={title} size={26} weight={700} />
      {unit ? <Text x={w - 162} y={36} width={140} align="right" text={unit} size={22} weight={700} /> : null}
    </>
  );
}

export function InjectionCalibrationScreen() {
  return (
    <ScreenFrame>
      <IconBadge x={48} y={162} size={64} icon={Gauge} iconSize={38} shape="rounded" />
      <Text x={132} y={174} text="Autoteste" size={38} weight={800} />
      <Text x={900} y={182} text="Etapa atual" size={22} weight={700} />
      <DisplayField x={1036} y={164} w={516} h={60} />

      <Card x={47} y={243} w={600} h={360}>
        <CardTitle icon={Gauge} title="Pressão" unit="kgf/cm²" w={600} />
        <Text x={20} y={100} width={270} align="center" text="Atual" size={20} weight={600} />
        <DisplayField x={20} y={136} w={270} h={160} />
        <Text x={308} y={100} width={270} align="center" text="Alvo" size={20} weight={600} />
        <DisplayField x={308} y={136} w={270} h={160} />
      </Card>

      <Card x={663} y={243} w={436} h={360}>
        <CardTitle icon={Weight} title="Força" unit="kgf" w={436} />
        <DisplayField x={20} y={136} w={394} h={160} />
      </Card>

      <Card x={1115} y={243} w={438} h={360}>
        <CardTitle icon={MoveHorizontal} title="Posição LVDT" unit="mm" w={438} />
        <DisplayField x={20} y={136} w={396} h={160} />
      </Card>

      <Card x={47} y={621} w={1506} h={140}>
        <CardTitle icon={Route} title="Progresso da etapa" w={1506} />
        <Track x={20} y={88} w={1464} h={34} />
      </Card>

      <ActionButton x={48} y={780} w={1504} h={120} tone="red" label="INTERROMPER" icon={OctagonX} iconSize={46} fontSize={36} />
    </ScreenFrame>
  );
}
