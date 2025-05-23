const crypto = require('crypto');
const bcrypt = require('bcrypt');
const jwt = require('jsonwebtoken');
const rateLimit = require('express-rate-limit');
const helmet = require('helmet');
const cors = require('cors');

// Verschlüsselungskonfiguration
const ENCRYPTION_KEY = process.env.ENCRYPTION_KEY || crypto.randomBytes(32);
const IV_LENGTH = 16;
const ALGORITHM = 'aes-256-gcm';

// JWT Konfiguration
const JWT_SECRET = process.env.JWT_SECRET || crypto.randomBytes(64).toString('hex');
const JWT_EXPIRATION = '24h';
const JWT_REFRESH_EXPIRATION = '7d';

// Verschlüsselungsfunktionen
const encrypt = (text) => {
    const iv = crypto.randomBytes(IV_LENGTH);
    const cipher = crypto.createCipheriv(ALGORITHM, Buffer.from(ENCRYPTION_KEY), iv);
    let encrypted = cipher.update(text, 'utf8', 'hex');
    encrypted += cipher.final('hex');
    const authTag = cipher.getAuthTag();
    return {
        iv: iv.toString('hex'),
        encryptedData: encrypted,
        authTag: authTag.toString('hex')
    };
};

const decrypt = (encryptedData, iv, authTag) => {
    const decipher = crypto.createDecipheriv(
        ALGORITHM,
        Buffer.from(ENCRYPTION_KEY),
        Buffer.from(iv, 'hex')
    );
    decipher.setAuthTag(Buffer.from(authTag, 'hex'));
    let decrypted = decipher.update(encryptedData, 'hex', 'utf8');
    decrypted += decipher.final('utf8');
    return decrypted;
};

// Passwort-Hashing
const SALT_ROUNDS = 12;
const hashPassword = async (password) => {
    return await bcrypt.hash(password, SALT_ROUNDS);
};

const comparePassword = async (password, hash) => {
    return await bcrypt.compare(password, hash);
};

// JWT Funktionen
const generateToken = (user) => {
    return jwt.sign(
        { 
            id: user._id,
            role: user.role,
            isPremium: user.isPremium
        },
        JWT_SECRET,
        { expiresIn: JWT_EXPIRATION }
    );
};

const generateRefreshToken = (user) => {
    return jwt.sign(
        { id: user._id },
        JWT_SECRET,
        { expiresIn: JWT_REFRESH_EXPIRATION }
    );
};

const verifyToken = (token) => {
    try {
        return jwt.verify(token, JWT_SECRET);
    } catch (error) {
        throw new Error('Ungültiger Token');
    }
};

// Rate Limiting
const apiLimiter = rateLimit({
    windowMs: 15 * 60 * 1000, // 15 Minuten
    max: 100, // Maximal 100 Anfragen pro IP
    message: 'Zu viele Anfragen von dieser IP, bitte versuchen Sie es später erneut'
});

const authLimiter = rateLimit({
    windowMs: 60 * 60 * 1000, // 1 Stunde
    max: 5, // Maximal 5 fehlgeschlagene Anmeldeversuche
    message: 'Zu viele fehlgeschlagene Anmeldeversuche, bitte versuchen Sie es später erneut'
});

// Sicherheits-Header
const securityHeaders = helmet({
    contentSecurityPolicy: {
        directives: {
            defaultSrc: ["'self'"],
            scriptSrc: ["'self'", "'unsafe-inline'"],
            styleSrc: ["'self'", "'unsafe-inline'"],
            imgSrc: ["'self'", "data:", "https:"],
            connectSrc: ["'self'"],
            fontSrc: ["'self'"],
            objectSrc: ["'none'"],
            mediaSrc: ["'self'"],
            frameSrc: ["'none'"]
        }
    },
    crossOriginEmbedderPolicy: true,
    crossOriginOpenerPolicy: true,
    crossOriginResourcePolicy: { policy: "same-site" },
    dnsPrefetchControl: { allow: false },
    frameguard: { action: "deny" },
    hidePoweredBy: true,
    hsts: {
        maxAge: 31536000,
        includeSubDomains: true,
        preload: true
    },
    ieNoOpen: true,
    noSniff: true,
    referrerPolicy: { policy: "strict-origin-when-cross-origin" },
    xssFilter: true
});

// CORS Konfiguration
const corsOptions = {
    origin: process.env.ALLOWED_ORIGINS?.split(',') || ['https://stompcoin.com'],
    methods: ['GET', 'POST', 'PUT', 'DELETE', 'PATCH'],
    allowedHeaders: ['Content-Type', 'Authorization'],
    exposedHeaders: ['X-Total-Count'],
    credentials: true,
    maxAge: 86400 // 24 Stunden
};

// Datenbank-Sicherheit
const dbSecurity = {
    ssl: process.env.NODE_ENV === 'production',
    sslValidate: true,
    sslCA: process.env.MONGODB_SSL_CA,
    authSource: 'admin',
    w: 'majority',
    wtimeout: 2500,
    useNewUrlParser: true,
    useUnifiedTopology: true
};

// Input-Validierung
const sanitizeInput = (input) => {
    if (typeof input === 'string') {
        return input.replace(/[<>]/g, '');
    }
    return input;
};

// Session-Sicherheit
const sessionConfig = {
    secret: process.env.SESSION_SECRET || crypto.randomBytes(32).toString('hex'),
    resave: false,
    saveUninitialized: false,
    cookie: {
        secure: process.env.NODE_ENV === 'production',
        httpOnly: true,
        sameSite: 'strict',
        maxAge: 24 * 60 * 60 * 1000 // 24 Stunden
    }
};

module.exports = {
    encrypt,
    decrypt,
    hashPassword,
    comparePassword,
    generateToken,
    generateRefreshToken,
    verifyToken,
    apiLimiter,
    authLimiter,
    securityHeaders,
    corsOptions,
    dbSecurity,
    sanitizeInput,
    sessionConfig
}; 