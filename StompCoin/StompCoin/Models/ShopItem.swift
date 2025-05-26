import Foundation

enum ItemType: String, Codable {
    case token = "Token"
    case avatar = "Avatar"
    case theme = "Theme"
    case effect = "Effect"
    case boost = "Boost"
}

enum ItemRarity: String, Codable {
    case common = "Common"
    case rare = "Rare"
    case epic = "Epic"
    case legendary = "Legendary"
    case exclusive = "Exclusive"
}

struct ShopItem: Identifiable, Codable {
    let id: UUID
    let name: String
    let description: String
    let type: ItemType
    let rarity: ItemRarity
    let price: Decimal
    let isPremium: Bool
    let imageURL: URL?
    let previewURL: URL?
    let availableUntil: Date?
    let requiredLevel: Int
    let requiredSubscription: SubscriptionType?
    
    var isAvailable: Bool {
        if let availableUntil = availableUntil {
            return Date() < availableUntil
        }
        return true
    }
}

class ShopManager: ObservableObject {
    static let shared = ShopManager()
    
    @Published var availableItems: [ShopItem] = []
    @Published var ownedItems: [UUID: ShopItem] = [:]
    @Published var featuredItems: [ShopItem] = []
    
    private let userDefaults = UserDefaults.standard
    private let ownedItemsKey = "ownedItems"
    
    init() {
        loadOwnedItems()
        loadShopItems()
    }
    
    private func loadOwnedItems() {
        if let data = userDefaults.data(forKey: ownedItemsKey),
           let items = try? JSONDecoder().decode([UUID: ShopItem].self, from: data) {
            ownedItems = items
        }
    }
    
    private func loadShopItems() {
        // Hier die tatsächlichen Shop-Items laden
        // Beispiel-Items:
        availableItems = [
            ShopItem(
                id: UUID(),
                name: "Goldener Avatar",
                description: "Exklusiver goldener Avatar-Rahmen",
                type: .avatar,
                rarity: .legendary,
                price: 99.99,
                isPremium: true,
                imageURL: URL(string: "https://stompcoin.com/items/golden_avatar.png"),
                previewURL: URL(string: "https://stompcoin.com/items/golden_avatar_preview.mp4"),
                availableUntil: nil,
                requiredLevel: 50,
                requiredSubscription: .vipPremium24Months
            ),
            ShopItem(
                id: UUID(),
                name: "Stomp Boost",
                description: "Verdoppelt deine StompCoin-Einnahmen für 24 Stunden",
                type: .boost,
                rarity: .epic,
                price: 4.99,
                isPremium: false,
                imageURL: URL(string: "https://stompcoin.com/items/stomp_boost.png"),
                previewURL: nil,
                availableUntil: nil,
                requiredLevel: 10,
                requiredSubscription: nil
            )
        ]
        
        // Featured Items aktualisieren
        updateFeaturedItems()
    }
    
    private func updateFeaturedItems() {
        featuredItems = availableItems
            .filter { $0.isAvailable }
            .sorted { $0.rarity.rawValue > $1.rarity.rawValue }
            .prefix(5)
            .map { $0 }
    }
    
    func purchaseItem(_ item: ShopItem) async throws {
        // 1. Verfügbarkeit prüfen
        guard item.isAvailable else {
            throw ShopError.itemNotAvailable
        }
        
        // 2. Level-Anforderung prüfen
        guard let userLevel = await UserManager.shared.currentUser?.level,
              userLevel >= item.requiredLevel else {
            throw ShopError.levelTooLow
        }
        
        // 3. Abonnement-Anforderung prüfen
        if let requiredSubscription = item.requiredSubscription {
            guard let currentSubscription = SubscriptionService.shared.currentSubscription,
                  currentSubscription.type == requiredSubscription else {
                throw ShopError.subscriptionRequired
            }
        }
        
        // 4. Zahlung durchführen
        try await processPayment(amount: item.price)
        
        // 5. Item zum Inventar hinzufügen
        await MainActor.run {
            ownedItems[item.id] = item
            saveOwnedItems()
        }
    }
    
    private func processPayment(amount: Decimal) async throws {
        // Hier die tatsächliche Zahlungsabwicklung implementieren
        // Ähnlich wie im SubscriptionService
    }
    
    private func saveOwnedItems() {
        if let data = try? JSONEncoder().encode(ownedItems) {
            userDefaults.set(data, forKey: ownedItemsKey)
        }
    }
}

enum ShopError: Error {
    case itemNotAvailable
    case levelTooLow
    case subscriptionRequired
    case paymentFailed
    case insufficientFunds
} 