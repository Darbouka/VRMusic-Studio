const express = require('express');
const router = express.Router();
const User = require('../models/User');
const auth = require('../middleware/auth');

// Leaderboard abrufen
router.get('/', auth, async (req, res) => {
    try {
        const { timeFrame, category } = req.query;
        
        let leaderboard;
        
        switch (category) {
            case 'steps':
                leaderboard = await getStepsLeaderboard(timeFrame);
                break;
            case 'mining':
                leaderboard = await getMiningLeaderboard(timeFrame);
                break;
            case 'events':
                leaderboard = await getEventsLeaderboard(timeFrame);
                break;
            default:
                leaderboard = await getStepsLeaderboard(timeFrame);
        }

        res.json(leaderboard);
    } catch (error) {
        console.error(error);
        res.status(500).json({ error: 'Server-Fehler beim Abrufen des Leaderboards' });
    }
});

// Benutzer-Rang abrufen
router.get('/rank', auth, async (req, res) => {
    try {
        const { timeFrame, category } = req.query;
        const userId = req.user.userId;

        let rank;
        
        switch (category) {
            case 'steps':
                rank = await getUserStepsRank(userId, timeFrame);
                break;
            case 'mining':
                rank = await getUserMiningRank(userId, timeFrame);
                break;
            case 'events':
                rank = await getUserEventsRank(userId, timeFrame);
                break;
            default:
                rank = await getUserStepsRank(userId, timeFrame);
        }

        res.json(rank);
    } catch (error) {
        console.error(error);
        res.status(500).json({ error: 'Server-Fehler beim Abrufen des Rangs' });
    }
});

// Hilfsfunktionen
async function getStepsLeaderboard(timeFrame) {
    const users = await User.findByRank(timeFrame);
    return users.map((user, index) => ({
        rank: index + 1,
        userId: user._id,
        username: user.username,
        score: user.totalSteps
    }));
}

async function getMiningLeaderboard(timeFrame) {
    const now = new Date();
    let startDate;

    switch (timeFrame) {
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

    const users = await User.aggregate([
        {
            $match: {
                'miningStats.poolMining.lastMined': { $gte: startDate },
                'miningStats.soloMining.lastMined': { $gte: startDate }
            }
        },
        {
            $project: {
                username: 1,
                totalMined: {
                    $add: [
                        '$miningStats.poolMining.totalMined',
                        '$miningStats.soloMining.totalMined'
                    ]
                }
            }
        },
        {
            $sort: { totalMined: -1 }
        }
    ]);

    return users.map((user, index) => ({
        rank: index + 1,
        userId: user._id,
        username: user.username,
        score: user.totalMined
    }));
}

async function getEventsLeaderboard(timeFrame) {
    const now = new Date();
    let startDate;

    switch (timeFrame) {
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

    const users = await User.aggregate([
        {
            $match: {
                'events.joinedAt': { $gte: startDate }
            }
        },
        {
            $project: {
                username: 1,
                eventCount: {
                    $size: {
                        $filter: {
                            input: '$events',
                            as: 'event',
                            cond: { $gte: ['$$event.joinedAt', startDate] }
                        }
                    }
                }
            }
        },
        {
            $sort: { eventCount: -1 }
        }
    ]);

    return users.map((user, index) => ({
        rank: index + 1,
        userId: user._id,
        username: user.username,
        score: user.eventCount
    }));
}

async function getUserStepsRank(userId, timeFrame) {
    const leaderboard = await getStepsLeaderboard(timeFrame);
    const userRank = leaderboard.findIndex(entry => entry.userId.toString() === userId.toString());
    return userRank !== -1 ? userRank + 1 : null;
}

async function getUserMiningRank(userId, timeFrame) {
    const leaderboard = await getMiningLeaderboard(timeFrame);
    const userRank = leaderboard.findIndex(entry => entry.userId.toString() === userId.toString());
    return userRank !== -1 ? userRank + 1 : null;
}

async function getUserEventsRank(userId, timeFrame) {
    const leaderboard = await getEventsLeaderboard(timeFrame);
    const userRank = leaderboard.findIndex(entry => entry.userId.toString() === userId.toString());
    return userRank !== -1 ? userRank + 1 : null;
}

module.exports = router; 