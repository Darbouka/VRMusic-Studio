import Foundation

class ChallengeService: ObservableObject {
    static let shared = ChallengeService()
    
    struct Challenge: Identifiable, Codable {
        let id: UUID
        let title: String
        let description: String
        let eventId: UUID
        let startDate: Date
        let endDate: Date
        let requiredTokens: Int
        let requiredStompCoins: Int
        let participants: [UUID]
        let confirmedParticipants: [UUID]
        let status: ChallengeStatus
        let type: ChallengeType
    }
    
    enum ChallengeStatus: String, Codable {
        case upcoming = "Bevorstehend"
        case active = "Aktiv"
        case completed = "Abgeschlossen"
        case cancelled = "Abgebrochen"
    }
    
    enum ChallengeType: String, Codable {
        case event = "Event Challenge"
        case video = "Video Challenge"
        case social = "Social Media Challenge"
    }
    
    @Published private(set) var challenges: [Challenge] = []
    @Published private(set) var userChallenges: [UUID: Challenge] = [:]
    
    private let userDefaults = UserDefaults.standard
    private let challengesKey = "challenges"
    private let userChallengesKey = "userChallenges"
    
    init() {
        loadChallenges()
        startChallengeUpdates()
    }
    
    private func loadChallenges() {
        if let data = userDefaults.data(forKey: challengesKey),
           let loadedChallenges = try? JSONDecoder().decode([Challenge].self, from: data) {
            challenges = loadedChallenges
        }
        
        if let data = userDefaults.data(forKey: userChallengesKey),
           let loadedUserChallenges = try? JSONDecoder().decode([UUID: Challenge].self, from: data) {
            userChallenges = loadedUserChallenges
        }
    }
    
    private func saveChallenges() {
        if let data = try? JSONEncoder().encode(challenges) {
            userDefaults.set(data, forKey: challengesKey)
        }
        
        if let data = try? JSONEncoder().encode(userChallenges) {
            userDefaults.set(data, forKey: userChallengesKey)
        }
    }
    
    private func startChallengeUpdates() {
        // Timer für Echtzeit-Updates
        Timer.scheduledTimer(withTimeInterval: 60, repeats: true) { [weak self] _ in
            self?.updateChallenges()
        }
    }
    
    func updateChallenges() {
        Task {
            do {
                try await fetchNewChallenges()
                try await updateChallengeStatuses()
                await MainActor.run {
                    saveChallenges()
                }
            } catch {
                print("Fehler beim Aktualisieren der Challenges: \(error)")
            }
        }
    }
    
    private func fetchNewChallenges() async throws {
        // Hier die tatsächliche API-Integration implementieren
        // Beispiel:
        let newChallenges = [
            Challenge(
                id: UUID(),
                title: "Sommer Goa Festival Challenge",
                description: "Tanz die Nacht durch beim Sommer Goa Festival",
                eventId: UUID(),
                startDate: Date(),
                endDate: Calendar.current.date(byAdding: .day, value: 7, to: Date())!,
                requiredTokens: 100,
                requiredStompCoins: 200,
                participants: [],
                confirmedParticipants: [],
                status: .upcoming,
                type: .event
            )
        ]
        
        await MainActor.run {
            challenges.append(contentsOf: newChallenges)
        }
    }
    
    private func updateChallengeStatuses() async throws {
        let now = Date()
        
        await MainActor.run {
            for (index, challenge) in challenges.enumerated() {
                var updatedChallenge = challenge
                
                if challenge.status == .upcoming && now >= challenge.startDate {
                    updatedChallenge.status = .active
                } else if challenge.status == .active && now >= challenge.endDate {
                    updatedChallenge.status = .completed
                }
                
                challenges[index] = updatedChallenge
            }
        }
    }
    
    func joinChallenge(_ challenge: Challenge) async throws {
        guard let userId = UserManager.shared.currentUser?.id else {
            throw NSError(domain: "ChallengeService", code: 1, userInfo: [NSLocalizedDescriptionKey: "Benutzer nicht angemeldet"])
        }
        
        // Prüfen ob Benutzer VIP Premium ist
        let isVIPPremium = UserManager.shared.currentUser?.isVIPPremium ?? false
        let isDeveloper = UserManager.shared.currentUser?.isDeveloper ?? false
        
        // Wenn nicht VIP Premium oder Entwickler, dann Gebühren prüfen
        if !isVIPPremium && !isDeveloper {
            // Prüfen ob genügend Token und StompCoins vorhanden sind
            guard let userTokens = UserManager.shared.currentUser?.tokens,
                  userTokens >= challenge.requiredTokens else {
                throw NSError(domain: "ChallengeService", code: 2, userInfo: [NSLocalizedDescriptionKey: "Nicht genügend Token"])
            }
            
            guard let userStompCoins = UserManager.shared.currentUser?.stompCoins,
                  userStompCoins >= challenge.requiredStompCoins else {
                throw NSError(domain: "ChallengeService", code: 3, userInfo: [NSLocalizedDescriptionKey: "Nicht genügend StompCoins"])
            }
            
            // Token und StompCoins abziehen
            try await UserManager.shared.deductTokens(challenge.requiredTokens)
            try await UserManager.shared.deductStompCoins(challenge.requiredStompCoins)
            
            // Entwickler-Auszahlung
            try await PayoutService.shared.processDeveloperPayout(amount: Decimal(challenge.requiredStompCoins))
        }
        
        // Challenge beitreten
        await MainActor.run {
            if let index = challenges.firstIndex(where: { $0.id == challenge.id }) {
                var updatedChallenge = challenge
                updatedChallenge.participants.append(userId)
                challenges[index] = updatedChallenge
                userChallenges[challenge.id] = updatedChallenge
                saveChallenges()
            }
        }
    }
    
    func confirmParticipation(_ challenge: Challenge) async throws {
        guard let userId = UserManager.shared.currentUser?.id else {
            throw NSError(domain: "ChallengeService", code: 1, userInfo: [NSLocalizedDescriptionKey: "Benutzer nicht angemeldet"])
        }
        
        guard challenge.participants.contains(userId) else {
            throw NSError(domain: "ChallengeService", code: 4, userInfo: [NSLocalizedDescriptionKey: "Nicht an Challenge teilgenommen"])
        }
        
        await MainActor.run {
            if let index = challenges.firstIndex(where: { $0.id == challenge.id }) {
                var updatedChallenge = challenge
                updatedChallenge.confirmedParticipants.append(userId)
                challenges[index] = updatedChallenge
                userChallenges[challenge.id] = updatedChallenge
                saveChallenges()
            }
        }
    }
    
    func removeInactiveParticipants() async throws {
        let now = Date()
        
        await MainActor.run {
            for (index, challenge) in challenges.enumerated() {
                if challenge.status == .active {
                    var updatedChallenge = challenge
                    updatedChallenge.participants = challenge.participants.filter { userId in
                        challenge.confirmedParticipants.contains(userId)
                    }
                    challenges[index] = updatedChallenge
                }
            }
            saveChallenges()
        }
    }
    
    func getChallenges(for type: ChallengeType? = nil) -> [Challenge] {
        if let type = type {
            return challenges.filter { $0.type == type }
        }
        return challenges
    }
    
    func getActiveChallenges() -> [Challenge] {
        return challenges.filter { $0.status == .active }
    }
    
    func getUpcomingChallenges() -> [Challenge] {
        return challenges.filter { $0.status == .upcoming }
    }
    
    func getUserChallenges() -> [Challenge] {
        return Array(userChallenges.values)
    }
} 