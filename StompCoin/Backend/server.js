const express = require('express');
const mongoose = require('mongoose');
const session = require('express-session');
const MongoStore = require('connect-mongo');
const { 
    securityHeaders, 
    corsOptions, 
    dbSecurity,
    sessionConfig,
    apiLimiter,
    authLimiter
} = require('./config/security');
const {
    sqlInjectionProtection,
    xssProtection,
    validateToken,
    checkIPBlacklist,
    securityLogging,
    csrfProtection,
    validateContentType,
    requestSizeLimit,
    bruteForceProtection,
    validateRequestSignature,
    require2FA,
    apiKeyRotation,
    validateMiningRequest,
    validateEventRequest,
    validateTransaction
} = require('./middleware/security');

require('dotenv').config();

const app = express();

// Basis-Sicherheits-Header
app.use(securityHeaders);

// CORS-Konfiguration
app.use(cors(corsOptions));

// Body-Parser mit Größenlimit
app.use(express.json({ limit: '1mb' }));
app.use(express.urlencoded({ extended: true, limit: '1mb' }));

// Session-Konfiguration
app.use(session({
    ...sessionConfig,
    store: MongoStore.create({
        mongoUrl: process.env.MONGODB_URI,
        ttl: 24 * 60 * 60 // 1 Tag
    })
}));

// Rate Limiting
app.use('/api/', apiLimiter);
app.use('/api/auth/', authLimiter);

// Basis-Sicherheits-Middleware
app.use(sqlInjectionProtection);
app.use(xssProtection);
app.use(checkIPBlacklist);
app.use(securityLogging);
app.use(csrfProtection);
app.use(validateContentType);
app.use(requestSizeLimit);
app.use(bruteForceProtection);
app.use(validateRequestSignature);
app.use(apiKeyRotation);

// MongoDB Verbindung
mongoose.connect(process.env.MONGODB_URI, dbSecurity)
    .then(() => console.log('MongoDB verbunden'))
    .catch(err => console.error('MongoDB Verbindungsfehler:', err));

// Routen mit spezifischen Sicherheitsmaßnahmen
app.use('/api/auth', require('./routes/auth'));

// Geschützte Routen mit 2FA für kritische Operationen
app.use('/api/users', validateToken, require2FA, require('./routes/users'));
app.use('/api/events', validateToken, validateEventRequest, require('./routes/events'));
app.use('/api/mining', validateToken, validateMiningRequest, require('./routes/mining'));
app.use('/api/leaderboard', validateToken, require('./routes/leaderboard'));

// Transaktionsrouten mit zusätzlicher Validierung
app.use('/api/transfer', validateToken, require2FA, validateTransaction, require('./routes/transfer'));
app.use('/api/reward', validateToken, validateTransaction, require('./routes/reward'));

// Fehlerbehandlung
app.use((err, req, res, next) => {
    console.error(err.stack);
    
    // Keine sensiblen Fehlerinformationen im Produktionsmodus
    const error = process.env.NODE_ENV === 'production' 
        ? 'Ein Fehler ist aufgetreten' 
        : err.message;
    
    res.status(err.status || 500).json({ error });
});

// 404 Handler
app.use((req, res) => {
    res.status(404).json({ error: 'Route nicht gefunden' });
});

// Server starten
const PORT = process.env.PORT || 3000;
app.listen(PORT, () => {
    console.log(`Server läuft auf Port ${PORT}`);
    console.log(`Umgebung: ${process.env.NODE_ENV}`);
}); 