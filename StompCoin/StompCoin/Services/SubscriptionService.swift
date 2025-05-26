import Foundation
import StoreKit

class SubscriptionService: ObservableObject {
    static let shared = SubscriptionService()
    
    @Published var currentSubscription: Subscription?
    @Published var availableBonusTokens: Int = 0
    @Published var lastBonusClaimDate: Date?
    
    private let userDefaults = UserDefaults.standard
    private let bonusTokensKey = "availableBonusTokens"
    private let lastClaimDateKey = "lastBonusClaimDate"
    
    init() {
        loadSubscriptionState()
    }
    
    private func loadSubscriptionState() {
        availableBonusTokens = userDefaults.integer(forKey: bonusTokensKey)
        if let dateData = userDefaults.data(forKey: lastClaimDateKey),
           let date = try? JSONDecoder().decode(Date.self, from: dateData) {
            lastBonusClaimDate = date
        }
    }
    
    func purchaseSubscription(_ type: SubscriptionType) async throws {
        // 1. Zahlungsabwicklung
        let paymentResult = try await processPayment(amount: type.price)
        
        // 2. Abonnement aktivieren
        if paymentResult.success {
            let subscription = Subscription(
                id: UUID(),
                type: type,
                startDate: Date(),
                endDate: Calendar.current.date(byAdding: .month, value: type.durationInMonths, to: Date()),
                isActive: true,
                monthlyEarnings: 0,
                lastPayoutDate: nil,
                autoRenew: true,
                lastBonusClaimDate: nil
            )
            
            await MainActor.run {
                self.currentSubscription = subscription
            }
            
            // 3. Bestätigung an Nutzer
            NotificationCenter.default.post(
                name: NSNotification.Name("SubscriptionActivated"),
                object: nil,
                userInfo: ["type": type.rawValue]
            )
        }
    }
    
    private func processPayment(amount: Decimal) async throws -> (success: Bool, transactionId: String?) {
        // Hier die tatsächliche Zahlungsabwicklung implementieren
        // Beispiel mit StoreKit:
        let product = try await Product.products(for: ["com.stompcoin.subscription"])
        let result = try await product.first?.purchase()
        
        switch result {
        case .success(let verification):
            return (true, verification.transaction.id)
        case .failure(let error):
            throw error
        case .none:
            throw NSError(domain: "SubscriptionService", code: 1, userInfo: [NSLocalizedDescriptionKey: "Produkt nicht gefunden"])
        }
    }
    
    func claimMonthlyBonus() async throws {
        guard let subscription = currentSubscription,
              subscription.type == .vipPremium24Months,
              let nextClaimDate = subscription.nextBonusClaimDate,
              Date() >= nextClaimDate else {
            throw NSError(domain: "SubscriptionService", code: 2, userInfo: [NSLocalizedDescriptionKey: "Bonus nicht verfügbar"])
        }
        
        // Bonus-Tokens hinzufügen
        await MainActor.run {
            availableBonusTokens += subscription.type.monthlyBonusTokens
            lastBonusClaimDate = Date()
            
            // Speichern
            userDefaults.set(availableBonusTokens, forKey: bonusTokensKey)
            if let dateData = try? JSONEncoder().encode(lastBonusClaimDate) {
                userDefaults.set(dateData, forKey: lastClaimDateKey)
            }
        }
    }
    
    func cancelSubscription() async throws {
        guard let subscription = currentSubscription else {
            throw NSError(domain: "SubscriptionService", code: 3, userInfo: [NSLocalizedDescriptionKey: "Kein aktives Abonnement"])
        }
        
        // E-Mail an Support senden
        let supportEmail = "support@stompcoin.com"
        let subject = "Kündigung Abonnement"
        let body = """
        Sehr geehrtes StompCoin Team,
        
        ich möchte mein Abonnement kündigen.
        Abonnement-ID: \(subscription.id)
        Typ: \(subscription.type.rawValue)
        Laufzeit bis: \(subscription.endDate?.formatted() ?? "Unbekannt")
        
        Mit freundlichen Grüßen
        """
        
        if let url = URL(string: "mailto:\(supportEmail)?subject=\(subject)&body=\(body)") {
            await MainActor.run {
                UIApplication.shared.open(url)
            }
        }
    }
    
    func processMonthlyPayout() async throws {
        guard let subscription = currentSubscription else { return }
        
        let developerPayout = subscription.monthlyEarnings * Decimal(subscription.type.monthlyFeePercentage)
        let userPayout = subscription.monthlyEarnings - developerPayout
        
        // Entwickler-Auszahlung
        try await PayoutService.shared.processDeveloperPayout(amount: developerPayout)
        
        // Nutzer-Auszahlung
        try await PayoutService.shared.processUserPayout(amount: userPayout)
        
        // LastPayoutDate aktualisieren
        await MainActor.run {
            if let index = currentSubscription?.id {
                currentSubscription?.lastPayoutDate = Date()
            }
        }
    }
} 