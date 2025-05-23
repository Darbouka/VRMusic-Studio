const express = require('express');
const router = express.Router();
const User = require('../models/User');
const auth = require('../middleware/auth');

// Schritte synchronisieren
router.post('/sync', auth, async (req, res) => {
    try {
        const { count, type } = req.body;
        const user = await User.findById(req.user.userId);

        if (!user) {
            return res.status(404).json({ error: 'Benutzer nicht gefunden' });
        }

        // Schritte hinzufügen
        await user.addSteps(count, type);

        // Coins berechnen und aktualisieren
        const coinReward = calculateCoinReward(count, type, user.isPremium);
        await user.updateCoinBalance(coinReward);

        res.json({
            message: 'Schritte erfolgreich synchronisiert',
            coinsEarned: coinReward,
            totalBalance: user.coinBalance
        });
    } catch (error) {
        console.error(error);
        res.status(500).json({ error: 'Server-Fehler bei der Synchronisation' });
    }
});

// Schritthistorie abrufen
router.get('/history', auth, async (req, res) => {
    try {
        const { timeFrame } = req.query;
        const user = await User.findById(req.user.userId);

        if (!user) {
            return res.status(404).json({ error: 'Benutzer nicht gefunden' });
        }

        let filteredSteps = user.steps;
        const now = new Date();

        // Zeitraum filtern
        switch (timeFrame) {
            case 'daily':
                const startOfDay = new Date(now.setHours(0, 0, 0, 0));
                filteredSteps = user.steps.filter(step => step.timestamp >= startOfDay);
                break;
            case 'weekly':
                const startOfWeek = new Date(now.setDate(now.getDate() - 7));
                filteredSteps = user.steps.filter(step => step.timestamp >= startOfWeek);
                break;
            case 'monthly':
                const startOfMonth = new Date(now.setMonth(now.getMonth() - 1));
                filteredSteps = user.steps.filter(step => step.timestamp >= startOfMonth);
                break;
        }

        // Statistiken berechnen
        const stats = calculateStepStats(filteredSteps);

        res.json({
            steps: filteredSteps,
            stats
        });
    } catch (error) {
        console.error(error);
        res.status(500).json({ error: 'Server-Fehler beim Abrufen der Schritthistorie' });
    }
});

// Coin-Bestand abrufen
router.get('/balance', auth, async (req, res) => {
    try {
        const user = await User.findById(req.user.userId);

        if (!user) {
            return res.status(404).json({ error: 'Benutzer nicht gefunden' });
        }

        res.json({
            balance: user.coinBalance
        });
    } catch (error) {
        console.error(error);
        res.status(500).json({ error: 'Server-Fehler beim Abrufen des Coin-Bestands' });
    }
});

// Hilfsfunktionen
function calculateCoinReward(steps, type, isPremium) {
    let baseRate = 0.1; // Basis-Konvertierungsrate

    // Typ-spezifische Multiplikatoren
    switch (type) {
        case 'dance':
            baseRate *= 1.5;
            break;
        case 'fitness':
            baseRate *= 1.2;
            break;
        case 'walk':
            baseRate *= 1.0;
            break;
    }

    // Premium-Bonus
    if (isPremium) {
        baseRate *= 1.5;
    }

    return Math.floor(steps * baseRate);
}

function calculateStepStats(steps) {
    if (steps.length === 0) {
        return {
            total: 0,
            average: 0,
            max: 0,
            min: 0
        };
    }

    const total = steps.reduce((sum, step) => sum + step.count, 0);
    const average = total / steps.length;
    const max = Math.max(...steps.map(step => step.count));
    const min = Math.min(...steps.map(step => step.count));

    return {
        total,
        average: Math.round(average),
        max,
        min
    };
}

module.exports = router; 