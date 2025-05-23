import Foundation
import SwiftData

class GenreManager {
    static let shared = GenreManager()
    
    private init() {}
    
    // Genre-Rangliste aktualisieren
    func updateGenreRankings(modelContext: ModelContext) {
        let descriptor = FetchDescriptor<GenreRanking>()
        guard let rankings = try? modelContext.fetch(descriptor) else { return }
        
        for ranking in rankings {
            updateRanking(ranking, modelContext: modelContext)
        }
    }
    
    // Einzelne Genre-Rangliste aktualisieren
    private func updateRanking(_ ranking: GenreRanking, modelContext: ModelContext) {
        // Benutzer-Ranglisten für dieses Genre abrufen
        let descriptor = FetchDescriptor<UserRanking>(
            predicate: #Predicate<UserRanking> { $0.genre == ranking.genre }
        )
        guard let userRankings = try? modelContext.fetch(descriptor) else { return }
        
        // Internationale Rangliste
        let internationalRankings = userRankings.sorted { $0.stompCoins > $1.stompCoins }
        ranking.internationalRankings = internationalRankings
        
        // Nationale Rangliste (Beispiel: Deutschland)
        let nationalRankings = userRankings
            .filter { $0.user.country == "DE" }
            .sorted { $0.stompCoins > $1.stompCoins }
        ranking.nationalRankings = nationalRankings
        
        // Lokale Rangliste (Beispiel: Berlin)
        let localRankings = userRankings
            .filter { $0.user.city == "Berlin" }
            .sorted { $0.stompCoins > $1.stompCoins }
        ranking.localRankings = localRankings
        
        // Statistiken aktualisieren
        ranking.totalParticipants = userRankings.count
        ranking.totalStompCoins = userRankings.reduce(0) { $0 + $1.stompCoins }
        ranking.lastUpdated = Date()
        
        // Ränge aktualisieren
        for (index, ranking) in internationalRankings.enumerated() {
            ranking.rank = index + 1
        }
        
        try? modelContext.save()
    }
    
    // Benutzer-Rangliste aktualisieren
    func updateUserRanking(user: User, genre: MusicGenre, modelContext: ModelContext) {
        // Bestehende Rangliste suchen oder erstellen
        let descriptor = FetchDescriptor<UserRanking>(
            predicate: #Predicate<UserRanking> { $0.user.id == user.id && $0.genre == genre }
        )
        
        let userRanking: UserRanking
        if let existing = try? modelContext.fetch(descriptor).first {
            userRanking = existing
        } else {
            userRanking = UserRanking(user: user, genre: genre)
            modelContext.insert(userRanking)
            user.rankings.append(userRanking)
        }
        
        // Statistiken aktualisieren
        userRanking.stompCoins = calculateStompCoins(for: user, genre: genre)
        userRanking.challengesWon = calculateChallengesWon(for: user, genre: genre)
        userRanking.lastUpdated = Date()
        
        try? modelContext.save()
    }
    
    // StompCoins für ein Genre berechnen
    private func calculateStompCoins(for user: User, genre: MusicGenre) -> Double {
        // Coins aus Event-Teilnahmen
        let eventCoins = user.eventHistory
            .filter { $0.event.musicGenres.contains(genre) }
            .reduce(0) { $0 + $1.coinsEarned }
        
        // Coins aus Challenge-Gewinnen
        let challengeCoins = user.eventHistory
            .flatMap { $0.event.challenges }
            .filter { $0.winner?.id == user.id && $0.event.musicGenres.contains(genre) }
            .reduce(0) { $0 + $1.reward }
        
        return eventCoins + challengeCoins
    }
    
    // Gewonnene Challenges für ein Genre zählen
    private func calculateChallengesWon(for user: User, genre: MusicGenre) -> Int {
        return user.eventHistory
            .flatMap { $0.event.challenges }
            .filter { $0.winner?.id == user.id && $0.event.musicGenres.contains(genre) }
            .count
    }
    
    // Genre-Rangliste abrufen
    func getGenreRanking(for genre: MusicGenre, modelContext: ModelContext) -> GenreRanking? {
        let descriptor = FetchDescriptor<GenreRanking>(
            predicate: #Predicate<GenreRanking> { $0.genre == genre }
        )
        return try? modelContext.fetch(descriptor).first
    }
    
    // Benutzer-Rangliste abrufen
    func getUserRanking(for user: User, genre: MusicGenre, modelContext: ModelContext) -> UserRanking? {
        let descriptor = FetchDescriptor<UserRanking>(
            predicate: #Predicate<UserRanking> { $0.user.id == user.id && $0.genre == genre }
        )
        return try? modelContext.fetch(descriptor).first
    }
} 