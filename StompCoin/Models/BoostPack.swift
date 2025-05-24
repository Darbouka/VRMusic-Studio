import Foundation
import SwiftData

enum BoostType: String, Codable {
    case doubleBoost = "2x Boost"
    case tripleBoost = "3x Boost"
    case weekendBoost = "Weekend Boost"
    case eventBoost = "Event Boost"
    case challengeBoost = "Challenge Boost"
    case weekBoost = "Week Boost"
    case fitnessBoost = "Fitness Boost"
    case walkBoost = "Walk Boost"
    
    var multiplier: Double {
        switch self {
        case .doubleBoost: return 2.0
        case .tripleBoost: return 3.0
        case .weekendBoost: return 2.5
        case .eventBoost: return 3.0
        case .challengeBoost: return 4.0
        case .weekBoost: return 2.0
        case .fitnessBoost: return 2.5
        case .walkBoost: return 2.0
        }
    }
    
    var duration: TimeInterval {
        switch self {
        case .doubleBoost: return 1800 // 30 Minuten
        case .tripleBoost: return 1800 // 30 Minuten
        case .weekendBoost: return 259200 // 3 Tage
        case .eventBoost: return 86400 // 24 Stunden
        case .challengeBoost: return 21600 // 6 Stunden
        case .weekBoost: return 604800 // 7 Tage
        case .fitnessBoost: return 3600 // 1 Stunde
        case .walkBoost: return 3600 // 1 Stunde
        }
    }
    
    var description: String {
        switch self {
        case .doubleBoost:
            return "Verdoppelt deine StompCoins für 30 Minuten"
        case .tripleBoost:
            return "Verdreifacht deine StompCoins für 30 Minuten"
        case .weekendBoost:
            return "2.5x Boost für das gesamte Wochenende"
        case .eventBoost:
            return "3x Boost für die gesamte Event-Dauer"
        case .challengeBoost:
            return "4x Boost für 6 Stunden während einer Challenge"
        case .weekBoost:
            return "2x Boost für die gesamte Woche (Mo-Do)"
        case .fitnessBoost:
            return "2.5x Boost für den Fitness-Modus"
        case .walkBoost:
            return "2x Boost für den Walk-Modus"
        }
    }
    
    var price: Double {
        switch self {
        case .doubleBoost: return 0.0 // Kostenlos
        case .tripleBoost: return 0.0 // Kostenlos (mit Werbung)
        case .weekendBoost: return 9.99
        case .eventBoost: return 0.0 // Event-spezifisch
        case .challengeBoost: return 0.0 // Challenge-spezifisch
        case .weekBoost: return 12.99
        case .fitnessBoost: return 0.0 // Fitness-Modus-spezifisch
        case .walkBoost: return 0.0 // Walk-Modus-spezifisch
        }
    }
    
    var dailyLimit: Int {
        switch self {
        case .doubleBoost: return 6
        case .tripleBoost: return 6
        default: return 0 // Kein Limit
        }
    }
    
    var cooldownHours: Int {
        switch self {
        case .doubleBoost, .tripleBoost: return 24
        default: return 0 // Kein Cooldown
        }
    }
    
    var requiresAd: Bool {
        switch self {
        case .tripleBoost: return true
        default: return false
        }
    }
    
    var isAvailable: Bool {
        switch self {
        case .weekBoost:
            let calendar = Calendar.current
            let weekday = calendar.component(.weekday, from: Date())
            return weekday >= 2 && weekday <= 5 // Montag bis Donnerstag
        case .weekendBoost:
            let calendar = Calendar.current
            let weekday = calendar.component(.weekday, from: Date())
            return weekday >= 6 || weekday == 1 // Freitag bis Sonntag
        default:
            return true
        }
    }
    
    var isPremiumOnly: Bool {
        switch self {
        case .doubleBoost, .tripleBoost, .weekendBoost, .eventBoost, .challengeBoost:
            return false
        case .weekBoost, .fitnessBoost, .walkBoost:
            return true
        }
    }
    
    var premiumMultiplier: Double {
        switch self {
        case .doubleBoost: return 2.5 // Premium-Bonus
        case .tripleBoost: return 3.5 // Premium-Bonus
        default: return multiplier
        }
    }
}

@Model
class BoostPack {
    var id: UUID
    var type: BoostType
    var price: Double
    var quantity: Int
    var isActive: Bool
    var createdAt: Date
    var lastUsed: Date?
    var usesToday: Int
    
    init(type: BoostType, price: Double, quantity: Int) {
        self.id = UUID()
        self.type = type
        self.price = price
        self.quantity = quantity
        self.isActive = true
        self.createdAt = Date()
        self.usesToday = 0
    }
    
