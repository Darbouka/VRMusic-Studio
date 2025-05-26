import Foundation

enum SubscriptionType: String, Codable {
    case freemium = "Freemium"
    case premium3Months = "3-Monats Premium"
    case premium6Months = "6-Monats Premium"
    case premium12Months = "12-Monats Premium"
    case vipPremium24Months = "24-Monats VIP Premium"
    
    var monthlyFeePercentage: Double {
        switch self {
        case .freemium:
            return 0.40 // 40%
        case .premium3Months:
            return 0.30 // 30%
        case .premium6Months:
            return 0.20 // 20%
        case .premium12Months:
            return 0.10 // 10%
        case .vipPremium24Months:
            return 0.05 // 5%
        }
    }
    
    var durationInMonths: Int {
        switch self {
        case .freemium:
            return 0 // Unbegrenzt
        case .premium3Months:
            return 3
        case .premium6Months:
            return 6
        case .premium12Months:
            return 12
        case .vipPremium24Months:
            return 24
        }
    }
    
    var price: Decimal {
        switch self {
        case .freemium:
            return 0
        case .premium3Months:
            return 9.99
        case .premium6Months:
            return 49.99
        case .premium12Months:
            return 89.99
        case .vipPremium24Months:
            return 179.99
        }
    }
    
    var monthlyBonusTokens: Int {
        switch self {
        case .vipPremium24Months:
            return 2
        default:
            return 0
        }
    }
}

struct Subscription: Codable {
    let id: UUID
    let type: SubscriptionType
    let startDate: Date
    let endDate: Date?
    let isActive: Bool
    let monthlyEarnings: Decimal
    let lastPayoutDate: Date?
    let autoRenew: Bool
    let lastBonusClaimDate: Date?
    
    var nextPayoutDate: Date {
        let calendar = Calendar.current
        let now = Date()
        
        if let lastPayout = lastPayoutDate {
            return calendar.date(byAdding: .month, value: 1, to: lastPayout) ?? now
        }
        
        let components = calendar.dateComponents([.year, .month], from: now)
        guard let lastDayOfMonth = calendar.date(from: DateComponents(year: components.year, month: components.month! + 1, day: 0)) else {
            return now
        }
        return lastDayOfMonth
    }
    
    var nextBonusClaimDate: Date? {
        guard type == .vipPremium24Months else { return nil }
        
        let calendar = Calendar.current
        let now = Date()
        
        if let lastClaim = lastBonusClaimDate {
            let components = calendar.dateComponents([.year, .month], from: lastClaim)
            guard let nextMonth = calendar.date(from: DateComponents(year: components.year, month: components.month! + 1, day: 1)) else {
                return now
            }
            return nextMonth
        }
        
        // Wenn noch keine Bonus-Abholung erfolgt ist, nächster Monatserster
        let components = calendar.dateComponents([.year, .month], from: now)
        guard let firstDayOfNextMonth = calendar.date(from: DateComponents(year: components.year, month: components.month! + 1, day: 1)) else {
            return now
        }
        return firstDayOfNextMonth
    }
    
    var developerPayout: Decimal {
        return monthlyEarnings * Decimal(type.monthlyFeePercentage)
    }
    
    var userPayout: Decimal {
        return monthlyEarnings - developerPayout
    }
}

class SubscriptionManager: ObservableObject {
    @Published var currentSubscription: Subscription?
    @Published var monthlyEarnings: Decimal = 0
    @Published var availableBonusTokens: Int = 0
    
    func calculateMonthlyPayout() -> (developer: Decimal, user: Decimal) {
        guard let subscription = currentSubscription else {
            return (developer: monthlyEarnings * 0.4, user: monthlyEarnings * 0.6)
        }
        
        let developerPayout = monthlyEarnings * Decimal(subscription.type.monthlyFeePercentage)
        let userPayout = monthlyEarnings - developerPayout
        
        return (developer: developerPayout, user: userPayout)
    }
    
    func processMonthlyPayout() {
        let payouts = calculateMonthlyPayout()
        
        // Hier die Logik für die Auszahlung implementieren
        // 1. Entwickler-Auszahlung
        // 2. Nutzer-Auszahlung
        // 3. Transaktionshistorie aktualisieren
        // 4. LastPayoutDate aktualisieren
    }
    
    func claimMonthlyBonus() {
        guard let subscription = currentSubscription,
              subscription.type == .vipPremium24Months,
              let nextClaimDate = subscription.nextBonusClaimDate,
              Date() >= nextClaimDate else {
            return
        }
        
        // Bonus-Tokens hinzufügen
        availableBonusTokens += subscription.type.monthlyBonusTokens
        
        // LastBonusClaimDate aktualisieren
        // Hier die Logik für die Aktualisierung implementieren
    }
    
    func cancelSubscription() {
        // E-Mail an Support senden
        let supportEmail = "support@stompcoin.com"
        let subject = "Kündigung Abonnement"
        let body = """
        Sehr geehrtes StompCoin Team,
        
        ich möchte mein Abonnement kündigen.
        
        Mit freundlichen Grüßen
        """
        
        if let url = URL(string: "mailto:\(supportEmail)?subject=\(subject)&body=\(body)") {
            UIApplication.shared.open(url)
        }
    }
} 