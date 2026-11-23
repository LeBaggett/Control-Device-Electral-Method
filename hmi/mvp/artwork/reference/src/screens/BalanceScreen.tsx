import { Scale } from 'lucide-react';
import { ScreenFrame } from '../components/ScreenFrame';
import { ActionButton, Card, DisplayField, IconBadge, Text } from '../components/Hmi';

const columns = ['Placa Superior', 'Placa Inferior'] as const;

export function BalanceScreen() {
  return (
    <ScreenFrame>
      {columns.map((label, i) => {
        const x = 259 + i * 560;
        return (
          <Card key={label} x={x} y={231} w={520} h={640}>
            <IconBadge x={24} y={24} size={100} icon={Scale} iconSize={56} />
            <Text x={144} y={57} text={label} size={34} weight={800} />
            <div className="card-divider" style={{ left: 24, top: 148, width: 472, height: 1 }} />

            <Text x={24} y={180} text="Leitura (mV)" size={20} weight={600} />
            <DisplayField x={24} y={208} w={290} h={76} />
            <Text x={324} y={234} text="mV" size={23} weight={600} />
            <ActionButton x={368} y={208} w={128} h={76} label="TARA" fontSize={21} />

            <Text x={24} y={324} text="Peso (KG)" size={20} weight={600} />
            <DisplayField x={24} y={352} w={472} h={120} />

            <Text x={24} y={512} text="Fator de Calibração" size={20} weight={600} />
            <DisplayField x={24} y={540} w={472} h={76} />
          </Card>
        );
      })}
    </ScreenFrame>
  );
}
