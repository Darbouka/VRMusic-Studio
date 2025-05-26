import Foundation

class PayoutService {
    static let shared = PayoutService()
    
    private let userDefaults = UserDefaults.standard
    private let developerBalanceKey = "developerBalance"
    
    private var developerBalance: Decimal {
        get {
            return Decimal(userDefaults.double(forKey: developerBalanceKey))
        }
        set {
            userDefaults.set(NSDecimalNumber(decimal: newValue).doubleValue, forKey: developerBalanceKey)
        }
    }
    
    func processDeveloperPayout(amount: Decimal) async throws {
        // Entwickler-Balance aktualisieren
        developerBalance += amount
        
        // Hier die tatsächliche API-Integration für die Auszahlung implementieren
        // Beispiel:
        print("Entwickler-Auszahlung: \(amount) StompCoins")
    }
    
    func getDeveloperBalance() -> Decimal {
        return developerBalance
    }
} 