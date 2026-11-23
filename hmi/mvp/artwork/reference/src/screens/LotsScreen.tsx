import { Box as BoxIcon, Boxes, Save } from 'lucide-react';
import { ScreenFrame } from '../components/ScreenFrame';
import { ActionButton, Card, DisplayField, IconBadge, Text } from '../components/Hmi';

const SLOTS = 4;

const columns = ['ID', 'Código do Material', 'Quantidade', 'Validade'] as const;

export function LotsScreen() {
  return (
    <ScreenFrame>
      <IconBadge x={48} y={162} size={64} icon={Boxes} iconSize={38} shape="rounded" />
      <Text x={132} y={171} text="Gerenciar Lotes" size={44} weight={800} />

      {Array.from({ length: SLOTS }, (_, i) => (
        <Card key={i} x={47} y={243 + i * 176} w={1506} h={160}>
          <IconBadge x={20} y={31} size={96} icon={BoxIcon} iconSize={54} shape="rounded" />

          {columns.map((label, j) => {
            const cx = 136 + j * 282;
            return (
              <div key={label}>
                <Text x={cx} y={30} width={268} align="center" text={label} size={18} weight={600} />
                <DisplayField x={cx} y={62} w={268} h={66} />
              </div>
            );
          })}

          <div className="card-divider" style={{ left: 1269, top: 24, height: 110 }} />
          <ActionButton x={1322} y={16} w={128} h={128} icon={Save} iconSize={57} />
        </Card>
      ))}
    </ScreenFrame>
  );
}
