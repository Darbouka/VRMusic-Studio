import Foundation
import SwiftData

class GamificationManager {
    static let shared = GamificationManager()
    
    private let xpPerStep = 1
    private let xpPerCalorie = 2
    private let xpPerAchievement = 100
    
    private init() {}
    
    func calculateXP(from stats: FitnessStats) -> Int {
        let stepsXP = stats.steps * xpPerStep
        let caloriesXP = Int(stats.caloriesBurned) * xpPerCalorie
        return stepsXP + caloriesXP
    }
    
    func calculateLevel(xp: Int) -> Int {
        // Level = √(XP/100)
        return Int(sqrt(Double(xp) / 100.0)) + 1
    }
    
    func xpForNextLevel(currentLevel: Int) -> Int {
        return (currentLevel * currentLevel) * 100
    }
    
    func checkAchievements(for user: User, stats: FitnessStats) async -> [Achievement] {
        var newAchievements: [Achievement] = []
        
        // Schritte Achievements
        if stats.steps >= 10000 && !user.achievements.contains(where: { $0.id == "steps_10k" }) {
            newAchievements.append(Achievement(
                id: "steps_10k",
                title: "10.000 Schritte",
                description: "10.000 Schritte an einem Tag erreicht",
                xpReward: xpPerAchievement
            ))
        }
        
        // Kalorien Achievements
        if stats.caloriesBurned >= 500 && !user.achievements.contains(where: { $0.id == "calories_500" }) {
            newAchievements.append(Achievement(
                id: "calories_500",
                title: "Kalorienverbrenner",
                description: "500 Kalorien an einem Tag verbrannt",
                xpReward: xpPerAchievement
            ))
        }
        
        // Streak Achievements
        if user.currentStreak >= 7 && !user.achievements.contains(where: { $0.id == "streak_7" }) {
            newAchievements.append(Achievement(
                id: "streak_7",
                title: "Wöchentlicher Streak",
                description: "7 Tage in Folge aktiv",
                xpReward: xpPerAchievement * 2
            ))
        }
        
        return newAchievements
    }
    
    func updateLeaderboard(for user: User, stats: FitnessStats) async {
        let xp = calculateXP(from: stats)
        let level = calculateLevel(xp: xp)
        
        // Hier würde die Aktualisierung der Leaderboard-Datenbank erfolgen
    }
    
    func getLeaderboard(type: LeaderboardType) async -> [LeaderboardEntry] {
        // Hier würde die Abfrage der Leaderboard-Datenbank erfolgen
        return []
    }
}

struct Achievement: Codable, Identifiable {
    let id: String
    let title: String
    let description: String
    let xpReward: Int
    var dateEarned: Date?
}

struct LeaderboardEntry: Codable {
    let userId: UUID
    let username: String
    let xp: Int
    let level: Int
    let rank: Int
}

enum LeaderboardType {
    case daily
    case weekly
    case monthly
    case allTime
} 