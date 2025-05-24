import Foundation
import SwiftData

@Model
final class Event {
    var id: UUID
    var name: String
    var description: String
    var startDate: Date
    var endDate: Date
    var location: String
    var organizer: String
    var musicGenres: [MusicGenre]
    var boostMultiplier: Double
    var isActive: Bool
    var participants: [EventParticipation]
    var challenges: [Challenge]
    var vipTicketsAvailable: Int
    var vipTicketPrice: Double
    var organizerSharePercentage: Double
    
    init(name: String, description: String, startDate: Date, endDate: Date, location: String, organizer: String) {
        self.id = UUID()
        self.name = name
        self.description = description
        self.startDate = startDate
        self.endDate = endDate
        self.location = location
        self.organizer = organizer
        self.musicGenres = []
        self.boostMultiplier = 1.0
        self.isActive = false
        self.participants = []
        self.challenges = []
        self.vipTicketsAvailable = 0
        self.vipTicketPrice = 0.0
        self.organizerSharePercentage = 0.0
    }
}

@Model
final class EventParticipation {
    var id: UUID
    var event: Event
    var user: User
    var stepsGenerated: Int
    var coinsEarned: Double
    var boostActive: Bool
    var participationDate: Date
    
    init(event: Event, user: User) {
        self.id = UUID()
        self.event = event
        self.user = user
        self.stepsGenerated = 0
        self.coinsEarned = 0.0
        self.boostActive = false
        self.participationDate = Date()
    }
}

@Model
final class Challenge {
    var id: UUID
    var event: Event
    var name: String
    var description: String
    var reward: Double
    var startDate: Date
    var endDate: Date
    var participants: [User]
    var winner: User?
    var isLive: Bool
    var isSocialMedia: Bool
    
    init(event: Event, name: String, description: String, reward: Double, startDate: Date, endDate: Date) {
        self.id = UUID()
        self.event = event
        self.name = name
        self.description = description
        self.reward = reward
        self.startDate = startDate
        self.endDate = endDate
        self.participants = []
        self.isLive = false
        self.isSocialMedia = false
    }
} 