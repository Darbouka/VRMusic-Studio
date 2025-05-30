const mongoose = require('mongoose');

const transactionSchema = new mongoose.Schema({
    type: {
        type: String,
        enum: ['transfer', 'mining', 'reward', 'purchase'],
        required: true
    },
    amount: {
        type: Number,
        required: true,
        min: 0
    },
    fromUser: {
        type: mongoose.Schema.Types.ObjectId,
        ref: 'User',
        required: function() {
            return this.type === 'transfer';
        }
    },
    toUser: {
        type: mongoose.Schema.Types.ObjectId,
        ref: 'User',
        required: function() {
            return this.type === 'transfer' || this.type === 'reward';
        }
    },
    status: {
        type: String,
        enum: ['pending', 'completed', 'failed', 'cancelled'],
        default: 'pending'
    },
    metadata: {
        fitnessData: {
            steps: Number,
            calories: Number,
            distance: Number,
            timestamp: Date
        },
        miningData: {
            difficulty: Number,
            hashRate: Number,
            duration: Number
        },
        purchaseData: {
            itemId: String,
            itemName: String,
            quantity: Number
        }
    },
    fee: {
        type: Number,
        default: 0
    },
    timestamp: {
        type: Date,
        default: Date.now
    },
    blockNumber: {
        type: Number,
        required: function() {
            return this.type === 'mining';
        }
    },
    hash: {
        type: String,
        required: true,
        unique: true
    },
    signature: {
        type: String,
        required: true
    }
}, {
    timestamps: true
});

// Indizes für schnelle Abfragen
transactionSchema.index({ fromUser: 1, timestamp: -1 });
transactionSchema.index({ toUser: 1, timestamp: -1 });
transactionSchema.index({ type: 1, timestamp: -1 });
transactionSchema.index({ status: 1, timestamp: -1 });
transactionSchema.index({ hash: 1 }, { unique: true });

// Validierung vor dem Speichern
transactionSchema.pre('save', function(next) {
    if (this.type === 'transfer' && this.fromUser === this.toUser) {
        next(new Error('Sender und Empfänger können nicht identisch sein'));
    }
    next();
});

// Methode zum Überprüfen der Transaktionsvalidität
transactionSchema.methods.isValid = function() {
    return this.amount > 0 && 
           this.status === 'pending' && 
           this.timestamp > Date.now() - 24 * 60 * 60 * 1000; // 24 Stunden
};

// Methode zum Abrufen der Transaktionshistorie eines Benutzers
transactionSchema.statics.getUserHistory = async function(userId, limit = 50, skip = 0) {
    return this.find({
        $or: [
            { fromUser: userId },
            { toUser: userId }
        ]
    })
    .sort({ timestamp: -1 })
    .skip(skip)
    .limit(limit)
    .populate('fromUser', 'username')
    .populate('toUser', 'username');
};

const Transaction = mongoose.model('Transaction', transactionSchema);

module.exports = Transaction; 