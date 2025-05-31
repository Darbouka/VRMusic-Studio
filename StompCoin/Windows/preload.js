const { contextBridge, ipcRenderer } = require('electron');

// Exponiere sichere APIs an den Renderer-Prozess
contextBridge.exposeInMainWorld('electron', {
    ipcRenderer: {
        invoke: async (channel, ...args) => {
            const validChannels = [
                'get-user-data',
                'set-user-data',
                'get-fitness-data',
                'set-fitness-data'
            ];
            
            if (validChannels.includes(channel)) {
                try {
                    return await ipcRenderer.invoke(channel, ...args);
                } catch (error) {
                    console.error(`Fehler bei IPC-Aufruf ${channel}:`, error);
                    throw error;
                }
            } else {
                throw new Error(`Ungültiger IPC-Kanal: ${channel}`);
            }
        }
    }
}); 