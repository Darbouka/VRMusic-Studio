const express = require('express');
const http = require('http');
const WebSocket = require('ws');
const { v4: uuidv4 } = require('uuid');
const mongoose = require('mongoose');
const Redis = require('ioredis');
const cluster = require('cluster');
const os = require('os');

// Redis-Client für Caching
const redis = new Redis({
    host: process.env.REDIS_HOST || 'localhost',
    port: process.env.REDIS_PORT || 6379,
    password: process.env.REDIS_PASSWORD,
    maxRetriesPerRequest: 3
});

// MongoDB-Verbindung
mongoose.connect(process.env.MONGODB_URI, {
    useNewUrlParser: true,
    useUnifiedTopology: true,
    maxPoolSize: 50,
    minPoolSize: 10,
    socketTimeoutMS: 45000,
    keepAlive: true
});

// Cluster-Konfiguration für horizontale Skalierung
if (cluster.isMaster) {
    const numCPUs = os.cpus().length;
    console.log(`Master-Prozess ${process.pid} läuft`);

    // Worker-Prozesse starten
    for (let i = 0; i < numCPUs; i++) {
        cluster.fork();
    }

    cluster.on('exit', (worker, code, signal) => {
        console.log(`Worker ${worker.process.pid} beendet`);
        cluster.fork(); // Neuen Worker starten
    });
} else {
    const app = express();
    const server = http.createServer(app);
    const wss = new WebSocket.Server({ server });

    // Speicher für aktive Streams und Verbindungen
    const activeStreams = new Map();
    const connections = new Map();

    // Rate Limiting
    const rateLimit = require('express-rate-limit');
    const limiter = rateLimit({
        windowMs: 15 * 60 * 1000, // 15 Minuten
        max: 100 // Limit pro IP
    });
    app.use(limiter);

    // Body Parser
    app.use(express.json({ limit: '50mb' }));
    app.use(express.urlencoded({ extended: true, limit: '50mb' }));

    // CORS
    app.use((req, res, next) => {
        res.header('Access-Control-Allow-Origin', '*');
        res.header('Access-Control-Allow-Headers', 'Origin, X-Requested-With, Content-Type, Accept');
        next();
    });

    // WebSocket-Verbindungshandler
    wss.on('connection', (ws) => {
        const connectionId = uuidv4();
        connections.set(connectionId, ws);
        
        console.log(`Neue Verbindung: ${connectionId}`);
        
        // Nachrichten-Handler
        ws.on('message', async (message) => {
            try {
                const data = JSON.parse(message);
                await handleMessage(connectionId, data);
            } catch (error) {
                console.error('Fehler beim Verarbeiten der Nachricht:', error);
                sendError(ws, 'Ungültige Nachricht');
            }
        });
        
        // Verbindungsabbruch-Handler
        ws.on('close', () => {
            handleDisconnect(connectionId);
        });
    });

    // Datenverarbeitungs-Funktionen
    async function processFitnessData(data) {
        try {
            // Daten in Redis zwischenspeichern
            await redis.setex(`fitness:${data.userId}:${data.timestamp}`, 3600, JSON.stringify(data));
            
            // Daten in MongoDB speichern
            await FitnessData.create(data);
            
            // Batch-Verarbeitung für Statistiken
            await updateUserStats(data.userId);
        } catch (error) {
            console.error('Fehler bei der Fitness-Datenverarbeitung:', error);
            throw error;
        }
    }

    async function processTransactionData(data) {
        try {
            // Transaktion in Redis zwischenspeichern
            await redis.setex(`transaction:${data.id}`, 3600, JSON.stringify(data));
            
            // Transaktion in MongoDB speichern
            await Transaction.create(data);
            
            // Wallet-Salden aktualisieren
            await updateWalletBalances(data);
        } catch (error) {
            console.error('Fehler bei der Transaktionsverarbeitung:', error);
            throw error;
        }
    }

    async function processSocialData(data) {
        try {
            // Social-Daten in Redis zwischenspeichern
            await redis.setex(`social:${data.id}`, 3600, JSON.stringify(data));
            
            // Social-Daten in MongoDB speichern
            await SocialData.create(data);
            
            // Benachrichtigungen senden
            await sendNotifications(data);
        } catch (error) {
            console.error('Fehler bei der Social-Datenverarbeitung:', error);
            throw error;
        }
    }

    // Batch-Verarbeitung
    async function processBatchData(dataArray, type) {
        const batchSize = 1000;
        for (let i = 0; i < dataArray.length; i += batchSize) {
            const batch = dataArray.slice(i, i + batchSize);
            await Promise.all(batch.map(data => {
                switch (type) {
                    case 'fitness':
                        return processFitnessData(data);
                    case 'transaction':
                        return processTransactionData(data);
                    case 'social':
                        return processSocialData(data);
                    default:
                        throw new Error('Unbekannter Datentyp');
                }
            }));
        }
    }

    // Server starten
    const PORT = process.env.PORT || 3000;
    server.listen(PORT, () => {
        console.log(`Worker ${process.pid} läuft auf Port ${PORT}`);
    });
}

// Nachrichtenverarbeitung
function handleMessage(connectionId, message) {
    const ws = connections.get(connectionId);
    if (!ws) return;
    
    switch (message.type) {
        case 'register':
            handleStreamRegistration(connectionId, message);
            break;
            
        case 'offer':
            handleOffer(connectionId, message);
            break;
            
        case 'answer':
            handleAnswer(connectionId, message);
            break;
            
        case 'ice-candidate':
            handleIceCandidate(connectionId, message);
            break;
            
        case 'stomp':
            handleStompEvent(connectionId, message);
            break;
            
        case 'end':
            handleStreamEnd(connectionId, message);
            break;
            
        default:
            sendError(ws, 'Unbekannter Nachrichtentyp');
    }
}

