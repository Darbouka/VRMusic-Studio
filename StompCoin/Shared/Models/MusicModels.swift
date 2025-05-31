import Foundation

struct MusicGenre: Identifiable, Codable {
    let id: String
    let name: String
    let description: String
    let icon: String
    let floors: [Floor]
}

struct Floor: Identifiable, Codable {
    let id: String
    let name: String
    let image: String
    let difficulty: Int
    let requiredLevel: Int
    let reward: Int
    let musicGenre: String
    let description: String
}

struct LeaderboardEntry: Identifiable, Codable {
    let id: String
    let userId: String
    let username: String
    let score: Int
    let rank: Int
    let profileImage: String
}

struct Activity: Identifiable, Codable {
    let id: String
    let type: String
    let description: String
    let timestamp: Date
    let reward: Int?
}

struct UserSettings: Codable {
    var musicVolume: Double
    var soundEffectsVolume: Double
    var notificationsEnabled: Bool
    var darkMode: Bool
    var language: String
    var autoSync: Bool
    var showLeaderboard: Bool
    var showTutorial: Bool
    var emailNotifications: Bool
} 