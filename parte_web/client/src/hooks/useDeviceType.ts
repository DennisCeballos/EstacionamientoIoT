
import { useState, useEffect } from 'react';

export const useDeviceType = () => {
  const [isDesktop, setIsDesktop] = useState(false);

  useEffect(() => {
    const checkDevice = () => {
      // Consider desktop if screen width is >= 768px
      setIsDesktop(window.innerWidth >= 768);
    };

    checkDevice();
    window.addEventListener('resize', checkDevice);

    return () => window.removeEventListener('resize', checkDevice);
  }, []);

  return { isDesktop };
};
