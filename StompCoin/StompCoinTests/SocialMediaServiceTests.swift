import XCTest
@testable import StompCoin

class SocialMediaServiceTests: XCTestCase {
    var socialMediaService: SocialMediaService!
    var userManager: UserManager!
    var challengeService: ChallengeService!
    
    override func setUp() {
        super.setUp()
        socialMediaService = SocialMediaService.shared
        userManager = UserManager.shared
        challengeService = ChallengeService.shared
    }
    
    override func tearDown() {
        socialMediaService = nil
        userManager = nil
        challengeService = nil
        super.tearDown()
    }
    
    func testStartLiveStream() async throws {
        // Benutzer erstellen
        let user = UserManager.User(
            id: UUID(),
            username: "Test User",
            email: "test@example.com",
            level: 1,
            tokens: 100,
            stompCoins: 100,
            isVIPPremium: false,
            isDeveloper: false,
            vipPremiumGrantedBy: nil,
            vipPremiumGrantDate: nil,
            isDeveloperVIPPremium: false
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
            participants: [user.id],
            confirmedParticipants: [],
            status: .active,
            type: .event
        )
        
        // Benutzer als aktuellen Benutzer setzen
        await MainActor.run {
            userManager.currentUser = user
        }
        
        // LiveStream starten
        try await socialMediaService.startLiveStream(for: challenge, platform: .instagram)
        
        // Prüfen ob Stream erstellt wurde
        let userStream = socialMediaService.getUserStream(user.id)
        XCTAssertNotNil(userStream)
        XCTAssertTrue(userStream?.isActive ?? false)
        XCTAssertEqual(userStream?.platform, .instagram)
    }
    
    func testEndLiveStream() async throws {
        // Benutzer erstellen
        let user = UserManager.User(
            id: UUID(),
            username: "Test User",
            email: "test@example.com",
            level: 1,
            tokens: 100,
            stompCoins: 100,
            isVIPPremium: false,
            isDeveloper: false,
            vipPremiumGrantedBy: nil,
            vipPremiumGrantDate: nil,
            isDeveloperVIPPremium: false
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
            participants: [user.id],
            confirmedParticipants: [],
            status: .active,
            type: .event
        )
        
        // Benutzer als aktuellen Benutzer setzen
        await MainActor.run {
            userManager.currentUser = user
        }
        
        // LiveStream starten
        try await socialMediaService.startLiveStream(for: challenge, platform: .instagram)
        
        // LiveStream beenden
        try await socialMediaService.endLiveStream()
        
        // Prüfen ob Stream beendet wurde
        let userStream = socialMediaService.getUserStream(user.id)
        XCTAssertNotNil(userStream)
        XCTAssertFalse(userStream?.isActive ?? true)
        XCTAssertNotNil(userStream?.endTime)
    }
    
    func testShareChallenge() async throws {
        // Benutzer erstellen
        let user = UserManager.User(
            id: UUID(),
            username: "Test User",
            email: "test@example.com",
            level: 1,
            tokens: 100,
            stompCoins: 100,
            isVIPPremium: false,
            isDeveloper: false,
            vipPremiumGrantedBy: nil,
            vipPremiumGrantDate: nil,
            isDeveloperVIPPremium: false
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
            participants: [user.id],
            confirmedParticipants: [],
            status: .active,
            type: .event
        )
        
        // Benutzer als aktuellen Benutzer setzen
        await MainActor.run {
            userManager.currentUser = user
        }
        
        // Challenge auf mehreren Plattformen teilen
        try await socialMediaService.shareChallenge(challenge, to: [.instagram, .facebook, .twitter])
        
        // Prüfen ob aktive Streams vorhanden sind
        let activeStreams = socialMediaService.getActiveStreams()
        XCTAssertEqual(activeStreams.count, 0) // Keine aktiven Streams, nur geteilt
    }
    
    func testMultipleStreams() async throws {
        // Benutzer erstellen
        let user1 = UserManager.User(
            id: UUID(),
            username: "Test User 1",
            email: "test1@example.com",
            level: 1,
            tokens: 100,
            stompCoins: 100,
            isVIPPremium: false,
            isDeveloper: false,
            vipPremiumGrantedBy: nil,
            vipPremiumGrantDate: nil,
            isDeveloperVIPPremium: false
        )
        
        let user2 = UserManager.User(
            id: UUID(),
            username: "Test User 2",
            email: "test2@example.com",
            level: 1,
            tokens: 100,
            stompCoins: 100,
            isVIPPremium: false,
            isDeveloper: false,
            vipPremiumGrantedBy: nil,
            vipPremiumGrantDate: nil,
            isDeveloperVIPPremium: false
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
            participants: [user1.id, user2.id],
            confirmedParticipants: [],
            status: .active,
            type: .event
        )
        
        // Ersten Benutzer als aktuellen Benutzer setzen
        await MainActor.run {
            userManager.currentUser = user1
        }
        
        // LiveStream für ersten Benutzer starten
        try await socialMediaService.startLiveStream(for: challenge, platform: .instagram)
        
        // Zweiten Benutzer als aktuellen Benutzer setzen
        await MainActor.run {
            userManager.currentUser = user2
        }
        
        // LiveStream für zweiten Benutzer starten
        try await socialMediaService.startLiveStream(for: challenge, platform: .youtube)
        
        // Prüfen ob beide Streams aktiv sind
        let activeStreams = socialMediaService.getActiveStreams(for: challenge)
        XCTAssertEqual(activeStreams.count, 2)
        
        // Prüfen ob Streams auf verschiedenen Plattformen sind
        let platforms = Set(activeStreams.map { $0.platform })
        XCTAssertEqual(platforms.count, 2)
        XCTAssertTrue(platforms.contains(.instagram))
        XCTAssertTrue(platforms.contains(.youtube))
    }
} 