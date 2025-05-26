import XCTest
@testable import StompCoin

class UserManagerTests: XCTestCase {
    var userManager: UserManager!
    
    override func setUp() {
        super.setUp()
        userManager = UserManager.shared
    }
    
    override func tearDown() {
        userManager = nil
        super.tearDown()
    }
    
    func testAuthorizedDeveloperVIPPremiumGranting() async throws {
        // Autorisierten Entwickler erstellen
        let developer = UserManager.User(
            id: UserManager.Developer.fatihAcci.id,
            username: "Fatih Acci",
            email: "fatih@stompcoin.com",
            level: 100,
            tokens: 1000,
            stompCoins: 1000,
            isVIPPremium: true,
            isDeveloper: true,
            vipPremiumGrantedBy: nil,
            vipPremiumGrantDate: nil,
            isDeveloperVIPPremium: true
        )
        
        // Normalen Benutzer erstellen
        let normalUser = UserManager.User(
            id: UUID(),
            username: "Normal User",
            email: "user@example.com",
            level: 1,
            tokens: 100,
            stompCoins: 100,
            isVIPPremium: false,
            isDeveloper: false,
            vipPremiumGrantedBy: nil,
            vipPremiumGrantDate: nil,
            isDeveloperVIPPremium: false
        )
        
        // Entwickler als aktuellen Benutzer setzen
        await MainActor.run {
            userManager.currentUser = developer
        }
        
        // VIP Premium vergeben
        try await userManager.grantVIPPremium(to: normalUser.id)
        
        // Prüfen ob VIP Premium vergeben wurde
        XCTAssertTrue(userManager.isVIPPremiumUser(normalUser.id))
        XCTAssertTrue(userManager.isDeveloperVIPPremium(normalUser.id))
    }
    
    func testUnauthorizedDeveloperVIPPremiumGranting() async throws {
        // Nicht autorisierten Entwickler erstellen
        let unauthorizedDeveloper = UserManager.User(
            id: UUID(),
            username: "Unauthorized Developer",
            email: "unauthorized@stompcoin.com",
            level: 100,
            tokens: 1000,
            stompCoins: 1000,
            isVIPPremium: true,
            isDeveloper: true,
            vipPremiumGrantedBy: nil,
            vipPremiumGrantDate: nil,
            isDeveloperVIPPremium: true
        )
        
        // Normalen Benutzer erstellen
        let normalUser = UserManager.User(
            id: UUID(),
            username: "Normal User",
            email: "user@example.com",
            level: 1,
            tokens: 100,
            stompCoins: 100,
            isVIPPremium: false,
            isDeveloper: false,
            vipPremiumGrantedBy: nil,
            vipPremiumGrantDate: nil,
            isDeveloperVIPPremium: false
        )
        
        // Nicht autorisierten Entwickler als aktuellen Benutzer setzen
        await MainActor.run {
            userManager.currentUser = unauthorizedDeveloper
        }
        
        // VIP Premium vergeben sollte fehlschlagen
        do {
            try await userManager.grantVIPPremium(to: normalUser.id)
            XCTFail("VIP Premium Vererbung sollte fehlschlagen")
        } catch {
            XCTAssertEqual((error as NSError).code, 2)
        }
    }
    
    func testDeveloperVIPPremiumNoFees() async throws {
        // Entwickler-VIP Premium Benutzer erstellen
        let developerVIPUser = UserManager.User(
            id: UUID(),
            username: "Developer VIP User",
            email: "devvip@example.com",
            level: 100,
            tokens: 1000,
            stompCoins: 1000,
            isVIPPremium: true,
            isDeveloper: false,
            vipPremiumGrantedBy: UserManager.Developer.fatihAcci.id,
            vipPremiumGrantDate: Date(),
            isDeveloperVIPPremium: true
        )
        
        // Benutzer als aktuellen Benutzer setzen
        await MainActor.run {
            userManager.currentUser = developerVIPUser
        }
        
        // Gebühren abziehen
        try await userManager.deductTokens(50)
        try await userManager.deductStompCoins(50)
        
        // Prüfen ob keine Gebühren abgezogen wurden
        XCTAssertEqual(userManager.currentUser?.tokens, 1000)
        XCTAssertEqual(userManager.currentUser?.stompCoins, 1000)
    }
    
    func testNormalVIPPremiumNoFees() async throws {
        // Normaler VIP Premium Benutzer erstellen
        let normalVIPUser = UserManager.User(
            id: UUID(),
            username: "Normal VIP User",
            email: "vip@example.com",
            level: 100,
            tokens: 1000,
            stompCoins: 1000,
            isVIPPremium: true,
            isDeveloper: false,
            vipPremiumGrantedBy: nil,
            vipPremiumGrantDate: nil,
            isDeveloperVIPPremium: false
        )
        
        // Benutzer als aktuellen Benutzer setzen
        await MainActor.run {
            userManager.currentUser = normalVIPUser
        }
        
        // Gebühren abziehen
        try await userManager.deductTokens(50)
        try await userManager.deductStompCoins(50)
        
        // Prüfen ob keine Gebühren abgezogen wurden
        XCTAssertEqual(userManager.currentUser?.tokens, 1000)
        XCTAssertEqual(userManager.currentUser?.stompCoins, 1000)
    }
    
    func testNormalUserFees() async throws {
        // Normaler Benutzer erstellen
        let normalUser = UserManager.User(
            id: UUID(),
            username: "Normal User",
            email: "user@example.com",
            level: 1,
            tokens: 100,
            stompCoins: 100,
            isVIPPremium: false,
            isDeveloper: false,
            vipPremiumGrantedBy: nil,
            vipPremiumGrantDate: nil,
            isDeveloperVIPPremium: false
        )
        
        // Benutzer als aktuellen Benutzer setzen
        await MainActor.run {
            userManager.currentUser = normalUser
        }
        
        // Gebühren abziehen
        try await userManager.deductTokens(50)
        try await userManager.deductStompCoins(50)
        
        // Prüfen ob Gebühren abgezogen wurden
        XCTAssertEqual(userManager.currentUser?.tokens, 50)
        XCTAssertEqual(userManager.currentUser?.stompCoins, 50)
    }
} 