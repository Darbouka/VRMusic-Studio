import Foundation
import SwiftUI
import Combine

// MusicModels importieren, falls nötig (bei SwiftPM meist nicht nötig, aber für Klarheit)
// import MusicModels

class AppState: ObservableObject {
    @Published var isAuthenticated = false
    @Published var currentUser: User?
    @Published var settings = UserSettings()
    @Published var musicGenres: [MusicGenre] = []
    @Published var leaderboardEntries: [LeaderboardEntry] = []
    
    init() {
        loadDemoData()
    }
    
    func saveSettings() {
        // Hier später: Einstellungen in UserDefaults oder einer Datenbank speichern
        print("Einstellungen gespeichert")
    }
    
    private func loadDemoData() {
        // Demo-Musikgenres
        musicGenres = [
            MusicGenre(
                id: "1",
                name: "Hip Hop",
                description: "Klassische Hip-Hop-Beats für deine Stomps",
                icon: "music.note",
                floors: [
                    Floor(
                        id: "1",
                        name: "Hip Hop Basics",
                        difficulty: 1,
                        requiredLevel: 1,
                        reward: 50.0,
                        musicGenre: "Hip Hop",
                        description: "Perfekt für Anfänger",
                        image: "hiphop_basics"
                    )
                ]
            )
        ]
        
        // Demo-Ranglisten
        leaderboardEntries = [
            LeaderboardEntry(
                id: "1",
                userId: "demo1",
                username: "DanceKing",
                score: 1000,
                rank: 1,
                timestamp: Date(),
                floorId: "1"
            ),
            LeaderboardEntry(
                id: "2",
                userId: "demo2",
                username: "RhythmQueen",
                score: 850,
                rank: 2,
                timestamp: Date(),
                floorId: "1"
            ),
            LeaderboardEntry(
                id: "3",
                userId: "demo3",
                username: "BeatMaster",
                score: 700,
                rank: 3,
                timestamp: Date(),
                floorId: "1"
            )
        ]
    }
} 