// Stream-Registrierung
function handleStreamRegistration(connectionId, message) {
    const { streamId, challengeId } = message;
    
    if (!streamId || !challengeId) {
        sendError(connections.get(connectionId), 'Fehlende Stream- oder Challenge-ID');
        return;
    }
    
    // Stream-Informationen speichern
    activeStreams.set(streamId, {
        connectionId,
        challengeId,
        viewers: new Set(),
        stompCount: 0,
        startTime: Date.now()
    });
    
    console.log(`Stream registriert: ${streamId} für Challenge ${challengeId}`);
    
    // Bestätigung senden
    sendMessage(connections.get(connectionId), {
        type: 'registered',
        streamId,
        challengeId
    });
}

// WebRTC-Signalisierung
function handleOffer(connectionId, message) {
    const { streamId, sdp } = message;
    const stream = activeStreams.get(streamId);
    
    if (!stream) {
        sendError(connections.get(connectionId), 'Stream nicht gefunden');
        return;
    }
    
    // Angebot an alle Zuschauer weiterleiten
    stream.viewers.forEach(viewerId => {
        const viewerWs = connections.get(viewerId);
        if (viewerWs) {
            sendMessage(viewerWs, {
                type: 'offer',
                streamId,
                sdp
            });
        }
    });
}

function handleAnswer(connectionId, message) {
    const { streamId, sdp } = message;
    const stream = activeStreams.get(streamId);
    
    if (!stream) {
        sendError(connections.get(connectionId), 'Stream nicht gefunden');
        return;
    }
    
    // Antwort an den Streamer weiterleiten
    const streamerWs = connections.get(stream.connectionId);
    if (streamerWs) {
        sendMessage(streamerWs, {
            type: 'answer',
            streamId,
            sdp
        });
    }
}

function handleIceCandidate(connectionId, message) {
    const { streamId, candidate } = message;
    const stream = activeStreams.get(streamId);
    
    if (!stream) {
        sendError(connections.get(connectionId), 'Stream nicht gefunden');
        return;
    }
    
    // ICE-Kandidat an die andere Partei weiterleiten
    const targetId = stream.connectionId === connectionId ? 
        Array.from(stream.viewers)[0] : stream.connectionId;
    
    const targetWs = connections.get(targetId);
    if (targetWs) {
        sendMessage(targetWs, {
            type: 'ice-candidate',
            streamId,
            candidate
        });
    }
}

// Stomp-Event-Verarbeitung
function handleStompEvent(connectionId, message) {
    const { streamId } = message;
    const stream = activeStreams.get(streamId);
    
    if (!stream) {
        sendError(connections.get(connectionId), 'Stream nicht gefunden');
        return;
    }
    
    // Stomp-Zähler erhöhen
    stream.stompCount++;
    
    // Stomp-Event an alle Zuschauer weiterleiten
    stream.viewers.forEach(viewerId => {
        const viewerWs = connections.get(viewerId);
        if (viewerWs) {
            sendMessage(viewerWs, {
                type: 'stomp',
                streamId,
                totalStomps: stream.stompCount
            });
        }
    });
    
    // Challenge-Fortschritt aktualisieren
    updateChallengeProgress(stream);
}

// Stream-Ende
function handleStreamEnd(connectionId, message) {
    const { streamId } = message;
    const stream = activeStreams.get(streamId);
    
    if (!stream) return;
    
    // Alle Zuschauer benachrichtigen
    stream.viewers.forEach(viewerId => {
        const viewerWs = connections.get(viewerId);
        if (viewerWs) {
            sendMessage(viewerWs, {
                type: 'stream-ended',
                streamId
            });
        }
    });
    
    // Stream-Informationen löschen
    activeStreams.delete(streamId);
    console.log(`Stream beendet: ${streamId}`);
}

// Verbindungsabbruch
function handleDisconnect(connectionId) {
    // Aktive Streams des Benutzers beenden
    for (const [streamId, stream] of activeStreams.entries()) {
        if (stream.connectionId === connectionId) {
            handleStreamEnd(connectionId, { streamId });
        }
    }
    
    // Verbindung entfernen
    connections.delete(connectionId);
    console.log(`Verbindung getrennt: ${connectionId}`);
}

// Challenge-Fortschritt aktualisieren
async function updateChallengeProgress(stream) {
    try {
        // API-Aufruf zum Aktualisieren des Challenge-Fortschritts
        const response = await fetch('https://api.stompcoin.com/challenges/progress', {
            method: 'POST',
            headers: {
                'Content-Type': 'application/json'
            },
            body: JSON.stringify({
                challengeId: stream.challengeId,
                stomps: stream.stompCount
            })
        });
        
        if (!response.ok) {
            console.error('Fehler beim Aktualisieren des Challenge-Fortschritts');
        }
    } catch (error) {
        console.error('Fehler beim API-Aufruf:', error);
    }
}

// Hilfsfunktionen
function sendMessage(ws, message) {
    if (ws.readyState === WebSocket.OPEN) {
        ws.send(JSON.stringify(message));
    }
}

function sendError(ws, error) {
    if (ws.readyState === WebSocket.OPEN) {
        ws.send(JSON.stringify({ type: 'error', message: error }));
    }
}

// Fehlerbehandlung
process.on('uncaughtException', (error) => {
    console.error('Unbehandelter Fehler:', error);
    // Fehler protokollieren und ggf. Benachrichtigung senden
});

process.on('unhandledRejection', (reason, promise) => {
    console.error('Unbehandelte Promise-Ablehnung:', reason);
    // Fehler protokollieren und ggf. Benachrichtigung senden
}); 