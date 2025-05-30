const mongoose = require('mongoose');

const challengeSchema = new mongoose.Schema({
    title: {
        type: String,
        required: true,
        trim: true,
        minlength: 3,
        maxlength: 100
    },
    description: {
        type: String,
        required: true,
        trim: true,
        maxlength: 1000
    },
    creator: {
        type: mongoose.Schema.Types.ObjectId,
        ref: 'User',
        required: true
    },
    type: {
        type: String,
        enum: ['steps', 'calories', 'distance', 'duration', 'custom'],
        required: true
    },
    goal: {
        value: {
            type: Number,
            required: true,
            min: 0
        },
        unit: {
            type: String,
            enum: ['steps', 'kcal', 'km', 'minutes', 'custom'],
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
    participants: [{
        user: {
            type: mongoose.Schema.Types.ObjectId,
            ref: 'User'
        },
        progress: {
            type: Number,
            default: 0
        },
        joinedAt: {
            type: Date,
            default: Date.now
        },
        completedAt: Date,
        rewards: [{
            type: mongoose.Schema.Types.ObjectId,
            ref: 'Transaction'
        }]
    }],
    rewards: {
        coins: {
            type: Number,
            default: 0
        },
        badges: [{
            name: String,
            description: String,
            image: String
        }]
    },
    rules: {
        minParticipants: {
            type: Number,
            default: 1
        },
        maxParticipants: {
            type: Number,
            default: 1000
        },
        allowLateJoin: {
            type: Boolean,
            default: true
        },
        requireVerification: {
            type: Boolean,
            default: false
        }
    },
    status: {
        type: String,
        enum: ['upcoming', 'active', 'completed', 'cancelled'],
        default: 'upcoming'
    },
    leaderboard: [{
        user: {
            type: mongoose.Schema.Types.ObjectId,
            ref: 'User'
        },
        progress: Number,
        rank: Number,
        lastUpdated: Date
    }],
    group: {
        type: mongoose.Schema.Types.ObjectId,
        ref: 'Group'
    },
    tags: [{
        type: String,
        trim: true
    }],
    isPublic: {
        type: Boolean,
        default: true
    }
}, {
    timestamps: true
});

// Indizes für schnelle Abfragen
challengeSchema.index({ creator: 1, status: 1 });
challengeSchema.index({ 'participants.user': 1 });
challengeSchema.index({ startDate: 1, endDate: 1 });
challengeSchema.index({ type: 1, status: 1 });
challengeSchema.index({ tags: 1 });

// Methode zum Hinzufügen eines Teilnehmers
challengeSchema.methods.addParticipant = async function(userId) {
    if (this.status !== 'upcoming' && this.status !== 'active') {
        throw new Error('Challenge ist nicht mehr aktiv');
    }

    if (!this.rules.allowLateJoin && this.status === 'active') {
        throw new Error('Späte Teilnahme nicht erlaubt');
    }

    if (this.participants.length >= this.rules.maxParticipants) {
        throw new Error('Maximale Teilnehmerzahl erreicht');
    }

    if (this.participants.some(p => p.user.toString() === userId.toString())) {
        throw new Error('Benutzer ist bereits Teilnehmer');
    }

    this.participants.push({
        user: userId,
        progress: 0,
        joinedAt: new Date()
    });

    await this.save();
};

// Methode zum Aktualisieren des Fortschritts
challengeSchema.methods.updateProgress = async function(userId, progress) {
    const participant = this.participants.find(
        p => p.user.toString() === userId.toString()
    );

    if (!participant) {
        throw new Error('Benutzer ist kein Teilnehmer der Challenge');
    }

    participant.progress = progress;

    // Überprüfen, ob das Ziel erreicht wurde
    if (progress >= this.goal.value && !participant.completedAt) {
        participant.completedAt = new Date();
        await this.distributeRewards(userId);
    }

    // Leaderboard aktualisieren
    await this.updateLeaderboard();

    await this.save();
};

// Methode zum Verteilen der Belohnungen
challengeSchema.methods.distributeRewards = async function(userId) {
    const participant = this.participants.find(
        p => p.user.toString() === userId.toString()
    );

    if (!participant) return;

    // Coins verteilen
    if (this.rewards.coins > 0) {
        const transaction = await this.model('Transaction').create({
            type: 'reward',
            amount: this.rewards.coins,
            toUser: userId,
            metadata: {
                challengeId: this._id,
                challengeTitle: this.title
            }
        });

        participant.rewards.push(transaction._id);
    }

    // Badges verteilen
    if (this.rewards.badges.length > 0) {
        // Badge-Logik implementieren
    }

    await this.save();
};

// Methode zum Aktualisieren des Leaderboards
challengeSchema.methods.updateLeaderboard = async function() {
    this.leaderboard = this.participants
        .map(p => ({
            user: p.user,
            progress: p.progress,
            lastUpdated: new Date()
        }))
        .sort((a, b) => b.progress - a.progress)
        .map((entry, index) => ({
            ...entry,
            rank: index + 1
        }));

    await this.save();
};

// Statische Methode zum Suchen von Challenges
challengeSchema.statics.searchChallenges = async function(query, options = {}) {
    const {
        type,
        status,
        isPublic = true,
        startDate,
        endDate,
        limit = 20,
        skip = 0
    } = options;

    const searchQuery = {
        $text: { $search: query },
        isPublic
    };

    if (type) searchQuery.type = type;
    if (status) searchQuery.status = status;
    if (startDate) searchQuery.startDate = { $gte: startDate };
    if (endDate) searchQuery.endDate = { $lte: endDate };

    return this.find(searchQuery)
        .sort({ score: { $meta: 'textScore' } })
        .skip(skip)
        .limit(limit)
        .populate('creator', 'username')
        .populate('participants.user', 'username');
};

const Challenge = mongoose.model('Challenge', challengeSchema);

module.exports = Challenge; 