import { ArrowLeft, Boxes, Gauge, Save, Thermometer, Timer } from 'lucide-react';
import { ScreenFrame } from '../components/ScreenFrame';
import { ActionButton, Card, DisplayField, IconBadge, Text } from '../components/Hmi';

const fields = [
  ['Temperatura', Thermometer],
  ['Pressão', Gauge],
  ['Tempo de Cura', Timer],
  ['Lote', Boxes],
] as const;

export function TestConfigScreen() {
  return (
    <ScreenFrame>
      <Card x={259} y={220} w={1080} h={680}>
        <Text x={0} y={32} width={1078} align="center" text="Configurar Ensaio" size={44} weight={800} />

        {fields.map(([label, icon], i) => {
          const y = 114 + i * 112;
          return (
            <div key={label}>
              <IconBadge x={48} y={y + 6} size={72} icon={icon} iconSize={40} />
              <Text x={140} y={y + 27} text={label} size={30} weight={700} />
              <DisplayField x={420} y={y} w={610} h={84} />
            </div>
          );
        })}

        <ActionButton x={48} y={562} w={475} h={90} label="Voltar" icon={ArrowLeft} iconSize={36} fontSize={28} />
        <ActionButton x={555} y={562} w={475} h={90} label="Salvar Ensaio" icon={Save} iconSize={36} fontSize={28} />
      </Card>
    </ScreenFrame>
  );
}
