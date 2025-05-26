import Foundation

class UserManager: ObservableObject {
    static let shared = UserManager()
    
    struct User: Codable {
        let id: UUID
        var username: String
        var email: String
        var level: Int
        var tokens: Int
        var stompCoins: Int
        var isVIPPremium: Bool
        var isDeveloper: Bool
        var vipPremiumGrantedBy: UUID?
        var vipPremiumGrantDate: Date?
        var isDeveloperVIPPremium: Bool
    }
    
    enum Developer: String, CaseIterable {
        case fatihAcci = "Fatih Acci"
        case oscarArtes = "Oscar Artes"
        case shirleyDavidsen = "Shirley Davidsen"
        
        var id: UUID {
            switch self {
            case .fatihAcci:
                return UUID(uuidString: "00000000-0000-0000-0000-000000000001")!
            case .oscarArtes:
                return UUID(uuidString: "00000000-0000-0000-0000-000000000002")!
            case .shirleyDavidsen:
                return UUID(uuidString: "00000000-0000-0000-0000-000000000003")!
            }
        }
    }
    
    @Published private(set) var currentUser: User?
    
    private let userDefaults = UserDefaults.standard
    private let currentUserKey = "currentUser"
    
    init() {
        loadCurrentUser()
    }
    
    private func loadCurrentUser() {
        if let data = userDefaults.data(forKey: currentUserKey),
           let user = try? JSONDecoder().decode(User.self, from: data) {
            currentUser = user
        }
    }
    
    private func saveCurrentUser() {
        if let user = currentUser,
           let data = try? JSONEncoder().encode(user) {
            userDefaults.set(data, forKey: currentUserKey)
        }
    }
    
    func grantVIPPremium(to userId: UUID) async throws {
        guard let currentUser = currentUser,
              isDeveloper(currentUser.id) else {
            throw NSError(domain: "UserManager", code: 1, userInfo: [NSLocalizedDescriptionKey: "Nur Entwickler können VIP Premium vergeben"])
        }
        
        // Prüfen ob der aktuelle Benutzer einer der autorisierten Entwickler ist
        guard Developer.allCases.contains(where: { $0.id == currentUser.id }) else {
            throw NSError(domain: "UserManager", code: 2, userInfo: [NSLocalizedDescriptionKey: "Nur autorisierte Entwickler können VIP Premium vergeben"])
        }
        
        // Hier die tatsächliche API-Integration implementieren
        // Beispiel:
        let updatedUser = User(
            id: userId,
            username: "VIP User",
            email: "vip@example.com",
            level: 100,
            tokens: Int.max,
            stompCoins: Int.max,
            isVIPPremium: true,
            isDeveloper: false,
            vipPremiumGrantedBy: currentUser.id,
            vipPremiumGrantDate: Date(),
            isDeveloperVIPPremium: true
        )
        
        await MainActor.run {
            self.currentUser = updatedUser
            saveCurrentUser()
        }
    }
    
    func deductTokens(_ amount: Int) async throws {
        guard var user = currentUser else {
            throw NSError(domain: "UserManager", code: 2, userInfo: [NSLocalizedDescriptionKey: "Benutzer nicht angemeldet"])
        }
        
        // Entwickler-VIP Premium und normale VIP Premium zahlen keine Gebühren
        if user.isDeveloperVIPPremium || (user.isVIPPremium && !user.isDeveloperVIPPremium) {
            return
        }
        
        guard user.tokens >= amount else {
            throw NSError(domain: "UserManager", code: 3, userInfo: [NSLocalizedDescriptionKey: "Nicht genügend Token"])
        }
        
        user.tokens -= amount
        
        await MainActor.run {
            currentUser = user
            saveCurrentUser()
        }
    }
    
    func deductStompCoins(_ amount: Int) async throws {
        guard var user = currentUser else {
            throw NSError(domain: "UserManager", code: 2, userInfo: [NSLocalizedDescriptionKey: "Benutzer nicht angemeldet"])
        }
        
        // Entwickler-VIP Premium und normale VIP Premium zahlen keine Gebühren
        if user.isDeveloperVIPPremium || (user.isVIPPremium && !user.isDeveloperVIPPremium) {
            return
        }
        
        guard user.stompCoins >= amount else {
            throw NSError(domain: "UserManager", code: 4, userInfo: [NSLocalizedDescriptionKey: "Nicht genügend StompCoins"])
        }
        
        user.stompCoins -= amount
        
        await MainActor.run {
            currentUser = user
            saveCurrentUser()
        }
    }
    
    func isVIPPremiumUser(_ userId: UUID) -> Bool {
        guard let user = currentUser else { return false }
        return user.isVIPPremium || user.isDeveloperVIPPremium
    }
    
    func isDeveloper(_ userId: UUID) -> Bool {
        guard let user = currentUser else { return false }
        return user.isDeveloper
    }
    
    func isDeveloperVIPPremium(_ userId: UUID) -> Bool {
        guard let user = currentUser else { return false }
        return user.isDeveloperVIPPremium
    }
    
    func isAuthorizedDeveloper(_ userId: UUID) -> Bool {
        return Developer.allCases.contains(where: { $0.id == userId })
    }
} 