    var canBeUsed: Bool {
        guard isActive else { return false }
        
        // Prüfe Tageslimit
        if type.dailyLimit > 0 && usesToday >= type.dailyLimit {
            return false
        }
        
        // Prüfe Cooldown
        if let lastUsed = lastUsed, type.cooldownHours > 0 {
            let cooldownInterval = TimeInterval(type.cooldownHours * 3600)
            if Date().timeIntervalSince(lastUsed) < cooldownInterval {
                return false
            }
        }
        
        // Prüfe Verfügbarkeit basierend auf Wochentag
        return type.isAvailable
    }
    
    func use() {
        usesToday += 1
        lastUsed = Date()
        
        // Reset usesToday um Mitternacht
        let calendar = Calendar.current
        if let lastUsed = lastUsed,
           !calendar.isDateInToday(lastUsed) {
            usesToday = 0
        }
    }
    
    var effectiveMultiplier: Double {
        // Hier würde die Prüfung auf aktives Premium-Abo kommen
        // Für jetzt geben wir den normalen Multiplikator zurück
        return type.multiplier
    }
    
    var isAvailableForUser: Bool {
        if type.isPremiumOnly {
            // Hier würde die Prüfung auf aktives Premium-Abo kommen
            return false
        }
        return canBeUsed
    }
}

@Model
class TokenPack {
    var id: UUID
    var name: String
    var description: String
    var tokens: Int
    var price: Double
    var bonus: Int
    var isActive: Bool
    var createdAt: Date
    
    init(name: String, description: String, tokens: Int, price: Double, bonus: Int = 0) {
        self.id = UUID()
        self.name = name
        self.description = description
        self.tokens = tokens
        self.price = price
        self.bonus = bonus
        self.isActive = true
        self.createdAt = Date()
    }
}

@Model
class UserBoost {
    var id: UUID
    var user: User
    var type: BoostType
    var startTime: Date
    var endTime: Date
    var isActive: Bool
    
    init(user: User, type: BoostType) {
        self.id = UUID()
        self.user = user
        self.type = type
        self.startTime = Date()
        self.endTime = Date().addingTimeInterval(type.duration)
        self.isActive = true
    }
    
    var remainingTime: TimeInterval {
        max(0, endTime.timeIntervalSince(Date()))
    }
    
    var isExpired: Bool {
        Date() > endTime
    }
}

enum SubscriptionType: String, Codable {
    case threeMonths = "3 Monate Premium"
    case sixMonths = "6 Monate Premium"
    case twelveMonths = "12 Monate Premium"
    
    var duration: TimeInterval {
        switch self {
        case .threeMonths: return 7776000 // 90 Tage
        case .sixMonths: return 15552000 // 180 Tage
        case .twelveMonths: return 31536000 // 365 Tage
        }
    }
    
    var price: Double {
        switch self {
        case .threeMonths: return 39.99
        case .sixMonths: return 69.99
        case .twelveMonths: return 99.99
        }
    }
    
    var description: String {
        switch self {
        case .threeMonths:
            return "Premium-Vorteile für 3 Monate"
        case .sixMonths:
            return "Premium-Vorteile für 6 Monate"
        case .twelveMonths:
            return "Premium-Vorteile für 12 Monate"
        }
    }
    
    var savings: String {
        switch self {
        case .threeMonths:
            return "13,33€ pro Monat"
        case .sixMonths:
            return "11,67€ pro Monat"
        case .twelveMonths:
            return "8,33€ pro Monat"
        }
    }
}

@Model
class PremiumSubscription {
    var id: UUID
    var type: SubscriptionType
    var startDate: Date
    var endDate: Date
    var isActive: Bool
    var autoRenew: Bool
    
    init(type: SubscriptionType) {
        self.id = UUID()
        self.type = type
        self.startDate = Date()
        self.endDate = Date().addingTimeInterval(type.duration)
        self.isActive = true
        self.autoRenew = false
    }
    
    var remainingTime: TimeInterval {
        max(0, endDate.timeIntervalSince(Date()))
    }
    
    var isExpired: Bool {
        Date() > endDate
    }
    
    var daysRemaining: Int {
        Int(remainingTime / 86400)
    }
    
    var premiumBenefits: [String] {
        [
            "Unbegrenzte 2x Boosts",
            "Keine Werbung für 3x Boosts",
            "Exklusive Event-Boosts",
            "Früher Zugang zu neuen Features",
            "Premium-Badge im Profil",
            "Erweiterte Statistiken"
        ]
    }
} 