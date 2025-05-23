const { sanitizeInput } = require('../config/security');
const { verifyToken } = require('../config/security');
const crypto = require('crypto');

// SQL-Injection-Schutz
const sqlInjectionProtection = (req, res, next) => {
    const sqlInjectionPattern = /(\b(SELECT|INSERT|UPDATE|DELETE|DROP|UNION|ALTER|EXEC|DECLARE)\b)/i;
    
    const checkObject = (obj) => {
        for (let key in obj) {
            if (typeof obj[key] === 'object') {
                checkObject(obj[key]);
            } else if (typeof obj[key] === 'string' && sqlInjectionPattern.test(obj[key])) {
                throw new Error('Ungültige Zeichen in der Anfrage');
            }
        }
    };

    try {
        checkObject(req.body);
        checkObject(req.query);
        checkObject(req.params);
        next();
    } catch (error) {
        res.status(400).json({ error: 'Ungültige Anfrage' });
    }
};

// XSS-Schutz
const xssProtection = (req, res, next) => {
    const sanitizeObject = (obj) => {
        for (let key in obj) {
            if (typeof obj[key] === 'object') {
                sanitizeObject(obj[key]);
            } else if (typeof obj[key] === 'string') {
                obj[key] = sanitizeInput(obj[key]);
            }
        }
    };

    sanitizeObject(req.body);
    sanitizeObject(req.query);
    sanitizeObject(req.params);
    next();
};

// Token-Validierung mit erweiterten Prüfungen
const validateToken = async (req, res, next) => {
    try {
        const token = req.headers.authorization?.split(' ')[1];
        if (!token) {
            return res.status(401).json({ error: 'Kein Token vorhanden' });
        }

        const decoded = verifyToken(token);
        
        // Zusätzliche Token-Validierungen
        if (!decoded.id || !decoded.role) {
            throw new Error('Ungültiger Token');
        }

        // Token-Revocation-Check (optional)
        const isRevoked = await checkTokenRevocation(token);
        if (isRevoked) {
            throw new Error('Token wurde widerrufen');
        }

        req.user = decoded;
        next();
    } catch (error) {
        res.status(401).json({ error: 'Ungültiger Token' });
    }
};

// Request-Validierung
const validateRequest = (schema) => {
    return (req, res, next) => {
        try {
            const { error } = schema.validate(req.body);
            if (error) {
                throw error;
            }
            next();
        } catch (error) {
            res.status(400).json({ error: error.message });
        }
    };
};

// IP-Blacklist
const ipBlacklist = new Set();
const checkIPBlacklist = (req, res, next) => {
    const ip = req.ip;
    if (ipBlacklist.has(ip)) {
        return res.status(403).json({ error: 'IP ist gesperrt' });
    }
    next();
};

// Request-Logging für Sicherheitsanalysen
const securityLogging = (req, res, next) => {
    const logData = {
        timestamp: new Date().toISOString(),
        ip: req.ip,
        method: req.method,
        path: req.path,
        userAgent: req.headers['user-agent'],
        userId: req.user?.id
    };

    // Logging in Datei oder Datenbank
    console.log('Security Log:', logData);
    next();
};

// CSRF-Schutz
const csrfProtection = (req, res, next) => {
    if (req.method === 'GET') {
        return next();
    }

    const csrfToken = req.headers['x-csrf-token'];
    if (!csrfToken || csrfToken !== req.session.csrfToken) {
        return res.status(403).json({ error: 'CSRF-Token ungültig' });
    }
    next();
};

// Content-Type-Validierung
const validateContentType = (req, res, next) => {
    const allowedTypes = ['application/json', 'multipart/form-data'];
    const contentType = req.headers['content-type'];

    if (!contentType || !allowedTypes.some(type => contentType.includes(type))) {
        return res.status(415).json({ error: 'Nicht unterstützter Content-Type' });
    }
    next();
};

// Request-Größen-Limit
const requestSizeLimit = (req, res, next) => {
    const MAX_SIZE = 1024 * 1024; // 1MB
    if (req.headers['content-length'] > MAX_SIZE) {
        return res.status(413).json({ error: 'Anfrage zu groß' });
    }
    next();
};

// Brute-Force-Schutz
const loginAttempts = new Map();
const MAX_LOGIN_ATTEMPTS = 5;
const LOCKOUT_TIME = 15 * 60 * 1000; // 15 Minuten

const bruteForceProtection = (req, res, next) => {
    if (req.path === '/api/auth/login' && req.method === 'POST') {
        const ip = req.ip;
        const attempts = loginAttempts.get(ip) || { count: 0, timestamp: Date.now() };

        if (attempts.count >= MAX_LOGIN_ATTEMPTS) {
            const timeLeft = LOCKOUT_TIME - (Date.now() - attempts.timestamp);
            if (timeLeft > 0) {
                return res.status(429).json({
                    error: 'Zu viele Anmeldeversuche',
                    retryAfter: Math.ceil(timeLeft / 1000)
                });
            }
            loginAttempts.delete(ip);
        }
    }
    next();
};

