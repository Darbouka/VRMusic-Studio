const express = require('express');
const router = express.Router();
const Mining = require('../models/Mining');
const User = require('../models/User');
const auth = require('../middleware/auth');

// Mining starten
router.post('/start', auth, async (req, res) => {
    try {
        const { type, poolId } = req.body;
        const user = await User.findById(req.user.userId);

        if (!user) {
            return res.status(404).json({ error: 'Benutzer nicht gefunden' });
        }

        // Prüfen ob bereits eine aktive Mining-Session existiert
        const activeSession = await Mining.findActive(req.user.userId);
        if (activeSession.length > 0) {
            return res.status(400).json({ error: 'Bereits eine aktive Mining-Session vorhanden' });
        }

        // Neue Mining-Session erstellen
        const miningSession = new Mining({
            userId: req.user.userId,
            type,
            pool: poolId ? { poolId } : undefined
        });

        await miningSession.save();

        // Mining-Status im User-Profil aktualisieren
        if (type === 'pool') {
            user.miningStats.poolMining.isActive = true;
        } else {
            user.miningStats.soloMining.isActive = true;
        }
        await user.save();

        res.status(201).json(miningSession);
    } catch (error) {
        console.error(error);
        res.status(500).json({ error: 'Server-Fehler beim Starten des Mining' });
    }
});

// Mining beenden
router.post('/stop', auth, async (req, res) => {
    try {
        const activeSession = await Mining.findActive(req.user.userId);
        
        if (activeSession.length === 0) {
            return res.status(400).json({ error: 'Keine aktive Mining-Session gefunden' });
        }

        const session = activeSession[0];
        await session.endMining();

        // Belohnungen berechnen und verteilen
        const rewards = await Mining.calculateRewards(session);
        const user = await User.findById(req.user.userId);
        await user.updateCoinBalance(rewards);

        // Mining-Status im User-Profil aktualisieren
        if (session.type === 'pool') {
            user.miningStats.poolMining.isActive = false;
            user.miningStats.poolMining.totalMined += rewards;
        } else {
            user.miningStats.soloMining.isActive = false;
            user.miningStats.soloMining.totalMined += rewards;
        }
        await user.save();

        res.json({
            message: 'Mining erfolgreich beendet',
            rewards,
            totalBalance: user.coinBalance
        });
    } catch (error) {
        console.error(error);
        res.status(500).json({ error: 'Server-Fehler beim Beenden des Mining' });
    }
});

// Mining-Statistiken abrufen
router.get('/stats', auth, async (req, res) => {
    try {
        const user = await User.findById(req.user.userId);

        if (!user) {
            return res.status(404).json({ error: 'Benutzer nicht gefunden' });
        }

        const activeSession = await Mining.findActive(req.user.userId);
        const miningHistory = await Mining.find({
            userId: req.user.userId,
            status: 'completed'
        }).sort({ endTime: -1 }).limit(10);

        res.json({
            activeSession: activeSession[0] || null,
            stats: user.miningStats,
            history: miningHistory
        });
    } catch (error) {
        console.error(error);
        res.status(500).json({ error: 'Server-Fehler beim Abrufen der Mining-Statistiken' });
    }
});

// Mining-Belohnungen aktualisieren
router.post('/update-rewards', auth, async (req, res) => {
    try {
        const activeSession = await Mining.findActive(req.user.userId);
        
        if (activeSession.length === 0) {
            return res.status(400).json({ error: 'Keine aktive Mining-Session gefunden' });
        }

        const session = activeSession[0];
        const { minedAmount, blockFound } = req.body;

        await session.updateStats(minedAmount, blockFound);

        res.json({
            message: 'Mining-Statistiken aktualisiert',
            stats: session.stats
        });
    } catch (error) {
        console.error(error);
        res.status(500).json({ error: 'Server-Fehler beim Aktualisieren der Mining-Belohnungen' });
    }
});

// Pool-Mining Beitritt
router.post('/pool/join', auth, async (req, res) => {
    try {
        const { poolId } = req.body;
        const activeSession = await Mining.findActive(req.user.userId);

        if (activeSession.length === 0) {
            return res.status(400).json({ error: 'Keine aktive Mining-Session gefunden' });
        }

        const session = activeSession[0];
        if (session.type !== 'pool') {
            return res.status(400).json({ error: 'Nur Pool-Mining-Sessions können beitreten' });
        }

        session.pool = { poolId };
        await session.save();

        res.json({
            message: 'Erfolgreich dem Mining-Pool beigetreten',
            session
        });
    } catch (error) {
        console.error(error);
        res.status(500).json({ error: 'Server-Fehler beim Beitreten des Mining-Pools' });
    }
});

module.exports = router; 