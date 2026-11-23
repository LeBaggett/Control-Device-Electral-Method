import type { ReactNode } from 'react';

type Props = {
  width: number;
  height: number;
  children: ReactNode;
};

export function PopupFrame({ width, height, children }: Props) {
  return (
    <main className="popup-frame" data-screen-root style={{ width, height }}>
      {children}
    </main>
  );
}
