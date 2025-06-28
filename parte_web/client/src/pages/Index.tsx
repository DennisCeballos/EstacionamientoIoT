
import { useDeviceType } from '@/hooks/useDeviceType';
import { DesktopDashboard } from '@/components/DesktopDashboard';
import { MobileInterface } from '@/components/MobileInterface';

const Index = () => {
  const { isDesktop } = useDeviceType();

  return isDesktop ? <DesktopDashboard /> : <MobileInterface />;
};

export default Index;
