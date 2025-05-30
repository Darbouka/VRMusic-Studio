const mongoose = require('mongoose');
const bcrypt = require('bcryptjs');

const userSchema = new mongoose.Schema({
    username: {
        type: String,
        required: true,
        unique: true,
        trim: true,
        minlength: 3,
        maxlength: 30
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
        required: true,
        minlength: 8
    },
    wallet: {
        balance: {
            type: Number,
            default: 0
        },
        isVerified: {
            type: Boolean,
            default: false
        },
        transactions: [{
            type: mongoose.Schema.Types.ObjectId,
            ref: 'Transaction'
        }]
    },
    fitnessData: {
        totalSteps: {
            type: Number,
            default: 0
        },
        totalCalories: {
            type: Number,
            default: 0
        },
        totalDistance: {
            type: Number,
            default: 0
        },
        achievements: [{
            type: mongoose.Schema.Types.ObjectId,
            ref: 'Achievement'
        }]
    },
    social: {
        friends: [{
            type: mongoose.Schema.Types.ObjectId,
            ref: 'User'
        }],
        groups: [{
            type: mongoose.Schema.Types.ObjectId,
            ref: 'Group'
        }],
        posts: [{
            type: mongoose.Schema.Types.ObjectId,
            ref: 'Post'
        }]
    },
    preferences: {
        language: {
            type: String,
            default: 'de'
        },
        measurementSystem: {
            type: String,
            enum: ['metric', 'imperial'],
            default: 'metric'
        },
        notifications: {
            email: {
                type: Boolean,
                default: true
            },
            push: {
                type: Boolean,
                default: true
            }
        }
    },
    security: {
        twoFactorEnabled: {
            type: Boolean,
            default: false
        },
        twoFactorSecret: String,
        lastLogin: Date,
        loginAttempts: {
            type: Number,
            default: 0
        },
        lockUntil: Date
    },
    createdAt: {
        type: Date,
        default: Date.now
    },
    updatedAt: {
        type: Date,
        default: Date.now
    }
}, {
    timestamps: true
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

// Passwort-Vergleichsmethode
userSchema.methods.comparePassword = async function(candidatePassword) {
    return bcrypt.compare(candidatePassword, this.password);
};

// Benutzer-Sperrung
userSchema.methods.incrementLoginAttempts = async function() {
    if (this.security.lockUntil && this.security.lockUntil > Date.now()) {
        return;
    }
    
    this.security.loginAttempts += 1;
    
    if (this.security.loginAttempts >= 5) {
        this.security.lockUntil = Date.now() + 2 * 60 * 60 * 1000; // 2 Stunden
    }
    
    await this.save();
};

// Benutzer-Entsperrung
userSchema.methods.resetLoginAttempts = async function() {
    this.security.loginAttempts = 0;
    this.security.lockUntil = undefined;
    await this.save();
};

const User = mongoose.model('User', userSchema);

module.exports = User; 