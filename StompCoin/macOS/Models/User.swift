import Foundation

struct User: Codable, Identifiable {
    let id: String
    var username: String
    var email: String
    var profileImage: String
    var level: Int
    var experience: Int
    var walletBalance: Double
    var settings: UserSettings
    var createdAt: Date
    var lastLogin: Date
    var fitnessData: FitnessData?
    var activities: [Activity] = []
    
    init(id: String,
         username: String,
         email: String,
         profileImage: String = "profile_default",
         level: Int = 1,
         experience: Int = 0,
         walletBalance: Double = 100.0,
         settings: UserSettings = UserSettings(),
         createdAt: Date = Date(),
         lastLogin: Date = Date(),
         fitnessData: FitnessData? = nil) {
        self.id = id
        self.username = username
        self.email = email
        self.profileImage = profileImage
        self.level = level
        self.experience = experience
        self.walletBalance = walletBalance
        self.settings = settings
        self.createdAt = createdAt
        self.lastLogin = lastLogin
        self.fitnessData = fitnessData
    }
}

struct UserSettings: Codable {
    var musicVolume: Double = 0.8
    var soundEffectsVolume: Double = 0.8
    var pushNotifications: Bool = true
    var emailNotifications: Bool = true
    var darkMode: Bool = false
    var language: String = "de"
    var autoSync: Bool = true
    var showLeaderboard: Bool = true
    var showTutorial: Bool = true
} 