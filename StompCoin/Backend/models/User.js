const mongoose = require('mongoose');
const bcrypt = require('bcrypt');

const userSchema = new mongoose.Schema({
    username: {
        type: String,
        required: true,
        unique: true,
        trim: true
    },
    email: {
        type: String,
        required: true,
        unique: true,
        trim: true,
        lowercase: true
    },
    password: {
        type: String,
        required: true
    },
    coinBalance: {
        type: Number,
        default: 0
    },
    isPremium: {
        type: Boolean,
        default: false
    },
    premiumExpiry: {
        type: Date
    },
    steps: [{
        count: Number,
        timestamp: Date,
        type: {
            type: String,
            enum: ['dance', 'fitness', 'walk']
        }
    }],
    miningStats: {
        poolMining: {
            isActive: Boolean,
            totalMined: Number,
            lastMined: Date
        },
        soloMining: {
            isActive: Boolean,
            totalMined: Number,
            lastMined: Date
        }
    },
    events: [{
        eventId: mongoose.Schema.Types.ObjectId,
        joinedAt: Date,
        status: {
            type: String,
            enum: ['registered', 'attended', 'completed']
        }
    }],
    createdAt: {
        type: Date,
        default: Date.now
    },
    lastLogin: Date
});

// Passwort-Hashing vor dem Speichern
userSchema.pre('save', async function(next) {
    if (!this.isModified('password')) return next();
    
    try {
        const salt = await bcrypt.genSalt(10);
        this.password = await bcrypt.hash(this.password, salt);
        next();
    } catch (error) {
        next(error);
    }
});

// Methode zum Passwort-Vergleich
userSchema.methods.comparePassword = async function(candidatePassword) {
    return bcrypt.compare(candidatePassword, this.password);
};

// Methode zum Aktualisieren des Coin-Bestands
userSchema.methods.updateCoinBalance = async function(amount) {
    this.coinBalance += amount;
    return this.save();
};

// Methode zum Hinzufügen von Schritten
userSchema.methods.addSteps = async function(count, type) {
    this.steps.push({
        count,
        timestamp: new Date(),
        type
    });
    return this.save();
};

// Statische Methode zum Finden von Benutzern nach Rang
userSchema.statics.findByRank = async function(timeFrame) {
    const now = new Date();
    let startDate;
    
    switch(timeFrame) {
        case 'daily':
            startDate = new Date(now.setHours(0, 0, 0, 0));
            break;
        case 'weekly':
            startDate = new Date(now.setDate(now.getDate() - 7));
            break;
        case 'monthly':
            startDate = new Date(now.setMonth(now.getMonth() - 1));
            break;
        default:
            startDate = new Date(0);
    }
    
    return this.aggregate([
        {
            $match: {
                'steps.timestamp': { $gte: startDate }
            }
        },
        {
            $project: {
                username: 1,
                totalSteps: {
                    $sum: '$steps.count'
                }
            }
        },
        {
            $sort: { totalSteps: -1 }
        }
    ]);
};

module.exports = mongoose.model('User', userSchema); 