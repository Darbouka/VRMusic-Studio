const express = require('express');
const router = express.Router();
const User = require('../models/User');
const auth = require('../middleware/auth');
const { validateUserUpdate } = require('../middleware/validation');

// Benutzerprofil abrufen
router.get('/profile', auth, async (req, res) => {
    try {
        const user = await User.findById(req.user.userId)
            .select('-password')
            .populate('events.eventId', 'name startDate endDate');

        if (!user) {
            return res.status(404).json({ error: 'Benutzer nicht gefunden' });
        }

        res.json(user);
    } catch (error) {
        console.error(error);
        res.status(500).json({ error: 'Server-Fehler beim Abrufen des Profils' });
    }
});

// Benutzerprofil aktualisieren
router.put('/profile', [auth, validateUserUpdate], async (req, res) => {
    try {
        const { username, email, preferences } = req.body;
        const user = await User.findById(req.user.userId);

        if (!user) {
            return res.status(404).json({ error: 'Benutzer nicht gefunden' });
        }

        // Überprüfen ob Benutzername oder E-Mail bereits existiert
        if (username && username !== user.username) {
            const existingUser = await User.findOne({ username });
            if (existingUser) {
                return res.status(400).json({ error: 'Benutzername bereits vergeben' });
            }
            user.username = username;
        }

        if (email && email !== user.email) {
            const existingUser = await User.findOne({ email });
            if (existingUser) {
                return res.status(400).json({ error: 'E-Mail bereits vergeben' });
            }
            user.email = email;
        }

        if (preferences) {
            user.preferences = { ...user.preferences, ...preferences };
        }

        await user.save();

        res.json({
            message: 'Profil erfolgreich aktualisiert',
            user: {
                id: user._id,
                username: user.username,
                email: user.email,
                preferences: user.preferences
            }
        });
    } catch (error) {
        console.error(error);
        res.status(500).json({ error: 'Server-Fehler beim Aktualisieren des Profils' });
    }
});

// Benutzerstatistiken abrufen
router.get('/stats', auth, async (req, res) => {
    try {
        const user = await User.findById(req.user.userId);
        
        if (!user) {
            return res.status(404).json({ error: 'Benutzer nicht gefunden' });
        }

        const stats = {
            totalSteps: user.steps.reduce((sum, step) => sum + step.count, 0),
            totalCoins: user.coinBalance,
            eventsAttended: user.events.filter(e => e.status === 'completed').length,
            miningStats: user.miningStats,
            premiumStatus: {
                isPremium: user.isPremium,
                expiryDate: user.premiumExpiry
            }
        };

        res.json(stats);
    } catch (error) {
        console.error(error);
        res.status(500).json({ error: 'Server-Fehler beim Abrufen der Statistiken' });
    }
});

// Benutzer löschen
router.delete('/profile', auth, async (req, res) => {
    try {
        const user = await User.findById(req.user.userId);
        
        if (!user) {
            return res.status(404).json({ error: 'Benutzer nicht gefunden' });
        }

        await user.remove();

        res.json({ message: 'Benutzer erfolgreich gelöscht' });
    } catch (error) {
        console.error(error);
        res.status(500).json({ error: 'Server-Fehler beim Löschen des Benutzers' });
    }
});

// Benutzerpräferenzen aktualisieren
router.patch('/preferences', auth, async (req, res) => {
    try {
        const { preferences } = req.body;
        const user = await User.findById(req.user.userId);

        if (!user) {
            return res.status(404).json({ error: 'Benutzer nicht gefunden' });
        }

        user.preferences = { ...user.preferences, ...preferences };
        await user.save();

        res.json({
            message: 'Präferenzen erfolgreich aktualisiert',
            preferences: user.preferences
        });
    } catch (error) {
        console.error(error);
        res.status(500).json({ error: 'Server-Fehler beim Aktualisieren der Präferenzen' });
    }
});

module.exports = router; 