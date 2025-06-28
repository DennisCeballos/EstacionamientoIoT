import { create } from 'zustand';

export enum ParkingSpotStatus {
  FREE = 1,
  OCCUPIED = 0,
  RESERVED = 2
}

export interface ParkingSpot {
  id: number;
  floor: number;
  status: ParkingSpotStatus;
}

export interface ServerParkingState {
  [floor: number]: {
    [spot: number]: ParkingSpotStatus
  }
};
interface ParkingStore {
  wsUrl: string;
  changeWsUrl: (newWsUrl: string) => void;
  spots: ParkingSpot[];
  initializeSpots: () => void;
  reserveSpot: () => number | null;
  getAvailableSpots: () => ParkingSpot[];
  getOccupiedSpots: () => ParkingSpot[];
  getFloorSpots: (floor: number) => ParkingSpot[];
  syncSpots: (updatedParkingSpots: ServerParkingState) => void;
  getSpot: (id: number) => ParkingSpot | undefined;
}

export const useParkingStore = create<ParkingStore>((set, get) => ({
  wsUrl: '/',

  changeWsUrl: (newWsUrl) => {
    set({ wsUrl: newWsUrl });
  },

  spots: [],

  initializeSpots: () => {
    const initialSpots: ParkingSpot[] = [
      // Floor 1 - spots 1-4
      // { id: 1, floor: 1, status: ParkingSpotStatus.FREE },
      // { id: 2, floor: 1, status: ParkingSpotStatus.FREE },
      // { id: 3, floor: 1, status: ParkingSpotStatus.FREE },
      // { id: 4, floor: 1, status: ParkingSpotStatus.FREE },
      // Floor 2 - spots 5-8
      // { id: 5, floor: 2, status: ParkingSpotStatus.FREE },
      // { id: 6, floor: 2, status: ParkingSpotStatus.FREE },
      // { id: 7, floor: 2, status: ParkingSpotStatus.FREE },
      // { id: 8, floor: 2, status: ParkingSpotStatus.FREE },
    ];
    set({ spots: initialSpots });
  },

  syncSpots: (serverParkingState: ServerParkingState) => {
    const syncedParkingSpots: ParkingSpot[] = [];
    for (let i = 0; i < 8; i++) {// 8 espacios totales
      const syncedSpot: ParkingSpot = {
        id: i + 1,
        floor: Math.floor(i / 4) + 1,// 4 espacios por piso
        status: serverParkingState[Math.floor(i / 4)][i % 4]
      };
      syncedParkingSpots.push(syncedSpot);
    }
    console.log(syncedParkingSpots);
    set({ spots: syncedParkingSpots });
  },

  reserveSpot: () => {
    const spots = get().spots;
    const availableSpots = spots.filter(spot => spot.status !== ParkingSpotStatus.OCCUPIED && spot.status !== ParkingSpotStatus.RESERVED);
    
    if (availableSpots.length === 0) return null;
    
    const targetSpot = availableSpots[Math.floor(Math.random() * availableSpots.length)];
    
    const updatedSpots: ParkingSpot[] = spots.map(spot =>
      spot.id === targetSpot.id
        ? { ...spot, status: ParkingSpotStatus.RESERVED }
        : spot
    );
    
    set({ spots: updatedSpots });
    return targetSpot.id;
  },

  getAvailableSpots: () => get().spots.filter(spot => spot.status === ParkingSpotStatus.FREE),
  getOccupiedSpots: () => get().spots.filter(spot => spot.status === ParkingSpotStatus.OCCUPIED || spot.status === ParkingSpotStatus.RESERVED),
  getFloorSpots: (floor: number) => get().spots.filter(spot => spot.floor === floor),
  getSpot: (id: number) => get().spots.find(spot => spot.id === id)
}));
