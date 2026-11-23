import {
  Box as BoxIcon,
  ClipboardCheck,
  FileClock,
  FlaskConical,
  LogOut,
  Settings,
  Users,
} from 'lucide-react';
import { ScreenFrame } from '../components/ScreenFrame';
import { ActionButton, Card, IconBadge, Text } from '../components/Hmi';

const rows = [
  ['Ajustes', ClipboardCheck],
  ['Gerenciar Usuário', Users],
  ['Gerenciar Ensaio', FlaskConical],
  ['Gerenciar Lote', BoxIcon],
  ['Auditoria', FileClock],
  ['Configuração', Settings],
] as const;

export function MenuScreen() {
  return (
    <ScreenFrame>
      {rows.map(([label, icon], i) => {
        const y = 170 + i * 112;
        return (
          <Card key={label} x={222} y={y} w={1156} h={100}>
            <IconBadge x={26} y={14} size={72} icon={icon} iconSize={44} shape="rounded" />
            <Text x={143} y={31} text={label} size={36} weight={700} />
          </Card>
        );
      })}
      <ActionButton x={595} y={847} w={445} h={73} label="SAIR" tone="red" icon={LogOut} fontSize={31} />
    </ScreenFrame>
  );
}
