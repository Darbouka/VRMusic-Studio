import Foundation
import SwiftData

class CoinManager {
    static let shared = CoinManager()
    
    private init() {}
    
    // Coin-Generierung basierend auf Schritten
    func generateCoins(from steps: Int, user: User, modelContext: ModelContext) -> Double {
        let baseRate = 0.1 // 0.1 StompCoins pro Schritt
        var multiplier = 1.0
        
        // Premium-Bonus
        if user.isPremium {
            multiplier *= 1.5
        }
        
        // Event-Boost
        if let activeEvent = getActiveEvent(for: user) {
            multiplier *= activeEvent.boostMultiplier
        }
        
        let coins = Double(steps) * baseRate * multiplier
        user.stompCoins += coins
        
        // Transaktion erstellen
        let transaction = Transaction(
            user: user,
            type: .reward,
            amount: coins,
            currency: .stompCoin,
            description: "Coins generiert durch \(steps) Schritte"
        )
        modelContext.insert(transaction)
        
        try? modelContext.save()
        return coins
    }
    
    // Coin-Konvertierung
    func convertCoins(
        amount: Double,
        from: Currency,
        to: Currency,
        user: User,
        modelContext: ModelContext
    ) -> Bool {
        guard amount > 0 else { return false }
        
        let conversionRate: Double
        switch (from, to) {
        case (.stompCoin, .omCoin):
            conversionRate = 0.1 // 10 StompCoins = 1 OM Coin
            guard user.stompCoins >= amount else { return false }
            user.stompCoins -= amount
            user.omCoins += amount * conversionRate
        case (.omCoin, .stompCoin):
            conversionRate = 10.0 // 1 OM Coin = 10 StompCoins
            guard user.omCoins >= amount else { return false }
            user.omCoins -= amount
            user.stompCoins += amount * conversionRate
        default:
            return false
        }
        
        // Transaktion erstellen
        let transaction = Transaction(
            user: user,
            type: .conversion,
            amount: amount,
            currency: from,
            description: "Konvertierung von \(from.rawValue) zu \(to.rawValue)"
        )
        modelContext.insert(transaction)
        
        try? modelContext.save()
        return true
    }
    
    // Coin-Kauf
    func purchaseCoins(
        amount: Double,
        paymentMethod: PaymentMethod,
        user: User,
        modelContext: ModelContext
    ) -> Bool {
        guard amount > 0 else { return false }
        
        // Hier würde die Zahlungsabwicklung implementiert werden
        // Für jetzt simulieren wir einen erfolgreichen Kauf
        
        user.stompCoins += amount
        
        // Transaktion erstellen
        let transaction = Transaction(
            user: user,
            type: .purchase,
            amount: amount,
            currency: .stompCoin,
            description: "Kauf von \(Int(amount)) StompCoins"
        )
        modelContext.insert(transaction)
        
        try? modelContext.save()
        return true
    }
    
    // Hilfsfunktion für aktive Events
    private func getActiveEvent(for user: User) -> Event? {
        return user.eventHistory.first { $0.event.isActive }
    }
} 