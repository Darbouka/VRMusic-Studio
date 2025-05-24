const { body, validationResult } = require('express-validator');

// Validierung für Benutzeraktualisierungen
exports.validateUserUpdate = [
    body('username')
        .optional()
        .isLength({ min: 3, max: 30 })
        .withMessage('Benutzername muss zwischen 3 und 30 Zeichen lang sein')
        .matches(/^[a-zA-Z0-9_]+$/)
        .withMessage('Benutzername darf nur Buchstaben, Zahlen und Unterstriche enthalten'),
    
    body('email')
        .optional()
        .isEmail()
        .withMessage('Ungültige E-Mail-Adresse')
        .normalizeEmail(),
    
    body('preferences')
        .optional()
        .isObject()
        .withMessage('Präferenzen müssen ein Objekt sein'),
    
    (req, res, next) => {
        const errors = validationResult(req);
        if (!errors.isEmpty()) {
            return res.status(400).json({ errors: errors.array() });
        }
        next();
    }
];

// Validierung für Event-Erstellung
exports.validateEventCreation = [
    body('name')
        .isLength({ min: 3, max: 100 })
        .withMessage('Eventname muss zwischen 3 und 100 Zeichen lang sein'),
    
    body('description')
        .isLength({ min: 10, max: 1000 })
        .withMessage('Beschreibung muss zwischen 10 und 1000 Zeichen lang sein'),
    
    body('location')
        .isObject()
        .withMessage('Standort muss ein Objekt sein'),
    
    body('location.coordinates')
        .isArray({ min: 2, max: 2 })
        .withMessage('Koordinaten müssen ein Array mit 2 Elementen sein'),
    
    body('startDate')
        .isISO8601()
        .withMessage('Ungültiges Startdatum'),
    
    body('endDate')
        .isISO8601()
        .withMessage('Ungültiges Enddatum')
        .custom((endDate, { req }) => {
            if (new Date(endDate) <= new Date(req.body.startDate)) {
                throw new Error('Enddatum muss nach dem Startdatum liegen');
            }
            return true;
        }),
    
    body('genre')
        .isIn(['goa', 'techno', 'electronic'])
        .withMessage('Ungültiges Genre'),
    
    body('floor')
        .isIn(['goa', 'techno'])
        .withMessage('Ungültiger Floor'),
    
    body('maxParticipants')
        .isInt({ min: 1 })
        .withMessage('Maximale Teilnehmerzahl muss eine positive Zahl sein'),
    
    (req, res, next) => {
        const errors = validationResult(req);
        if (!errors.isEmpty()) {
            return res.status(400).json({ errors: errors.array() });
        }
        next();
    }
];

// Validierung für Mining-Operationen
exports.validateMiningOperation = [
    body('type')
        .isIn(['pool', 'solo'])
        .withMessage('Ungültiger Mining-Typ'),
    
    body('poolId')
        .optional()
        .isMongoId()
        .withMessage('Ungültige Pool-ID'),
    
    (req, res, next) => {
        const errors = validationResult(req);
        if (!errors.isEmpty()) {
            return res.status(400).json({ errors: errors.array() });
        }
        next();
    }
];

// Validierung für Schritt-Synchronisation
exports.validateStepSync = [
    body('count')
        .isInt({ min: 0 })
        .withMessage('Schrittzahl muss eine positive Zahl sein'),
    
    body('type')
        .isIn(['dance', 'fitness', 'walk'])
        .withMessage('Ungültiger Schritt-Typ'),
    
    (req, res, next) => {
        const errors = validationResult(req);
        if (!errors.isEmpty()) {
            return res.status(400).json({ errors: errors.array() });
        }
        next();
    }
]; 