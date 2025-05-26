import XCTest
@testable import StompCoin

class VideoStreamingServiceTests: XCTestCase {
    var videoStreamingService: VideoStreamingService!
    var userManager: UserManager!
    var challengeService: ChallengeService!
    
    override func setUp() {
        super.setUp()
        videoStreamingService = VideoStreamingService.shared
        userManager = UserManager.shared
        challengeService = ChallengeService.shared
    }
    
    override func tearDown() {
        videoStreamingService = nil
        userManager = nil
        challengeService = nil
        super.tearDown()
    }
    
    func testFreemiumUserOMCoinEarning() async throws {
        // Freemium Benutzer erstellen
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
        
        // Stream starten
        try await videoStreamingService.startVideoStream(for: challenge)
        
        // Stomps hinzufügen
        try await videoStreamingService.addStomps(100_000, for: user.id)
        
        // Prüfen ob OM Coin verdient wurde
        let progress = videoStreamingService.getOMCoinProgress(for: user.id)
        XCTAssertEqual(progress.earned, 1)
        XCTAssertEqual(progress.progress, 1.0)
        XCTAssertNotNil(progress.timeRemaining)
    }
    
    func testFreemiumUserOMCoinTimeWindow() async throws {
        // Freemium Benutzer erstellen
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
        
        // Stream starten
        try await videoStreamingService.startVideoStream(for: challenge)
        
        // Erste 50.000 Stomps hinzufügen
        try await videoStreamingService.addStomps(50_000, for: user.id)
        
        // Prüfen ob Fortschritt korrekt ist
        var progress = videoStreamingService.getOMCoinProgress(for: user.id)
        XCTAssertEqual(progress.earned, 0)
        XCTAssertEqual(progress.progress, 0.5)
        
        // Weitere 50.000 Stomps hinzufügen
        try await videoStreamingService.addStomps(50_000, for: user.id)
        
        // Prüfen ob OM Coin verdient wurde
        progress = videoStreamingService.getOMCoinProgress(for: user.id)
        XCTAssertEqual(progress.earned, 1)
        XCTAssertEqual(progress.progress, 1.0)
    }
    
    func testPremiumUserOMCoinEarning() async throws {
        // Premium Benutzer erstellen
        let user = UserManager.User(
            id: UUID(),
            username: "Test User",
            email: "test@example.com",
            level: 1,
            tokens: 100,
            stompCoins: 100,
            isVIPPremium: true,
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
        
        // Stream starten
        try await videoStreamingService.startVideoStream(for: challenge)
        
        // Stomps hinzufügen
        try await videoStreamingService.addStomps(100_000, for: user.id)
        
        // Prüfen ob OM Coin verdient wurde
        let progress = videoStreamingService.getOMCoinProgress(for: user.id)
        XCTAssertEqual(progress.earned, 1)
        XCTAssertEqual(progress.progress, 1.0)
        XCTAssertNil(progress.timeRemaining) // Premium User haben kein Zeitfenster
    }
    
    func testMultipleOMCoinsInTimeWindow() async throws {
        // Freemium Benutzer erstellen
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
        
        // Stream starten
        try await videoStreamingService.startVideoStream(for: challenge)
        
        // 200.000 Stomps hinzufügen (sollte 2 OM Coins ergeben)
        try await videoStreamingService.addStomps(200_000, for: user.id)
        
        // Prüfen ob 2 OM Coins verdient wurden
        let progress = videoStreamingService.getOMCoinProgress(for: user.id)
        XCTAssertEqual(progress.earned, 2)
        XCTAssertEqual(progress.progress, 1.0)
    }
} 