const mongoose = require('mongoose');

const fitnessDataSchema = new mongoose.Schema({
    userId: {
        type: mongoose.Schema.Types.ObjectId,
        ref: 'User',
        required: true
    },
    date: {
        type: Date,
        required: true
    },
    steps: {
        type: Number,
        default: 0,
        min: 0
    },
    calories: {
        type: Number,
        default: 0,
        min: 0
    },
    distance: {
        type: Number,
        default: 0,
        min: 0
    },
    activeMinutes: {
        type: Number,
        default: 0,
        min: 0
    },
    heartRate: {
        average: {
            type: Number,
            min: 0
        },
        max: {
            type: Number,
            min: 0
        },
        min: {
            type: Number,
            min: 0
        }
    },
    activities: [{
        type: {
            type: String,
            enum: ['walking', 'running', 'cycling', 'swimming', 'dancing', 'other']
        },
        duration: Number,
        calories: Number,
        distance: Number,
        startTime: Date,
        endTime: Date
    }],
    goals: {
        dailySteps: {
            type: Number,
            default: 10000
        },
        dailyCalories: {
            type: Number,
            default: 2000
        },
        dailyDistance: {
            type: Number,
            default: 5
        }
    },
    achievements: [{
        type: {
            type: String,
            enum: ['steps', 'calories', 'distance', 'streak']
        },
        value: Number,
        timestamp: Date
    }],
    rewards: [{
        type: mongoose.Schema.Types.ObjectId,
        ref: 'Transaction'
    }],
    isSynced: {
        type: Boolean,
        default: false
    },
    deviceInfo: {
        type: String,
        model: String,
        os: String,
        version: String
    }
}, {
    timestamps: true
});

// Indizes für schnelle Abfragen
fitnessDataSchema.index({ userId: 1, date: -1 });
fitnessDataSchema.index({ userId: 1, 'activities.startTime': -1 });
fitnessDataSchema.index({ userId: 1, isSynced: 1 });

// Methode zum Berechnen der Tagesstatistiken
fitnessDataSchema.methods.calculateDailyStats = function() {
    let totalCalories = 0;
    let totalDistance = 0;
    let totalActiveMinutes = 0;

    this.activities.forEach(activity => {
        totalCalories += activity.calories || 0;
        totalDistance += activity.distance || 0;
        totalActiveMinutes += activity.duration || 0;
    });

    this.calories = totalCalories;
    this.distance = totalDistance;
    this.activeMinutes = totalActiveMinutes;
};

// Methode zum Überprüfen der Zielerreichung
fitnessDataSchema.methods.checkGoals = function() {
    const achievements = [];

    if (this.steps >= this.goals.dailySteps) {
        achievements.push({
            type: 'steps',
            value: this.steps,
            timestamp: new Date()
        });
    }

    if (this.calories >= this.goals.dailyCalories) {
        achievements.push({
            type: 'calories',
            value: this.calories,
            timestamp: new Date()
        });
    }

    if (this.distance >= this.goals.dailyDistance) {
        achievements.push({
            type: 'distance',
            value: this.distance,
            timestamp: new Date()
        });
    }

    this.achievements.push(...achievements);
    return achievements;
};

// Statische Methode zum Abrufen der Benutzerstatistiken
fitnessDataSchema.statics.getUserStats = async function(userId, startDate, endDate) {
    return this.aggregate([
        {
            $match: {
                userId: mongoose.Types.ObjectId(userId),
                date: {
                    $gte: startDate,
                    $lte: endDate
                }
            }
        },
        {
            $group: {
                _id: null,
                totalSteps: { $sum: '$steps' },
                totalCalories: { $sum: '$calories' },
                totalDistance: { $sum: '$distance' },
                totalActiveMinutes: { $sum: '$activeMinutes' },
                averageHeartRate: { $avg: '$heartRate.average' },
                maxHeartRate: { $max: '$heartRate.max' },
                minHeartRate: { $min: '$heartRate.min' }
            }
        }
    ]);
};

const FitnessData = mongoose.model('FitnessData', fitnessDataSchema);

module.exports = FitnessData; 