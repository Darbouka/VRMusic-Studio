const mongoose = require('mongoose');

const miningSchema = new mongoose.Schema({
    userId: {
        type: mongoose.Schema.Types.ObjectId,
        ref: 'User',
        required: true
    },
    type: {
        type: String,
        enum: ['pool', 'solo'],
        required: true
    },
    startTime: {
        type: Date,
        default: Date.now
    },
    endTime: Date,
    status: {
        type: String,
        enum: ['active', 'completed', 'cancelled'],
        default: 'active'
    },
    stats: {
        totalMined: {
            type: Number,
            default: 0
        },
        blocksFound: {
            type: Number,
            default: 0
        },
        lastMined: Date,
        researchPower: {
            type: Number,
            default: 0
        }
    },
    pool: {
        poolId: {
            type: mongoose.Schema.Types.ObjectId,
            ref: 'MiningPool'
        },
        contribution: {
            type: Number,
            default: 0
        }
    },
    rewards: [{
        amount: Number,
        timestamp: Date,
        type: {
            type: String,
            enum: ['block', 'pool', 'bonus']
        }
    }]
});

// Methode zum Aktualisieren der Mining-Statistiken
miningSchema.methods.updateStats = async function(minedAmount, blockFound = false) {
    this.stats.totalMined += minedAmount;
    this.stats.lastMined = new Date();
    
    if (blockFound) {
        this.stats.blocksFound += 1;
    }
    
    return this.save();
};

// Methode zum Beenden des Mining-Prozesses
miningSchema.methods.endMining = async function() {
    this.endTime = new Date();
    this.status = 'completed';
    return this.save();
};

// Statische Methode zum Finden aktiver Mining-Sessions
miningSchema.statics.findActive = async function(userId) {
    return this.find({
        userId,
        status: 'active'
    });
};

// Statische Methode zum Berechnen der Mining-Belohnungen
miningSchema.statics.calculateRewards = async function(miningSession) {
    const duration = (new Date() - miningSession.startTime) / 1000; // in Sekunden
    const baseRate = 0.1; // Basis-Mining-Rate pro Sekunde
    
    let reward = duration * baseRate;
    
    // Bonus für Pool-Mining
    if (miningSession.type === 'pool') {
        reward *= 1.2; // 20% Bonus
    }
    
    // Bonus für Premium-User
    const user = await mongoose.model('User').findById(miningSession.userId);
    if (user.isPremium) {
        reward *= 1.5; // 50% Bonus
    }
    
    return Math.floor(reward);
};

module.exports = mongoose.model('Mining', miningSchema); 