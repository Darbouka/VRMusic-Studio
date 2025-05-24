const express = require('express');
const router = express.Router();
const jwt = require('jsonwebtoken');
const User = require('../models/User');
const auth = require('../middleware/auth');

// Registrierung
router.post('/register', async (req, res) => {
    try {
        const { username, email, password } = req.body;

        // Überprüfen ob Benutzer bereits existiert
        let user = await User.findOne({ $or: [{ email }, { username }] });
        if (user) {
            return res.status(400).json({
                error: 'Benutzer mit dieser E-Mail oder diesem Benutzernamen existiert bereits'
            });
        }

        // Neuen Benutzer erstellen
        user = new User({
            username,
            email,
            password
        });

        await user.save();

        // JWT Token generieren
        const token = jwt.sign(
            { userId: user._id },
            process.env.JWT_SECRET,
            { expiresIn: '24h' }
        );

        res.status(201).json({
            token,
            user: {
                id: user._id,
                username: user.username,
                email: user.email,
                isPremium: user.isPremium
            }
        });
    } catch (error) {
        console.error(error);
        res.status(500).json({ error: 'Server-Fehler bei der Registrierung' });
    }
});

// Login
router.post('/login', async (req, res) => {
    try {
        const { email, password } = req.body;

        // Benutzer finden
        const user = await User.findOne({ email });
        if (!user) {
            return res.status(400).json({ error: 'Ungültige Anmeldedaten' });
        }

        // Passwort überprüfen
        const isMatch = await user.comparePassword(password);
        if (!isMatch) {
            return res.status(400).json({ error: 'Ungültige Anmeldedaten' });
        }

        // JWT Token generieren
        const token = jwt.sign(
            { userId: user._id },
            process.env.JWT_SECRET,
            { expiresIn: '24h' }
        );

        // Last Login aktualisieren
        user.lastLogin = new Date();
        await user.save();

        res.json({
            token,
            user: {
                id: user._id,
                username: user.username,
                email: user.email,
                isPremium: user.isPremium
            }
        });
    } catch (error) {
        console.error(error);
        res.status(500).json({ error: 'Server-Fehler beim Login' });
    }
});

// Benutzer abrufen
router.get('/me', auth, async (req, res) => {
    try {
        const user = await User.findById(req.user.userId).select('-password');
        if (!user) {
            return res.status(404).json({ error: 'Benutzer nicht gefunden' });
        }
        res.json(user);
    } catch (error) {
        console.error(error);
        res.status(500).json({ error: 'Server-Fehler beim Abrufen des Benutzers' });
    }
});

// Passwort zurücksetzen
router.post('/reset-password', async (req, res) => {
    try {
        const { email } = req.body;
        const user = await User.findOne({ email });
        
        if (!user) {
            return res.status(404).json({ error: 'Benutzer nicht gefunden' });
        }

        // Hier würde normalerweise ein Reset-Token generiert und per E-Mail versendet werden
        // Für dieses Beispiel senden wir nur eine Bestätigung
        res.json({ message: 'Passwort-Reset-E-Mail wurde versendet' });
    } catch (error) {
        console.error(error);
        res.status(500).json({ error: 'Server-Fehler beim Zurücksetzen des Passworts' });
    }
});

module.exports = router; 