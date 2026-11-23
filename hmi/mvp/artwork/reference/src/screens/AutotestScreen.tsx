import { Gauge, Pipette, Thermometer } from 'lucide-react';
import { ScreenFrame } from '../components/ScreenFrame';
import { Card, DisplayField, IconBadge, Text, Track } from '../components/Hmi';

export function AutotestScreen() {
  return (
    <ScreenFrame>
      <Text x={470} y={158} width={660} align="center" text="Progresso do Autoteste" size={53} weight={800} />

      <Text x={570} y={246} text="Tempo desde de o inicio:" size={23} weight={600} />
      <DisplayField x={860} y={238} w={210} h={50} />

      <Card x={344} y={310} w={944} h={190}>
        <IconBadge x={22} y={22} size={86} icon={Thermometer} iconSize={52} />
        <Text x={128} y={25} text="Teste de Temperatura" size={27} weight={800} />
        <div className="card-divider" style={{ left: 414, top: 22, height: 101 }} />

        <Text x={430} y={22} width={165} align="center" text="Êmbolo" size={19} weight={500} />
        <Text x={610} y={22} width={165} align="center" text="Placa Sup." size={19} weight={500} />
        <Text x={790} y={22} width={132} align="center" text="Placa Inf." size={19} weight={500} />

        <DisplayField x={430} y={50} w={165} h={74} />
        <DisplayField x={610} y={50} w={165} h={74} />
        <DisplayField x={790} y={50} w={132} h={74} />
        <Track x={20} y={143} w={904} h={34} />
      </Card>

      <Card x={344} y={515} w={944} h={126}>
        <IconBadge x={22} y={20} size={82} icon={Gauge} iconSize={51} />
        <Text x={128} y={25} text="Teste de Pressão" size={27} weight={800} />
        <Track x={128} y={72} w={796} h={36} />
      </Card>

      <Card x={344} y={656} w={944} h={126}>
        <IconBadge x={22} y={20} size={82} icon={Pipette} iconSize={49} />
        <Text x={128} y={25} text="Calibração LVDT" size={27} weight={800} />
        <Track x={128} y={72} w={796} h={36} />
      </Card>

      <Card x={344} y={798} w={944} h={88} />
    </ScreenFrame>
  );
}
