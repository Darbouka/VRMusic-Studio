import XCTest
@testable import StompCoin

class ChallengeServiceTests: XCTestCase {
    var challengeService: ChallengeService!
    var userManager: UserManager!
    
    override func setUp() {
        super.setUp()
        challengeService = ChallengeService.shared
        userManager = UserManager.shared
    }
    
    override func tearDown() {
        challengeService = nil
        userManager = nil
        super.tearDown()
    }
    
    func testNormalUserJoiningChallenge() async throws {
        // Normaler Benutzer erstellen
        let normalUser = UserManager.User(
            id: UUID(),
            username: "Normal User",
            email: "user@example.com",
            level: 1,
            tokens: 1000,
            stompCoins: 1000,
            isVIPPremium: false,
            isDeveloper: false,
            vipPremiumGrantedBy: nil,
            vipPremiumGrantDate: nil
        )
        
        // Challenge erstellen
        let challenge = ChallengeService.Challenge(
            id: UUID(),
            title: "Test Challenge",
            description: "Test Description",
            eventId: UUID(),
            startDate: Date(),
            endDate: Calendar.current.date(byAdding: .day, value: 7, to: Date())!,
            requiredTokens: 100,
            requiredStompCoins: 200,
            participants: [],
            confirmedParticipants: [],
            status: .upcoming,
            type: .event
        )
        
        // Benutzer als aktuellen Benutzer setzen
        await MainActor.run {
            userManager.currentUser = normalUser
        }
        
        // Challenge beitreten
        try await challengeService.joinChallenge(challenge)
        
        // Prüfen ob Gebühren abgezogen wurden
        XCTAssertEqual(userManager.currentUser?.tokens, 900)
        XCTAssertEqual(userManager.currentUser?.stompCoins, 800)
        
        // Prüfen ob Benutzer zur Challenge hinzugefügt wurde
        let updatedChallenge = challengeService.getChallenges().first { $0.id == challenge.id }
        XCTAssertNotNil(updatedChallenge)
        XCTAssertTrue(updatedChallenge?.participants.contains(normalUser.id) ?? false)
    }
    
    func testVIPPremiumUserJoiningChallenge() async throws {
        // VIP Premium Benutzer erstellen
        let vipUser = UserManager.User(
            id: UUID(),
            username: "VIP User",
            email: "vip@example.com",
            level: 100,
            tokens: 1000,
            stompCoins: 1000,
            isVIPPremium: true,
            isDeveloper: false,
            vipPremiumGrantedBy: nil,
            vipPremiumGrantDate: nil
        )
        
        // Challenge erstellen
        let challenge = ChallengeService.Challenge(
            id: UUID(),
            title: "Test Challenge",
            description: "Test Description",
            eventId: UUID(),
            startDate: Date(),
            endDate: Calendar.current.date(byAdding: .day, value: 7, to: Date())!,
            requiredTokens: 100,
            requiredStompCoins: 200,
            participants: [],
            confirmedParticipants: [],
            status: .upcoming,
            type: .event
        )
        
        // Benutzer als aktuellen Benutzer setzen
        await MainActor.run {
            userManager.currentUser = vipUser
        }
        
        // Challenge beitreten
        try await challengeService.joinChallenge(challenge)
        
        // Prüfen ob keine Gebühren abgezogen wurden
        XCTAssertEqual(userManager.currentUser?.tokens, 1000)
        XCTAssertEqual(userManager.currentUser?.stompCoins, 1000)
        
        // Prüfen ob Benutzer zur Challenge hinzugefügt wurde
        let updatedChallenge = challengeService.getChallenges().first { $0.id == challenge.id }
        XCTAssertNotNil(updatedChallenge)
        XCTAssertTrue(updatedChallenge?.participants.contains(vipUser.id) ?? false)
    }
    
    func testDeveloperJoiningChallenge() async throws {
        // Entwickler-Benutzer erstellen
        let developer = UserManager.User(
            id: UUID(),
            username: "Developer",
            email: "dev@stompcoin.com",
            level: 100,
            tokens: 1000,
            stompCoins: 1000,
            isVIPPremium: true,
            isDeveloper: true,
            vipPremiumGrantedBy: nil,
            vipPremiumGrantDate: nil
        )
        
        // Challenge erstellen
        let challenge = ChallengeService.Challenge(
            id: UUID(),
            title: "Test Challenge",
            description: "Test Description",
            eventId: UUID(),
            startDate: Date(),
            endDate: Calendar.current.date(byAdding: .day, value: 7, to: Date())!,
            requiredTokens: 100,
            requiredStompCoins: 200,
            participants: [],
            confirmedParticipants: [],
            status: .upcoming,
            type: .event
        )
        
        // Benutzer als aktuellen Benutzer setzen
        await MainActor.run {
            userManager.currentUser = developer
        }
        
        // Challenge beitreten
        try await challengeService.joinChallenge(challenge)
        
        // Prüfen ob keine Gebühren abgezogen wurden
        XCTAssertEqual(userManager.currentUser?.tokens, 1000)
        XCTAssertEqual(userManager.currentUser?.stompCoins, 1000)
        
        // Prüfen ob Benutzer zur Challenge hinzugefügt wurde
        let updatedChallenge = challengeService.getChallenges().first { $0.id == challenge.id }
        XCTAssertNotNil(updatedChallenge)
        XCTAssertTrue(updatedChallenge?.participants.contains(developer.id) ?? false)
    }
    
    func testChallengeStatusUpdates() async throws {
        // Challenge erstellen
        let challenge = ChallengeService.Challenge(
            id: UUID(),
            title: "Test Challenge",
            description: "Test Description",
            eventId: UUID(),
            startDate: Date(),
            endDate: Calendar.current.date(byAdding: .day, value: 7, to: Date())!,
            requiredTokens: 100,
            requiredStompCoins: 200,
            participants: [],
            confirmedParticipants: [],
            status: .upcoming,
            type: .event
        )
        
        // Challenge hinzufügen
        await MainActor.run {
            challengeService.challenges.append(challenge)
        }
        
        // Status aktualisieren
        try await challengeService.updateChallengeStatuses()
        
        // Prüfen ob Status aktualisiert wurde
        let updatedChallenge = challengeService.getChallenges().first { $0.id == challenge.id }
        XCTAssertNotNil(updatedChallenge)
        XCTAssertEqual(updatedChallenge?.status, .active)
    }
} 