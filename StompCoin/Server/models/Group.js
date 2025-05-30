const mongoose = require('mongoose');

const groupSchema = new mongoose.Schema({
    name: {
        type: String,
        required: true,
        trim: true,
        minlength: 3,
        maxlength: 50
    },
    description: {
        type: String,
        trim: true,
        maxlength: 500
    },
    creator: {
        type: mongoose.Schema.Types.ObjectId,
        ref: 'User',
        required: true
    },
    members: [{
        user: {
            type: mongoose.Schema.Types.ObjectId,
            ref: 'User'
        },
        role: {
            type: String,
            enum: ['admin', 'moderator', 'member'],
            default: 'member'
        },
        joinedAt: {
            type: Date,
            default: Date.now
        }
    }],
    type: {
        type: String,
        enum: ['public', 'private', 'invite-only'],
        default: 'public'
    },
    settings: {
        allowInvites: {
            type: Boolean,
            default: true
        },
        requireApproval: {
            type: Boolean,
            default: false
        },
        allowPosts: {
            type: Boolean,
            default: true
        },
        allowComments: {
            type: Boolean,
            default: true
        }
    },
    challenges: [{
        type: mongoose.Schema.Types.ObjectId,
        ref: 'Challenge'
    }],
    events: [{
        type: mongoose.Schema.Types.ObjectId,
        ref: 'Event'
    }],
    posts: [{
        type: mongoose.Schema.Types.ObjectId,
        ref: 'Post'
    }],
    stats: {
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
        memberCount: {
            type: Number,
            default: 1
        }
    },
    tags: [{
        type: String,
        trim: true
    }],
    location: {
        type: {
            type: String,
            enum: ['Point'],
            default: 'Point'
        },
        coordinates: {
            type: [Number],
            default: [0, 0]
        }
    },
    isActive: {
        type: Boolean,
        default: true
    }
}, {
    timestamps: true
});

// Indizes für schnelle Abfragen
groupSchema.index({ name: 'text', description: 'text' });
groupSchema.index({ 'members.user': 1 });
groupSchema.index({ type: 1, isActive: 1 });
groupSchema.index({ location: '2dsphere' });

// Methode zum Hinzufügen eines Mitglieds
groupSchema.methods.addMember = async function(userId, role = 'member') {
    if (this.members.some(member => member.user.toString() === userId.toString())) {
        throw new Error('Benutzer ist bereits Mitglied der Gruppe');
    }

    this.members.push({
        user: userId,
        role: role,
        joinedAt: new Date()
    });

    this.stats.memberCount += 1;
    await this.save();
};

// Methode zum Entfernen eines Mitglieds
groupSchema.methods.removeMember = async function(userId) {
    const memberIndex = this.members.findIndex(
        member => member.user.toString() === userId.toString()
    );

    if (memberIndex === -1) {
        throw new Error('Benutzer ist kein Mitglied der Gruppe');
    }

    this.members.splice(memberIndex, 1);
    this.stats.memberCount -= 1;
    await this.save();
};

// Methode zum Aktualisieren der Gruppenstatistiken
groupSchema.methods.updateStats = async function() {
    const stats = await this.model('FitnessData').aggregate([
        {
            $match: {
                userId: {
                    $in: this.members.map(member => member.user)
                }
            }
        },
        {
            $group: {
                _id: null,
                totalSteps: { $sum: '$steps' },
                totalCalories: { $sum: '$calories' },
                totalDistance: { $sum: '$distance' }
            }
        }
    ]);

    if (stats.length > 0) {
        this.stats.totalSteps = stats[0].totalSteps;
        this.stats.totalCalories = stats[0].totalCalories;
        this.stats.totalDistance = stats[0].totalDistance;
        await this.save();
    }
};

// Statische Methode zum Suchen von Gruppen
groupSchema.statics.searchGroups = async function(query, options = {}) {
    const {
        type,
        isActive = true,
        location,
        radius,
        limit = 20,
        skip = 0
    } = options;

    const searchQuery = {
        $text: { $search: query },
        isActive
    };

    if (type) {
        searchQuery.type = type;
    }

    if (location && radius) {
        searchQuery.location = {
            $near: {
                $geometry: {
                    type: 'Point',
                    coordinates: location
                },
                $maxDistance: radius
            }
        };
    }

    return this.find(searchQuery)
        .sort({ score: { $meta: 'textScore' } })
        .skip(skip)
        .limit(limit)
        .populate('creator', 'username')
        .populate('members.user', 'username');
};

const Group = mongoose.model('Group', groupSchema);

module.exports = Group; 