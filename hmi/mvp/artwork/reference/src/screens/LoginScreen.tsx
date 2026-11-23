import { House, Lock, LogIn, User } from 'lucide-react';
import { ScreenFrame } from '../components/ScreenFrame';
import { ActionButton, Box, Card, DisplayField, Text } from '../components/Hmi';
import { theme } from '../theme';

const fields = [
  ['Usuário', User, 232, 268],
  ['Senha', Lock, 366, 404],
] as const;

export function LoginScreen() {
  return (
    <ScreenFrame>
      <Card x={351} y={177} w={898} h={714}>
        <Text x={0} y={46} width={896} align="center" text="Spiral Flow" size={96} weight={800} />
        <Text x={0} y={174} width={896} align="center" text="Login" size={36} weight={700} />

        {fields.map(([label, Icon, labelY, fieldY]) => (
          <div key={label}>
            <Text x={76} y={labelY} text={label} size={26} weight={700} />
            <DisplayField x={76} y={fieldY} w={744} h={76} />
            <Box x={96} y={fieldY + 20} w={36} h={36}>
              <Icon size={36} strokeWidth={2.1} color={theme.blue} />
            </Box>
          </div>
        ))}

        <ActionButton x={76} y={502} w={744} h={74} tone="navy" label="ENTRAR" icon={LogIn} iconSize={38} fontSize={30} />
        <ActionButton x={76} y={594} w={744} h={74} label="MENU PRINCIPAL" icon={House} iconSize={38} fontSize={30} />
      </Card>
    </ScreenFrame>
  );
}
