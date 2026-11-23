import type { LucideIcon } from 'lucide-react';
import {
  Box as BoxIcon,
  Clock,
  FlaskConical,
  Gauge,
  Save,
  Play,
  Thermometer,
} from 'lucide-react';
import { ScreenFrame } from '../components/ScreenFrame';
import { ActionButton, Card, DisplayField, IconBadge, Text } from '../components/Hmi';
import { theme } from '../theme';

const SLOTS = 4;

const columns = [
  ['Temperatura', Thermometer],
  ['Pressão', Gauge],
  ['Tempo', Clock],
  ['Lote', BoxIcon],
] as const;

function ColumnHeader({ x, y, w, icon: Icon, label }: { x: number; y: number; w: number; icon: LucideIcon; label: string }) {
  return (
    <div
      className="hmi-text"
      style={{
        position: 'absolute',
        left: x,
        top: y,
        width: w,
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'center',
        gap: 8,
        fontSize: 19,
        fontWeight: 600,
      }}
    >
      <Icon size={24} strokeWidth={2.1} color={theme.blue} />
      {label}
    </div>
  );
}

export function TestListScreen() {
  return (
    <ScreenFrame>
      <IconBadge x={48} y={162} size={64} icon={FlaskConical} iconSize={38} shape="rounded" />
      <Text x={132} y={171} text="Gerenciar Ensaio" size={44} weight={800} />

      {Array.from({ length: SLOTS }, (_, i) => (
        <Card key={i} x={47} y={243 + i * 176} w={1506} h={160}>
          {columns.map(([label, icon], j) => {
            const cx = 24 + j * 290;
            return (
              <div key={label}>
                <ColumnHeader x={cx} y={22} w={276} icon={icon} label={label} />
                <DisplayField x={cx} y={58} w={276} h={76} />
              </div>
            );
          })}

          <div className="card-divider" style={{ left: 1196, top: 24, height: 110 }} />
          <ActionButton x={1214} y={16} w={128} h={128} tone="green" icon={Play} iconSize={59} />
          <ActionButton x={1356} y={16} w={128} h={128} icon={Save} iconSize={59} />
        </Card>
      ))}
    </ScreenFrame>
  );
}
