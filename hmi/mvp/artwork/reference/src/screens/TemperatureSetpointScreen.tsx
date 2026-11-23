import { Thermometer } from 'lucide-react';
import { ScreenFrame } from '../components/ScreenFrame';
import { Card, DisplayField, IconBadge, Text } from '../components/Hmi';

const rows = ['Êmbolo', 'Placa Superior', 'Placa Inferior'] as const;
const fields = ['Leitura', 'Setpoint'] as const;

export function TemperatureSetpointScreen() {
  return (
    <ScreenFrame>
      {rows.map((label, i) => {
        const y = 207 + i * 238;
        return (
          <Card key={label} x={259} y={y} w={1080} h={210}>
            <IconBadge x={28} y={50} size={110} icon={Thermometer} iconSize={60} />
            <Text x={162} y={88} text={label} size={32} weight={800} />
            <div className="card-divider" style={{ left: 410, top: 28, height: 154 }} />

            {fields.map((field, j) => {
              const fx = 434 + j * 326;
              return (
                <div key={field}>
                  <Text x={fx} y={28} width={292} align="center" text={field} size={20} weight={600} />
                  <DisplayField x={fx} y={58} w={292} h={124} />
                </div>
              );
            })}
          </Card>
        );
      })}
    </ScreenFrame>
  );
}
