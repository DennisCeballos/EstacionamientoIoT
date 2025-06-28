
import { useParkingStore, ParkingSpot, ParkingSpotStatus } from '@/lib/parkingStore';
import { cn } from '@/lib/utils';

interface ParkingFloorProps {
  floor: number;
  isDesktop: boolean;
}

export const ParkingFloor = ({ floor, isDesktop }: ParkingFloorProps) => {
  const { getFloorSpots } = useParkingStore();
  const spots = getFloorSpots(floor);

  return (
    <div className="bg-card rounded-lg p-4 border shadow-sm">
      <h3 className="text-lg font-semibold mb-4 text-center">
        Piso {floor}
      </h3>
      <div className="grid grid-cols-2 gap-3">
        {spots.map((spot) => (
          <ParkingSpotCard
            key={spot.id}
            spot={spot}
          />
        ))}
      </div>
    </div>
  );
};

interface ParkingSpotCardProps {
  spot: ParkingSpot;
}

const ParkingSpotCard = ({ spot }: ParkingSpotCardProps) => {
  return (
    <div
      className={cn(
        "relative aspect-[3/2] rounded-lg border-2 transition-all duration-300 flex items-center justify-center",
        spot.status === ParkingSpotStatus.FREE && "bg-green-50 dark:bg-green-950 border-green-500 text-green-700 dark:text-green-400 hover:bg-green-100 dark:hover:bg-green-900",
        spot.status === ParkingSpotStatus.OCCUPIED && "bg-destructive/10 border-destructive text-destructive",
        spot.status === ParkingSpotStatus.RESERVED && "bg-orange-400/10 border-orange-400 text-orange-400"
      )}
    >
      <div className="text-center">
        <div className="text-2xl font-bold">
          {spot.id}
        </div>
        <div className="text-xs">
          {spot.status === ParkingSpotStatus.OCCUPIED ? 'OCUPADO' : (spot.status === ParkingSpotStatus.RESERVED ? 'RESERVADO' : 'DISPONIBLE') }
        </div>
      </div>
      
      {/* Car icon for occupied spots */}
      {spot.status === ParkingSpotStatus.OCCUPIED || spot.status === ParkingSpotStatus.RESERVED && (
        <div className="absolute inset-0 flex items-center justify-center bg-destructive/5 rounded-lg">
          <div className="w-8 h-5 bg-destructive/20 rounded-sm"></div>
        </div>
      )}
    </div>
  );
};
