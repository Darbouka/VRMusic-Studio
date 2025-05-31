const { app, BrowserWindow, ipcMain } = require('electron');
const path = require('path');
const Store = require('electron-store');

const store = new Store();

function createWindow() {
    const mainWindow = new BrowserWindow({
        width: 1200,
        height: 800,
        webPreferences: {
            nodeIntegration: true,
            contextIsolation: false
        },
        icon: path.join(__dirname, 'build/icon.ico')
    });

    // Im Entwicklungsmodus laden wir localhost:3000
    if (process.env.NODE_ENV === 'development') {
        mainWindow.loadURL('http://localhost:3000');
        mainWindow.webContents.openDevTools();
    } else {
        mainWindow.loadFile(path.join(__dirname, 'build/index.html'));
    }
}

app.whenReady().then(createWindow);

app.on('window-all-closed', () => {
    if (process.platform !== 'darwin') {
        app.quit();
    }
});

app.on('activate', () => {
    if (BrowserWindow.getAllWindows().length === 0) {
        createWindow();
    }
});

// IPC-Handler für die Kommunikation zwischen Haupt- und Renderer-Prozess
ipcMain.handle('get-user-data', () => {
    return store.get('userData');
});

ipcMain.handle('set-user-data', (event, data) => {
    store.set('userData', data);
    return true;
});

ipcMain.handle('get-fitness-data', () => {
    return store.get('fitnessData');
});

ipcMain.handle('set-fitness-data', (event, data) => {
    store.set('fitnessData', data);
    return true;
}); 