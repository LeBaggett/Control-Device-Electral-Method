import { Gauge, MoveHorizontal, OctagonX } from 'lucide-react';
import { ScreenFrame } from '../components/ScreenFrame';
import { ActionButton, Card, ChartAxes, IconBadge, Text, Track } from '../components/Hmi';
const charts = [
  ['Pressão (kgf/cm²)', Gauge, [0, 20, 40, 60, 80, 100]],
  ['Deslocamento LVDT (mm)', MoveHorizontal, [0, 5, 10, 15, 20, 25, 30, 35]],
] as const;
export function ExecutionScreen() {
  return <ScreenFrame>
    <IconBadge x={48} y={162} size={64} icon={Gauge} shape="rounded" />
    <Text x={132} y={171} text="Execução do Ensaio" size={44} weight={800} />
    {charts.map(([label, icon, scale], i) => <Card key={label} x={47 + i * 766} y={246} w={740} h={562}>
      <IconBadge x={24} y={16} size={48} icon={icon} iconSize={28} shape="rounded" />
      <Text x={90} y={24} text={label} size={26} />
      <ChartAxes x={20} y={78} w={700} h={462} scale={scale} />
    </Card>)}
    <Card x={47} y={828} w={1120} h={100}>
      <Text x={24} y={10} text="Progresso do ensaio" size={22} />
      <Track x={24} y={48} w={1072} h={30} />
    </Card>
    <ActionButton x={1188} y={828} w={365} h={100} label="Interromper" tone="red" icon={OctagonX} iconSize={34} fontSize={26} />
  </ScreenFrame>;
}
