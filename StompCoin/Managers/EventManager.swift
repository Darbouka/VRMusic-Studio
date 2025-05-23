import Foundation
import SwiftData

class EventManager {
    static let shared = EventManager()
    
    private init() {}
    
    // Event erstellen
    func createEvent(
        name: String,
        description: String,
        startDate: Date,
        endDate: Date,
        location: String,
        organizer: String,
        musicGenres: [MusicGenre],
        boostMultiplier: Double,
        vipTicketsAvailable: Int,
        vipTicketPrice: Double,
        organizerSharePercentage: Double,
        modelContext: ModelContext
    ) -> Event {
        let event = Event(
            name: name,
            description: description,
            startDate: startDate,
            endDate: endDate,
            location: location,
            organizer: organizer
        )
        
        event.musicGenres = musicGenres
        event.boostMultiplier = boostMultiplier
        event.vipTicketsAvailable = vipTicketsAvailable
        event.vipTicketPrice = vipTicketPrice
        event.organizerSharePercentage = organizerSharePercentage
        
        modelContext.insert(event)
        try? modelContext.save()
        
        return event
    }
    
    // Event-Teilnahme
    func participateInEvent(event: Event, user: User, modelContext: ModelContext) -> Bool {
        // Überprüfe, ob der Benutzer bereits teilnimmt
        guard !user.eventHistory.contains(where: { $0.event.id == event.id }) else {
            return false
        }
        
        let participation = EventParticipation(event: event, user: user)
        user.eventHistory.append(participation)
        event.participants.append(participation)
        
        try? modelContext.save()
        return true
    }
    
    // VIP-Ticket kaufen
    func purchaseVipTicket(event: Event, user: User, modelContext: ModelContext) -> Bool {
        guard event.vipTicketsAvailable > 0 else { return false }
        guard user.stompCoins >= event.vipTicketPrice else { return false }
        
        // Coins abziehen
        user.stompCoins -= event.vipTicketPrice
        
        // VIP-Ticket verfügbar machen
        event.vipTicketsAvailable -= 1
        
        // Transaktion erstellen
        let transaction = Transaction(
            user: user,
            type: .purchase,
            amount: event.vipTicketPrice,
            currency: .stompCoin,
            description: "VIP-Ticket für \(event.name)"
        )
        modelContext.insert(transaction)
        
        try? modelContext.save()
        return true
    }
    
    // Event-Status aktualisieren
    func updateEventStatus(event: Event) {
        let now = Date()
        event.isActive = now >= event.startDate && now <= event.endDate
    }
    
    // Challenge erstellen
    func createChallenge(
        event: Event,
        name: String,
        description: String,
        reward: Double,
        startDate: Date,
        endDate: Date,
        isLive: Bool,
        isSocialMedia: Bool,
        modelContext: ModelContext
    ) -> Challenge {
        let challenge = Challenge(
            event: event,
            name: name,
            description: description,
            reward: reward,
            startDate: startDate,
            endDate: endDate
        )
        
        challenge.isLive = isLive
        challenge.isSocialMedia = isSocialMedia
        
        event.challenges.append(challenge)
        modelContext.insert(challenge)
        try? modelContext.save()
        
        return challenge
    }
    
    // Challenge-Teilnahme
    func participateInChallenge(challenge: Challenge, user: User, modelContext: ModelContext) -> Bool {
        guard challenge.isLive else { return false }
        guard !challenge.participants.contains(where: { $0.id == user.id }) else { return false }
        
        challenge.participants.append(user)
        try? modelContext.save()
        return true
    }
    
    // Challenge-Gewinner setzen
    func setChallengeWinner(challenge: Challenge, winner: User, modelContext: ModelContext) -> Bool {
        guard challenge.isLive else { return false }
        guard challenge.participants.contains(where: { $0.id == winner.id }) else { return false }
        
        challenge.winner = winner
        challenge.isLive = false
        
        // Belohnung auszahlen
        winner.stompCoins += challenge.reward
        
        // Transaktion erstellen
        let transaction = Transaction(
            user: winner,
            type: .reward,
            amount: challenge.reward,
            currency: .stompCoin,
            description: "Gewinn der Challenge: \(challenge.name)"
        )
        modelContext.insert(transaction)
        
        try? modelContext.save()
        return true
    }
} 