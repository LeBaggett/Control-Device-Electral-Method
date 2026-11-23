import { DESIGN, theme } from '../theme';

const TOOTH_DEPTH = 22;

function gearPath(cx: number, cy: number, r: number, teeth: number) {
  const root = r - TOOTH_DEPTH / 2;
  const tip = r + TOOTH_DEPTH / 2;
  const pitch = (Math.PI * 2) / teeth;
  const pt = (radius: number, a: number) =>
    `${(cx + Math.cos(a) * radius).toFixed(2)} ${(cy + Math.sin(a) * radius).toFixed(2)}`;

  let d = `M ${pt(root, 0)}`;
  for (let i = 0; i < teeth; i++) {
    const a = i * pitch;
    d += ` L ${pt(tip, a + 0.12 * pitch)}`;
    d += ` A ${tip} ${tip} 0 0 1 ${pt(tip, a + 0.38 * pitch)}`;
    d += ` L ${pt(root, a + 0.5 * pitch)}`;
    d += ` A ${root} ${root} 0 0 1 ${pt(root, a + pitch)}`;
  }
  return `${d} Z`;
}

function Gear({
  cx,
  cy,
  r,
  teeth,
  hub = true,
  opacity = 1,
}: {
  cx: number;
  cy: number;
  r: number;
  teeth: number;
  hub?: boolean;
  opacity?: number;
}) {
  const root = r - TOOTH_DEPTH / 2;
  const line = { fill: 'none', stroke: theme.blueprintStrong, strokeWidth: 2 };

  return (
    <g opacity={opacity}>
      <path d={gearPath(cx, cy, r, teeth)} {...line} strokeLinejoin="round" />
      <circle cx={cx} cy={cy} r={root * 0.76} {...line} />
      {hub && (
        <>
          <circle cx={cx} cy={cy} r={root * 0.36} {...line} />
          <circle cx={cx} cy={cy} r={root * 0.16} {...line} />
        </>
      )}
    </g>
  );
}

function Circuit({
  d,
  opacity = 1,
}: {
  d: string;
  opacity?: number;
}) {
  return (
    <path
      d={d}
      fill="none"
      stroke={theme.blueprintStrong}
      strokeWidth={2}
      opacity={opacity}
      strokeLinecap="round"
      strokeLinejoin="round"
    />
  );
}

function Dot({
  x,
  y,
  r,
  opacity = 1,
}: {
  x: number;
  y: number;
  r: number;
  opacity?: number;
}) {
  return (
    <circle
      cx={x}
      cy={y}
      r={r}
      fill="none"
      stroke={theme.blueprintStrong}
      strokeWidth={1.5}
      opacity={opacity}
    />
  );
}

export function EngineeringBackground() {
  const { width, height, headerHeight } = DESIGN;

  return (
    <svg
      width={width}
      height={height}
      viewBox={`0 0 ${width} ${height}`}
      style={{
        position: 'absolute',
        inset: 0,
        pointerEvents: 'none',
      }}
    >
      <rect x={0} y={0} width={width} height={height} fill={theme.bg} />

      {/* grid bem suave */}
      <g opacity={0.28}>
        {Array.from({ length: Math.ceil(width / 48) }).map((_, i) => (
          <line
            key={`vx-${i}`}
            x1={i * 48}
            y1={headerHeight}
            x2={i * 48}
            y2={height}
            stroke={theme.blueprint}
            strokeWidth={1}
          />
        ))}
        {Array.from({ length: Math.ceil((height - headerHeight) / 48) }).map((_, i) => (
          <line
            key={`hy-${i}`}
            x1={0}
            y1={headerHeight + i * 48}
            x2={width}
            y2={headerHeight + i * 48}
            stroke={theme.blueprint}
            strokeWidth={1}
          />
        ))}
      </g>

      {/* laterais mais “desenhadas”, centro mais limpo */}
      <g opacity={0.5}>
        {/* esquerda */}
        <Gear cx={70} cy={470} r={135} teeth={22} opacity={0.7} />

        <Circuit
          opacity={0.5}
          d="
            M 42 305
            L 42 265
            L 110 265
            L 110 228
            L 156 228

            M 38 730
            L 145 730
            L 145 775
            L 330 775
            L 330 862
          "
        />

        <Dot x={36} y={212} r={8} opacity={0.55} />
        <Dot x={63} y={250} r={12} opacity={0.5} />
        <Dot x={96} y={212} r={12} opacity={0.45} />
        <Dot x={126} y={288} r={8} opacity={0.4} />
        <Dot x={152} y={250} r={7} opacity={0.38} />
        <Dot x={178} y={288} r={12} opacity={0.42} />
        <Dot x={205} y={250} r={7} opacity={0.38} />

        {/* direita */}
        <Gear cx={1530} cy={470} r={128} teeth={18} hub={false} opacity={0.7} />
        <Gear cx={1530} cy={470} r={69} teeth={10} opacity={0.6} />

        <Circuit
          opacity={0.5}
          d="
            M 1392 284
            L 1432 284
            L 1432 240
            L 1500 240
            L 1500 196

            M 1368 742
            L 1460 742
            L 1460 808
            L 1540 808
          "
        />

        <Dot x={1415} y={238} r={8} opacity={0.42} />
        <Dot x={1474} y={212} r={7} opacity={0.38} />
        <Dot x={1512} y={252} r={12} opacity={0.4} />
        <Dot x={1558} y={210} r={8} opacity={0.38} />
      </g>
    </svg>
  );
}
