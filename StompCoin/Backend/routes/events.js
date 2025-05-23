const express = require('express');
const router = express.Router();
const Event = require('../models/Event');
const User = require('../models/User');
const auth = require('../middleware/auth');

// Events in der Nähe abrufen
router.get('/nearby', auth, async (req, res) => {
    try {
        const { longitude, latitude, maxDistance } = req.query;
        
        if (!longitude || !latitude) {
            return res.status(400).json({ error: 'Standort erforderlich' });
        }

        const events = await Event.findNearby(
            parseFloat(longitude),
            parseFloat(latitude),
            maxDistance ? parseInt(maxDistance) : 10000
        );

        res.json(events);
    } catch (error) {
        console.error(error);
        res.status(500).json({ error: 'Server-Fehler beim Abrufen der Events' });
    }
});

// Event erstellen
router.post('/', auth, async (req, res) => {
    try {
        const {
            name,
            description,
            location,
            startDate,
            endDate,
            genre,
            floor,
            maxParticipants,
            rewards
        } = req.body;

        const event = new Event({
            name,
            description,
            location: {
                type: 'Point',
                coordinates: [location.longitude, location.latitude]
            },
            startDate,
            endDate,
            genre,
            floor,
            maxParticipants,
            rewards,
            createdBy: req.user.userId
        });

        await event.save();

        res.status(201).json(event);
    } catch (error) {
        console.error(error);
        res.status(500).json({ error: 'Server-Fehler beim Erstellen des Events' });
    }
});

// Event beitreten
router.post('/:eventId/join', auth, async (req, res) => {
    try {
        const event = await Event.findById(req.params.eventId);
        
        if (!event) {
            return res.status(404).json({ error: 'Event nicht gefunden' });
        }

        if (event.status !== 'upcoming') {
            return res.status(400).json({ error: 'Event ist nicht mehr verfügbar' });
        }

        await event.addParticipant(req.user.userId);

        // Event zum Benutzer hinzufügen
        await User.findByIdAndUpdate(req.user.userId, {
            $push: {
                events: {
                    eventId: event._id,
                    joinedAt: new Date(),
                    status: 'registered'
                }
            }
        });

        res.json({ message: 'Erfolgreich zum Event angemeldet' });
    } catch (error) {
        console.error(error);
        res.status(500).json({ error: 'Server-Fehler beim Beitreten des Events' });
    }
});

// Event-Details abrufen
router.get('/:eventId', auth, async (req, res) => {
    try {
        const event = await Event.findById(req.params.eventId)
            .populate('participants.userId', 'username');

        if (!event) {
            return res.status(404).json({ error: 'Event nicht gefunden' });
        }

        res.json(event);
    } catch (error) {
        console.error(error);
        res.status(500).json({ error: 'Server-Fehler beim Abrufen der Event-Details' });
    }
});

// Events nach Genre abrufen
router.get('/genre/:genre', auth, async (req, res) => {
    try {
        const events = await Event.findByGenre(req.params.genre);
        res.json(events);
    } catch (error) {
        console.error(error);
        res.status(500).json({ error: 'Server-Fehler beim Abrufen der Events' });
    }
});

// Event-Status aktualisieren
router.patch('/:eventId/status', auth, async (req, res) => {
    try {
        const { status } = req.body;
        const event = await Event.findById(req.params.eventId);

        if (!event) {
            return res.status(404).json({ error: 'Event nicht gefunden' });
        }

        // Nur der Ersteller kann den Status ändern
        if (event.createdBy.toString() !== req.user.userId) {
            return res.status(403).json({ error: 'Nicht autorisiert' });
        }

        await event.updateStatus(status);

        // Wenn das Event abgeschlossen ist, Belohnungen verteilen
        if (status === 'completed') {
            await distributeEventRewards(event);
        }

        res.json(event);
    } catch (error) {
        console.error(error);
        res.status(500).json({ error: 'Server-Fehler beim Aktualisieren des Event-Status' });
    }
});

// Hilfsfunktion zum Verteilen der Event-Belohnungen
async function distributeEventRewards(event) {
    const participants = event.participants.filter(p => p.status === 'attended');
    
    for (const participant of participants) {
        await User.findByIdAndUpdate(participant.userId, {
            $inc: { coinBalance: event.rewards.coins }
        });
    }
}

module.exports = router; 