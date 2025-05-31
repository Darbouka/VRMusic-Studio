import Foundation

struct User: Codable, Identifiable, Equatable {
    let id: String
    let email: String
    let username: String
    var walletBalance: Double
}

struct MusicGenre: Identifiable, Equatable {
    let id: String
    let name: String
    let description: String
    let icon: String
    var floors: [Floor]
}

struct Floor: Identifiable, Equatable {
    let id: String
    let name: String
    let difficulty: Int
    let requiredLevel: Int
    let reward: Double
    let musicGenre: String
    let description: String
    let image: String
}

struct LeaderboardEntry: Identifiable, Equatable {
    let id: String
    let userId: String
    let username: String
    let score: Int
    let rank: Int
    let timestamp: Date
    let floorId: String
}

struct UserSettings: Codable, Equatable {
    var musicVolume: Double = 0.8
    var soundEffectsVolume: Double = 0.8
    var notificationsEnabled: Bool = true
    var darkMode: Bool = false
    var language: String = "Deutsch"
    var autoSync: Bool = true
    var showLeaderboard: Bool = true
    var showTutorial: Bool = true
} 