import type { CSSProperties, ReactNode } from 'react';
import type { LucideIcon } from 'lucide-react';
import { theme } from '../theme';

type BoxProps = {
  x: number;
  y: number;
  w: number;
  h: number;
  className?: string;
  children?: ReactNode;
  style?: CSSProperties;
};

export function Box({ x, y, w, h, className = '', children, style }: BoxProps) {
  return (
    <div
      className={className}
      style={{ position: 'absolute', left: x, top: y, width: w, height: h, ...style }}
    >
      {children}
    </div>
  );
}

export function Card(props: BoxProps) {
  return <Box {...props} className={`hmi-card ${props.className ?? ''}`} />;
}

type TextProps = {
  x: number;
  y: number;
  text: string;
  size?: number;
  weight?: number;
  width?: number;
  align?: CSSProperties['textAlign'];
  className?: string;
};

export function Text({ x, y, text, size = 28, weight = 700, width, align = 'left', className = '' }: TextProps) {
  return (
    <div
      className={`hmi-text ${className}`}
      style={{ position: 'absolute', left: x, top: y, width, fontSize: size, fontWeight: weight, textAlign: align }}
    >
      {text}
    </div>
  );
}

type IconBadgeProps = {
  x: number;
  y: number;
  size: number;
  icon: LucideIcon;
  iconSize?: number;
  shape?: 'circle' | 'rounded';
};

export function IconBadge({ x, y, size, icon: Icon, iconSize = Math.round(size * 0.54), shape = 'circle' }: IconBadgeProps) {
  return (
    <div className={`icon-badge icon-badge--${shape}`} style={{ left: x, top: y, width: size, height: size }}>
      <Icon size={iconSize} strokeWidth={2.1} />
    </div>
  );
}

export function DisplayField({ x, y, w, h, className = '' }: BoxProps) {
  return <Box x={x} y={y} w={w} h={h} className={`display-field ${className}`} />;
}

export function Track({ x, y, w, h }: BoxProps) {
  return <Box x={x} y={y} w={w} h={h} className="progress-track" />;
}

// Margins leave room for the axis scale labels drawn by the HMI trend graph.
export const CHART_MARGIN = { left: 80, right: 16, top: 12, bottom: 48 } as const;

type ChartAxesProps = BoxProps & {
  rows?: number;
  cols?: number;
  /** Y-axis labels from bottom to top, evenly spaced; overrides `rows`. */
  scale?: readonly number[];
};

export function ChartAxes({ x, y, w, h, rows: rowsProp = 4, cols = 6, scale }: ChartAxesProps) {
  const rows = scale ? scale.length - 1 : rowsProp;
  const { left, right, top, bottom } = CHART_MARGIN;
  const pw = w - left - right;
  const ph = h - top - bottom;
  const x0 = left;
  const y0 = top + ph;
  const axis = { stroke: theme.navy, strokeOpacity: 0.55, strokeWidth: 2 };

  return (
    <Box x={x} y={y} w={w} h={h}>
      <svg width={w} height={h} aria-hidden="true">
        {Array.from({ length: rows }, (_, i) => {
          const gy = top + (i * ph) / rows;
          return (
            <line
              key={`g-${i}`}
              x1={x0}
              y1={gy}
              x2={x0 + pw}
              y2={gy}
              stroke={theme.cardBorder}
              strokeWidth={1.5}
              strokeDasharray="6 6"
            />
          );
        })}
        {Array.from({ length: rows + 1 }, (_, i) => {
          const ty = top + (i * ph) / rows;
          return <line key={`yt-${i}`} x1={x0 - 10} y1={ty} x2={x0} y2={ty} {...axis} />;
        })}
        {Array.from({ length: cols + 1 }, (_, i) => {
          const tx = x0 + (i * pw) / cols;
          return <line key={`xt-${i}`} x1={tx} y1={y0} x2={tx} y2={y0 + 10} {...axis} />;
        })}
        {scale?.map((value, i) => (
          <text
            key={`yl-${i}`}
            x={x0 - 16}
            y={y0 - (i * ph) / rows}
            textAnchor="end"
            dominantBaseline="central"
            fontSize={20}
            fontWeight={600}
            fill={theme.navy}
          >
            {value}
          </text>
        ))}
        <line x1={x0} y1={top} x2={x0} y2={y0} {...axis} />
        <line x1={x0} y1={y0} x2={x0 + pw} y2={y0} {...axis} />
      </svg>
    </Box>
  );
}

type ButtonProps = BoxProps & {
  label?: string;
  tone?: 'blue' | 'red' | 'navy' | 'green';
  icon?: LucideIcon;
  iconSize?: number;
  fontSize?: number;
};

export function ActionButton({
  x,
  y,
  w,
  h,
  label,
  tone = 'blue',
  icon: Icon,
  iconSize = 36,
  fontSize = 28,
}: ButtonProps) {
  return (
    <Box x={x} y={y} w={w} h={h} className={`action-button action-button--${tone}`}>
      {Icon ? <Icon size={iconSize} strokeWidth={2.2} /> : null}
      {label ? <span style={{ fontSize }}>{label}</span> : null}
    </Box>
  );
}
