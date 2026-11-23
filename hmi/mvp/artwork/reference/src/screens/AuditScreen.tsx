import { FileClock } from 'lucide-react';
import { ScreenFrame } from '../components/ScreenFrame';
import { Card, DisplayField, IconBadge, Text } from '../components/Hmi';

const SLOTS = 4;

const columns = [
  'Classificação',
  'Comprimento da espiral (mm)',
  'Tempo de gel (s)',
  'Tempo morto (s)',
] as const;

export function AuditScreen() {
  return (
    <ScreenFrame>
      <IconBadge x={48} y={162} size={64} icon={FileClock} iconSize={38} shape="rounded" />
      <Text x={132} y={171} text="Auditoria" size={44} weight={800} />

      {Array.from({ length: SLOTS }, (_, i) => (
        <Card key={i} x={47} y={243 + i * 176} w={1506} h={160}>
          <IconBadge x={20} y={31} size={96} icon={FileClock} iconSize={54} shape="rounded" />

          {columns.map((label, j) => {
            const cx = 136 + j * 340;
            return (
              <div key={label}>
                <Text x={cx} y={30} width={326} align="center" text={label} size={18} weight={600} />
                <DisplayField x={cx} y={62} w={326} h={66} />
              </div>
            );
          })}
        </Card>
      ))}
    </ScreenFrame>
  );
}
