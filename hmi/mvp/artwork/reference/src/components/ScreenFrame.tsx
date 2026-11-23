import type { ReactNode } from 'react';
import { DESIGN } from '../theme';
import { EngineeringBackground } from './EngineeringBackground';

type Props = {
  children: ReactNode;
};

export function ScreenFrame({ children }: Props) {
  return (
    <main
      className="screen-frame"
      data-screen-root
      style={{ width: DESIGN.width, height: DESIGN.height }}
    >
      <img className="standard-header" src="/assets/header-standard.png" alt="" />
      <EngineeringBackground />
      <div className="screen-layer">{children}</div>
    </main>
  );
}
