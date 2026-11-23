import { ArrowLeft, Barcode, CalendarDays, Hash, Save, Weight } from 'lucide-react';
import { ScreenFrame } from '../components/ScreenFrame';
import { ActionButton, Card, DisplayField, IconBadge, Text } from '../components/Hmi';

const fields = [
  ['ID', Hash],
  ['Código do Material', Barcode],
  ['Quantidade (g)', Weight],
  ['Validade (mês/ano)', CalendarDays],
] as const;

export function LotConfigScreen() {
  return (
    <ScreenFrame>
      <Card x={320} y={178} w={960} h={734}>
        <Text x={0} y={32} width={958} align="center" text="Cadastrar Lote" size={48} weight={800} />

        {fields.map(([label, icon], i) => {
          const y = 120 + i * 116;
          return (
            <div key={label}>
              <IconBadge x={64} y={y - 4} size={40} icon={icon} iconSize={25} shape="rounded" />
              <Text x={120} y={y} text={label} size={24} weight={700} />
              <DisplayField x={64} y={y + 40} w={832} h={64} />
            </div>
          );
        })}

        <ActionButton x={64} y={622} w={398} h={76} label="Voltar" icon={ArrowLeft} iconSize={34} fontSize={28} />
        <ActionButton x={498} y={622} w={398} h={76} label="Salvar Lote" tone="navy" icon={Save} iconSize={34} fontSize={28} />
      </Card>
    </ScreenFrame>
  );
}
