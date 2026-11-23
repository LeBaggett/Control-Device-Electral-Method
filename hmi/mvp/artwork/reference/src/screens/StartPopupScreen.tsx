import { CirclePlay } from 'lucide-react';
import { PopupFrame } from '../components/PopupFrame';
import { IconBadge, Text } from '../components/Hmi';

export function StartPopupScreen() {
  return (
    <PopupFrame width={800} height={440}>
      <IconBadge x={332} y={56} size={128} icon={CirclePlay} iconSize={76} />
      <Text x={0} y={220} width={792} align="center" text="Pressione" size={36} weight={600} />
      <Text x={0} y={278} width={792} align="center" text="INICIAR OPERAÇÃO" size={56} weight={800} className="popup-highlight" />
    </PopupFrame>
  );
}
