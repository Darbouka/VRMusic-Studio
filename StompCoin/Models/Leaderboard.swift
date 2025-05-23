import Foundation
import SwiftData

@Model
class Leaderboard {
    var genre: MusicGenre?
    var timeFrame: TimeFrame
    var entries: [LeaderboardEntry]
    var lastUpdated: Date
    
    init(genre: MusicGenre? = nil, timeFrame: TimeFrame) {
        self.genre = genre
        self.timeFrame = timeFrame
        self.entries = []
        self.lastUpdated = Date()
    }
}

@Model
class LeaderboardEntry {
    var user: User
    var steps: Int
    var coins: Int
    var rank: Int
    var lastUpdated: Date
    
    init(user: User, steps: Int, coins: Int) {
        self.user = user
        self.steps = steps
        self.coins = coins
        self.rank = 0
        self.lastUpdated = Date()
    }
}

enum TimeFrame: String, Codable {
    case daily = "Täglich"
    case weekly = "Wöchentlich"
    case monthly = "Monatlich"
    case allTime = "Gesamt"
} 