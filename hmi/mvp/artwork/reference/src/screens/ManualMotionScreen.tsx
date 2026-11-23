import { ArrowDown, ArrowUp, PanelTop, Syringe } from 'lucide-react';
import { ScreenFrame } from '../components/ScreenFrame';
import { ActionButton, Card, DisplayField, IconBadge, Text } from '../components/Hmi';

export function ManualMotionScreen() {
  return (
    <ScreenFrame>
      <Card x={305} y={238} w={925} h={157}>
        <IconBadge x={24} y={13} size={126} icon={PanelTop} iconSize={66} />
        <Text x={183} y={26} text="Placa" size={36} weight={800} />
        <DisplayField x={184} y={74} w={375} h={57} />
        <ActionButton x={599} y={31} w={137} h={98} icon={ArrowUp} />
        <ActionButton x={760} y={31} w={137} h={98} icon={ArrowDown} />
      </Card>

      <Card x={305} y={417} w={925} h={157}>
        <IconBadge x={24} y={13} size={126} icon={Syringe} iconSize={66} />
        <Text x={183} y={26} text="Êmbolo" size={36} weight={800} />
        <DisplayField x={184} y={74} w={375} h={57} />
        <ActionButton x={599} y={31} w={137} h={98} icon={ArrowUp} />
        <ActionButton x={760} y={31} w={137} h={98} icon={ArrowDown} />
      </Card>

      <Card x={95} y={603} w={660} h={278}>
        <DisplayField x={30} y={52} w={600} h={104} />
        <ActionButton x={30} y={180} w={600} h={74} label="ZERAR" fontSize={29} />
      </Card>

      <Card x={777} y={603} w={660} h={278}>
        <DisplayField x={30} y={52} w={600} h={104} />
        <ActionButton x={30} y={180} w={174} h={74} label="ZERAR" fontSize={25} />
        <ActionButton x={220} y={180} w={202} h={74} label="GOTO ZERO" fontSize={23} />
        <ActionButton x={438} y={180} w={192} h={74} label="INJETAR" fontSize={24} />
      </Card>
    </ScreenFrame>
  );
}
