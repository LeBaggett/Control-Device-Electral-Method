import { Thermometer, OctagonX } from 'lucide-react';
import { ScreenFrame } from '../components/ScreenFrame';
import { ActionButton, Card, ChartAxes, DisplayField, IconBadge, Text } from '../components/Hmi';
export function HeatingScreen() {
  return <ScreenFrame>
    <IconBadge x={48} y={162} size={64} icon={Thermometer} shape="rounded" />
    <Text x={132} y={171} text="Aquecimento" size={44} weight={800} />
    <Card x={47} y={246} w={986} h={560}>
      <Text x={28} y={24} text="Temperatura" size={28} />
      <Text x={828} y={28} text="°C" size={24} />
      <ChartAxes x={24} y={78} w={938} h={454} scale={[0, 50, 100, 150, 200, 250, 300, 350]} />
    </Card>
    <Card x={1051} y={246} w={502} h={560}>
      <Text x={28} y={24} text="Temperaturas" size={26} />
      {['Superior', 'Êmbolo', 'Inferior'].map((zone, i) => <div key={zone}>
        <Text x={28} y={120 + i * 160} text={`${zone} (°C)`} size={22} />
        <DisplayField x={264} y={100 + i * 160} w={210} h={76} />
      </div>)}
    </Card>
    <ActionButton x={47} y={832} w={1506} h={96} label="Interromper" tone="red" icon={OctagonX} iconSize={38} />
  </ScreenFrame>;
}
