const mongoose = require('mongoose');

const eventSchema = new mongoose.Schema({
    name: {
        type: String,
        required: true,
        trim: true
    },
    description: {
        type: String,
        required: true
    },
    location: {
        type: {
            type: String,
            enum: ['Point'],
            default: 'Point'
        },
        coordinates: {
            type: [Number],
            required: true
        }
    },
    startDate: {
        type: Date,
        required: true
    },
    endDate: {
        type: Date,
        required: true
    },
    genre: {
        type: String,
        required: true,
        enum: ['goa', 'techno', 'electronic']
    },
    floor: {
        type: String,
        required: true,
        enum: ['goa', 'techno']
    },
    maxParticipants: {
        type: Number,
        required: true
    },
    currentParticipants: {
        type: Number,
        default: 0
    },
    participants: [{
        userId: {
            type: mongoose.Schema.Types.ObjectId,
            ref: 'User'
        },
        joinedAt: {
            type: Date,
            default: Date.now
        },
        status: {
            type: String,
            enum: ['registered', 'attended', 'completed'],
            default: 'registered'
        }
    }],
    rewards: {
        coins: {
            type: Number,
            required: true
        },
        experience: {
            type: Number,
            required: true
        }
    },
    status: {
        type: String,
        enum: ['upcoming', 'active', 'completed', 'cancelled'],
        default: 'upcoming'
    },
    createdAt: {
        type: Date,
        default: Date.now
    },
    createdBy: {
        type: mongoose.Schema.Types.ObjectId,
        ref: 'User',
        required: true
    }
});

// Index für Geospatial-Abfragen
eventSchema.index({ location: '2dsphere' });

// Methode zum Finden nahegelegener Events
eventSchema.statics.findNearby = async function(longitude, latitude, maxDistance = 10000) {
    return this.find({
        location: {
            $near: {
                $geometry: {
                    type: 'Point',
                    coordinates: [longitude, latitude]
                },
                $maxDistance: maxDistance
            }
        },
        status: 'upcoming'
    });
};

// Methode zum Hinzufügen eines Teilnehmers
eventSchema.methods.addParticipant = async function(userId) {
    if (this.currentParticipants >= this.maxParticipants) {
        throw new Error('Event ist bereits voll');
    }
    
    if (this.participants.some(p => p.userId.toString() === userId.toString())) {
        throw new Error('Benutzer ist bereits angemeldet');
    }
    
    this.participants.push({ userId });
    this.currentParticipants += 1;
    return this.save();
};

// Methode zum Aktualisieren des Event-Status
eventSchema.methods.updateStatus = async function(newStatus) {
    this.status = newStatus;
    return this.save();
};

// Statische Methode zum Finden von Events nach Genre
eventSchema.statics.findByGenre = async function(genre) {
    return this.find({
        genre,
        status: 'upcoming'
    }).sort({ startDate: 1 });
};

module.exports = mongoose.model('Event', eventSchema); 