// Request-Signatur-Validierung
const validateRequestSignature = (req, res, next) => {
    const timestamp = req.headers['x-timestamp'];
    const signature = req.headers['x-signature'];
    const clientId = req.headers['x-client-id'];

    if (!timestamp || !signature || !clientId) {
        return res.status(400).json({ error: 'Fehlende Authentifizierungsdaten' });
    }

    // Überprüfe, ob der Request nicht älter als 5 Minuten ist
    if (Date.now() - parseInt(timestamp) > 5 * 60 * 1000) {
        return res.status(400).json({ error: 'Request abgelaufen' });
    }

    // Validiere die Signatur
    const expectedSignature = crypto
        .createHmac('sha256', process.env.API_SECRET)
        .update(`${timestamp}${clientId}${JSON.stringify(req.body)}`)
        .digest('hex');

    if (signature !== expectedSignature) {
        return res.status(401).json({ error: 'Ungültige Signatur' });
    }

    next();
};

// Zwei-Faktor-Authentifizierung
const require2FA = async (req, res, next) => {
    if (!req.user) {
        return res.status(401).json({ error: 'Nicht authentifiziert' });
    }

    if (req.user.requires2FA && !req.headers['x-2fa-token']) {
        return res.status(403).json({ error: '2FA erforderlich' });
    }

    if (req.headers['x-2fa-token']) {
        try {
            const isValid = await verify2FAToken(req.user.id, req.headers['x-2fa-token']);
            if (!isValid) {
                return res.status(401).json({ error: 'Ungültiger 2FA-Token' });
            }
        } catch (error) {
            return res.status(500).json({ error: '2FA-Validierung fehlgeschlagen' });
        }
    }

    next();
};

// API-Schlüssel-Rotation
const apiKeyRotation = (req, res, next) => {
    const apiKey = req.headers['x-api-key'];
    if (!apiKey) {
        return res.status(401).json({ error: 'API-Schlüssel fehlt' });
    }

    // Überprüfe, ob der API-Schlüssel bald abläuft
    const keyExpiry = getApiKeyExpiry(apiKey);
    if (keyExpiry && keyExpiry - Date.now() < 7 * 24 * 60 * 60 * 1000) { // 7 Tage
        res.setHeader('X-API-Key-Expiry-Warning', 'true');
    }

    next();
};

// Request-Validierung für Mining-Operationen
const validateMiningRequest = (req, res, next) => {
    if (req.path.startsWith('/api/mining')) {
        const { type, amount } = req.body;
        
        // Überprüfe Mining-Typ
        if (!['solo', 'pool'].includes(type)) {
            return res.status(400).json({ error: 'Ungültiger Mining-Typ' });
        }

        // Überprüfe Mining-Betrag
        if (amount && (isNaN(amount) || amount <= 0 || amount > 1000)) {
            return res.status(400).json({ error: 'Ungültiger Mining-Betrag' });
        }

        // Überprüfe Mining-Frequenz
        const lastMiningTime = req.user.lastMiningTime;
        if (lastMiningTime && Date.now() - lastMiningTime < 60000) { // 1 Minute
            return res.status(429).json({ error: 'Zu häufige Mining-Anfragen' });
        }
    }
    next();
};

// Event-Sicherheit
const validateEventRequest = (req, res, next) => {
    if (req.path.startsWith('/api/events')) {
        const { startDate, endDate, maxParticipants } = req.body;

        // Überprüfe Datumsvalidität
        if (startDate && endDate) {
            const start = new Date(startDate);
            const end = new Date(endDate);
            if (start >= end || start < new Date()) {
                return res.status(400).json({ error: 'Ungültige Event-Zeitangaben' });
            }
        }

        // Überprüfe Teilnehmerlimit
        if (maxParticipants && (maxParticipants < 1 || maxParticipants > 1000)) {
            return res.status(400).json({ error: 'Ungültige Teilnehmeranzahl' });
        }
    }
    next();
};

// Coin-Transaktionssicherheit
const validateTransaction = (req, res, next) => {
    if (req.path.includes('/transfer') || req.path.includes('/reward')) {
        const { amount, recipientId } = req.body;

        // Überprüfe Transaktionsbetrag
        if (!amount || amount <= 0 || amount > 10000) {
            return res.status(400).json({ error: 'Ungültiger Transaktionsbetrag' });
        }

        // Überprüfe Empfänger
        if (!recipientId || recipientId === req.user.id) {
            return res.status(400).json({ error: 'Ungültiger Empfänger' });
        }

        // Überprüfe Kontostand
        if (req.user.coinBalance < amount) {
            return res.status(400).json({ error: 'Unzureichender Kontostand' });
        }
    }
    next();
};

module.exports = {
    sqlInjectionProtection,
    xssProtection,
    validateToken,
    validateRequest,
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
}; 