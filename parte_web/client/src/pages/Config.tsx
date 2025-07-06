import { ThemeToggle } from '@/components/ThemeToggle';
import { useParkingStore } from '@/lib/parkingStore';
import { FormEvent, useEffect, useRef } from 'react';

export default function Config() {
  const { wsUrl, changeWsUrl } = useParkingStore();

  const socketRef = useRef<WebSocket>();
  const testInputRef = useRef<HTMLInputElement>();
  const wsUrlInputRef = useRef<HTMLInputElement>();

  useEffect(() => {
    socketRef.current = new WebSocket(wsUrl, 'arduino');
    socketRef.current.onopen = () => {
      console.log('✅ Conectado al servidor WebSocket');
    };
    socketRef.current.onclose = () => {
      console.log('❌ Conexión cerrada');
    };
    socketRef.current.onerror = (error) => {
      console.log('⚠️ Error: ', error);
    };

    return () => socketRef.current.close();
  }, [wsUrl]);

  const handleEnviarOnSubmit = (e: FormEvent) => {
    e.preventDefault();
    if (testInputRef.current.value.length === 15) {
      socketRef.current.send(testInputRef.current.value);
    } else {
      alert('tienen que ser 8 bits!');
    }
  };

  const handleCambiarUrlOnSubmit = (e: FormEvent) => {
    e.preventDefault();
    changeWsUrl(wsUrlInputRef.current.value);
  };

  return (
    <div className="flex flex-col gap-10 mx-auto max-w-96 px-4 pt-10">
      <ThemeToggle className="self-center px-16" />
      <form onSubmit={handleEnviarOnSubmit} className="flex flex-col gap-2 w-auto">
      < input ref={testInputRef} className="px-4 py-2 bg-background border-2 border-purple-900 rounded-md" type="text" placeholder="Probar Estado" defaultValue="1 0 1 0 1 0 0 0" maxLength={15} />
        <button className="py-2 text-white bg-purple-900 border-2 border-purple-900 rounded-md active:scale-[1.02] transition-all" type="submit">Probar</button>
      </form>
      <form onSubmit={handleCambiarUrlOnSubmit} className="flex flex-col gap-2">
        <input ref={wsUrlInputRef} className="px-4 py-2 bg-background border-2 border-purple-900 rounded-md" type="text" placeholder="Nueva Url para websocket" defaultValue={wsUrl} />
        <button className="py-2 text-white bg-purple-900 border-2 border-purple-900 rounded-md active:scale-[1.02] transition-all" type="submit">Cambiar URL del Websocket</button>
      </form>
    </div>
  );
}