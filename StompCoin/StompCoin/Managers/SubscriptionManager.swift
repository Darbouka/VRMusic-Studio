import Foundation
import SwiftData

class SubscriptionManager {
    static let shared = SubscriptionManager()
    
    private init() {}
    
    // Abonnement erstellen
    func createSubscription(
        user: User,
        type: SubscriptionType,
        paymentMethod: PaymentMethod,
        modelContext: ModelContext
    ) -> Bool {
        guard type != .freemium else { return false }
        
        let startDate = Date()
        var endDate: Date
        
        // Enddatum basierend auf Abonnement-Typ berechnen
        switch type {
        case .monthly:
            endDate = Calendar.current.date(byAdding: .month, value: 1, to: startDate) ?? startDate
        case .quarterly:
            endDate = Calendar.current.date(byAdding: .month, value: 3, to: startDate) ?? startDate
        case .halfYearly:
            endDate = Calendar.current.date(byAdding: .month, value: 6, to: startDate) ?? startDate
        case .yearly:
            endDate = Calendar.current.date(byAdding: .year, value: 1, to: startDate) ?? startDate
        case .freemium:
            return false
        }
        
        // Preis berechnen
        let price = getPrice(for: type)
        
        // Abonnement erstellen
        let subscription = Subscription(
            user: user,
            type: type,
            startDate: startDate,
            endDate: endDate,
            paymentMethod: paymentMethod,
            price: price,
            currency: .eur
        )
        
        // Benutzer aktualisieren
        user.isPremium = true
        user.subscriptionType = type
        user.subscriptionEndDate = endDate
        
        // Transaktion erstellen
        let transaction = Transaction(
            user: user,
            type: .subscription,
            amount: price,
            currency: .eur,
            description: "Abonnement: \(type.rawValue.capitalized)"
        )
        
        modelContext.insert(subscription)
        modelContext.insert(transaction)
        
        try? modelContext.save()
        return true
    }
    
    // Abonnement kündigen
    func cancelSubscription(user: User, modelContext: ModelContext) -> Bool {
        guard user.isPremium else { return false }
        
        // Abonnement-Status aktualisieren
        if let subscription = getActiveSubscription(for: user) {
            subscription.status = .cancelled
        }
        
        // Benutzer auf Freemium zurücksetzen
        user.isPremium = false
        user.subscriptionType = .freemium
        user.subscriptionEndDate = nil
        
        try? modelContext.save()
        return true
    }
    
    // Abonnement verlängern
    func renewSubscription(user: User, modelContext: ModelContext) -> Bool {
        guard let currentType = user.subscriptionType else { return false }
        guard currentType != .freemium else { return false }
        
        // Neues Enddatum berechnen
        let startDate = Date()
        var endDate: Date
        
        switch currentType {
        case .monthly:
            endDate = Calendar.current.date(byAdding: .month, value: 1, to: startDate) ?? startDate
        case .quarterly:
            endDate = Calendar.current.date(byAdding: .month, value: 3, to: startDate) ?? startDate
        case .halfYearly:
            endDate = Calendar.current.date(byAdding: .month, value: 6, to: startDate) ?? startDate
        case .yearly:
            endDate = Calendar.current.date(byAdding: .year, value: 1, to: startDate) ?? startDate
        case .freemium:
            return false
        }
        
        // Benutzer aktualisieren
        user.subscriptionEndDate = endDate
        
        try? modelContext.save()
        return true
    }
    
    // Abonnement-Status überprüfen
    func checkSubscriptionStatus(user: User, modelContext: ModelContext) {
        guard let endDate = user.subscriptionEndDate else { return }
        
        if Date() > endDate {
            // Abonnement abgelaufen
            user.isPremium = false
            user.subscriptionType = .freemium
            user.subscriptionEndDate = nil
            
            if let subscription = getActiveSubscription(for: user) {
                subscription.status = .expired
            }
            
            try? modelContext.save()
        }
    }
    
    // Hilfsfunktionen
    private func getPrice(for type: SubscriptionType) -> Double {
        switch type {
        case .monthly:
            return 9.99
        case .quarterly:
            return 24.99
        case .halfYearly:
            return 44.99
        case .yearly:
            return 79.99
        case .freemium:
            return 0.0
        }
    }
    
    private func getActiveSubscription(for user: User) -> Subscription? {
        return user.subscriptions.first { $0.status == .active }
    }
} 