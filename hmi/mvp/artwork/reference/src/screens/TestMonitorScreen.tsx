import { CircleCheck, Gauge, Info, RefreshCw, Thermometer } from 'lucide-react';
import { ScreenFrame } from '../components/ScreenFrame';
import { ActionButton, Card, ChartAxes, IconBadge, Track } from '../components/Hmi';

const charts = [Thermometer, Gauge] as const;

export function TestMonitorScreen() {
  return (
    <ScreenFrame>
      {charts.map((icon, i) => (
        <Card key={i} x={47 + i * 766} y={173} w={740} h={470}>
          <IconBadge x={32} y={16} size={56} icon={icon} iconSize={32} />
          <ChartAxes x={20} y={80} w={698} h={368} />
        </Card>
      ))}

      <Card x={47} y={661} w={1506} h={150}>
        <IconBadge x={20} y={14} size={56} icon={Info} iconSize={32} />
        <Track x={20} y={88} w={1464} h={40} />
      </Card>

      <ActionButton x={48} y={832} w={740} h={96} tone="navy" icon={RefreshCw} iconSize={44} />
      <ActionButton x={812} y={832} w={740} h={96} icon={CircleCheck} iconSize={44} />
    </ScreenFrame>
  );
}
