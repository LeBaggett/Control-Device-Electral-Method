import { ResultsScreen } from './screens/ResultsScreen';
import { ExecutionScreen } from './screens/ExecutionScreen';
import { LoadingScreen } from './screens/LoadingScreen';
import { HeatingScreen } from './screens/HeatingScreen';
import { AuditScreen } from './screens/AuditScreen';
import { AutotestScreen } from './screens/AutotestScreen';
import { BalanceScreen } from './screens/BalanceScreen';
import { ConfigMenuScreen } from './screens/ConfigMenuScreen';
import { ConfigMenu2Screen } from './screens/ConfigMenu2Screen';
import { InjectionCalibrationScreen } from './screens/InjectionCalibrationScreen';
import { InjectionMenuScreen } from './screens/InjectionMenuScreen';
import { LoginScreen } from './screens/LoginScreen';
import { LotConfigScreen } from './screens/LotConfigScreen';
import { LotsScreen } from './screens/LotsScreen';
import { ManualMotionScreen } from './screens/ManualMotionScreen';
import { MenuScreen } from './screens/MenuScreen';
import { MotionParamsScreen } from './screens/MotionParamsScreen';
import { StartPopupScreen } from './screens/StartPopupScreen';
import { TemperatureScreen } from './screens/TemperatureScreen';
import { TemperatureSetpointScreen } from './screens/TemperatureSetpointScreen';
import { TestConfigScreen } from './screens/TestConfigScreen';
import { TestListScreen } from './screens/TestListScreen';
import { TestMonitorScreen } from './screens/TestMonitorScreen';

const screens = {
  'test-results': ResultsScreen,
  'test-execution': ExecutionScreen,
  'test-loading': LoadingScreen,
  'test-heating': HeatingScreen,
  audit: AuditScreen,
  autotest: AutotestScreen,
  balance: BalanceScreen,
  'config-menu': ConfigMenuScreen,
  'config-menu-2': ConfigMenu2Screen,
  'injection-menu': InjectionMenuScreen,
  'injection-calibration': InjectionCalibrationScreen,
  login: LoginScreen,
  lots: LotsScreen,
  'lot-config': LotConfigScreen,
  menu: MenuScreen,
  'manual-motion': ManualMotionScreen,
  'motion-params': MotionParamsScreen,
  'start-popup': StartPopupScreen,
  temperature: TemperatureScreen,
  'temperature-setpoint': TemperatureSetpointScreen,
  'test-config': TestConfigScreen,
  'test-list': TestListScreen,
  'test-monitor': TestMonitorScreen,
} as const;

export type ScreenName = keyof typeof screens;

export default function App() {
  const params = new URLSearchParams(window.location.search);
  const requested = params.get('screen') as ScreenName | null;
  const name: ScreenName = requested && requested in screens ? requested : 'autotest';
  const Screen = screens[name];
  return <Screen />;
}
