
import { useParkingStore } from '@/lib/parkingStore';
import { ParkingFloor } from './ParkingFloor';
import { Card, CardContent, CardHeader, CardTitle } from '@/components/ui/card';
import { Logo } from '@/components/Logo';
import { ThemeToggle } from '@/components/ThemeToggle';
import { cn } from '@/lib/utils';
import { useEffect } from 'react';

export const DesktopDashboard = () => {
  const { wsUrl, spots, getAvailableSpots, getOccupiedSpots, syncSpots } = useParkingStore();
  const availableSpots = getAvailableSpots();
  const occupiedSpots = getOccupiedSpots();
  const totalSpots = spots.length;

  const getStatusMessage = () => {
    const availableCount = availableSpots.length;
    if (availableCount === 0) return 'LLENO';
    if (availableCount === totalSpots) return 'VACÍO';
    return 'DISPONIBLE';
  };

  const getStatusColor = () => {
    const availableCount = availableSpots.length;
    if (availableCount === 0) return 'text-destructive';
    if (availableCount === totalSpots) return 'text-blue-600';
    return 'text-green-600';
  };

  useEffect(() => {
    const socket = new WebSocket(wsUrl, 'web');
    socket.onopen = () => {
      console.log('✅ Conectado al servidor WebSocket');
    };
    socket.onmessage = (event) => {
      console.log('📥 Mensaje del servidor: ', event.data);
      syncSpots(JSON.parse(event.data));
    };
    socket.onclose = () => {
      console.log('❌ Conexión cerrada');
    };
    socket.onerror = (error) => {
      console.log('⚠️ Error: ', error);
    };

    return () => socket.close();
  }, [wsUrl]);

  return (
    <div className="min-h-screen p-6">
      <div className="max-w-6xl mx-auto">
        {/* Header with Logo and Theme Toggle */}
        <div className="flex justify-between items-center mb-8">
          <div className="flex items-center gap-4">
            <Logo size="lg" showText={false} />
            <div>
              <h1 className="text-4xl font-bold">Panel sPark</h1>
              <p className="text-muted-foreground">Sistema Inteligente de Gestión de Estacionamiento</p>
            </div>
          </div>
          <ThemeToggle />
        </div>

        {/* Status Overview */}
        <div className="grid md:grid-cols-3 gap-6 mb-8">
          <Card>
            <CardHeader className="pb-3">
              <CardTitle className="text-sm font-medium">Espacios Totales</CardTitle>
            </CardHeader>
            <CardContent>
              <div className="text-3xl font-bold">{totalSpots}</div>
            </CardContent>
          </Card>

          <Card>
            <CardHeader className="pb-3">
              <CardTitle className="text-sm font-medium">Disponibles</CardTitle>
            </CardHeader>
            <CardContent>
              <div className="text-3xl font-bold text-green-600">
                {availableSpots.length}
              </div>
            </CardContent>
          </Card>

          <Card>
            <CardHeader className="pb-3">
              <CardTitle className="text-sm font-medium">Ocupados</CardTitle>
            </CardHeader>
            <CardContent>
              <div className="text-3xl font-bold text-destructive">
                {occupiedSpots.length}
              </div>
            </CardContent>
          </Card>
        </div>

        {/* Main Status Indicator */}
        <Card className="mb-8">
          <CardContent className="pt-6">
            <div className="text-center">
              <div className={cn('text-6xl font-bold mb-2', getStatusColor())}>
                {getStatusMessage()}
              </div>
              <div className="text-lg text-muted-foreground">
                Estado del Estacionamiento
              </div>
            </div>
          </CardContent>
        </Card>

        {/* Parking Floors */}
        <div className="grid lg:grid-cols-2 gap-6">
          <ParkingFloor floor={1} isDesktop={true} />
          <ParkingFloor floor={2} isDesktop={true} />
        </div>
      </div>
    </div>
  );
};
