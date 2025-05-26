import XCTest
@testable import StompCoin

class IntegrationTests: XCTestCase {
    var networkManager: NetworkManager!
    var cacheManager: CacheManager!
    var syncManager: SyncManager!
    var paymentManager: PaymentManager!
    var videoStreamingService: VideoStreamingService!
    var userManager: UserManager!
    var challengeService: ChallengeService!
    var rewardsService: RewardsService!
    var localizationService: LocalizationService!
    
    override func setUp() {
        super.setUp()
        networkManager = NetworkManager.shared
        cacheManager = CacheManager.shared
        syncManager = SyncManager.shared
        paymentManager = PaymentManager.shared
        videoStreamingService = VideoStreamingService.shared
        userManager = UserManager.shared
        challengeService = ChallengeService.shared
        rewardsService = RewardsService.shared
        localizationService = LocalizationService.shared
    }
    
    override func tearDown() {
        networkManager = nil
        cacheManager = nil
        syncManager = nil
        paymentManager = nil
        videoStreamingService = nil
        userManager = nil
        challengeService = nil
        rewardsService = nil
        localizationService = nil
        super.tearDown()
    }
    
    func testStepSyncIntegration() async throws {
        // Given
        let stepCounter = StepCounterManager()
        stepCounter.startCounting()
        stepCounter.updateSteps(1000)
        
        // When
        try await syncManager.syncData()
        
        // Then
        let cachedSteps = cacheManager.getCachedSteps()
        XCTAssertNotNil(cachedSteps)
        XCTAssertEqual(cachedSteps?.count, 1)
    }
    
    func testEventDetectionIntegration() async throws {
        // Given
        let eventManager = EventDetectionManager()
        
        // When
        let events = try await networkManager.getNearbyEvents()
        try cacheManager.cacheEvents(events)
        
        // Then
        let cachedEvents = cacheManager.getCachedEvents()
        XCTAssertNotNil(cachedEvents)
    }
    
    func testLeaderboardIntegration() async throws {
        // Given
        let timeFrame = TimeFrame.daily
        
        // When
        let entries = try await networkManager.getLeaderboard(timeFrame: timeFrame)
        try cacheManager.cacheLeaderboard(entries, for: timeFrame)
        
        // Then
        let cachedEntries = cacheManager.getCachedLeaderboard(for: timeFrame)
        XCTAssertNotNil(cachedEntries)
    }
    
    func testPaymentIntegration() async throws {
        // Given
        let productId = "com.stompcoin.subscription.3months"
        
        // When
        await paymentManager.loadProducts()
        
        // Then
        XCTAssertFalse(paymentManager.products.isEmpty)
    }
    
    func testBoostIntegration() async throws {
        // Given
        let boostManager = BoostManager()
        let boostType = BoostType.doubleBoost
        
        // When
        let boost = try await boostManager.activateBoost(type: boostType)
        
        // Then
        XCTAssertNotNil(boost)
        XCTAssertTrue(boost.isActive)
    }
    
    func testUserProfileIntegration() async throws {
        // Given
        let username = "testuser"
        let email = "test@example.com"
        let password = "password123"
        
        // When
        let user = try await networkManager.register(username: username, email: email, password: password)
        
        // Then
        XCTAssertNotNil(user)
        XCTAssertEqual(user.username, username)
    }
    
    func testCacheSyncIntegration() async throws {
        // Given
        let steps = [StepEntry(count: 100, timestamp: Date(), type: .dance)]
        try cacheManager.cacheSteps(steps)
        
        // When
        try await syncManager.syncData()
        
        // Then
        let cachedSteps = cacheManager.getCachedSteps()
        XCTAssertNotNil(cachedSteps)
        XCTAssertEqual(cachedSteps?.count, steps.count)
    }
    
    func testErrorHandlingIntegration() async {
        // Given
        let invalidEndpoint = "invalid_endpoint"
        
        // When
        do {
            _ = try await networkManager.makeRequest(endpoint: invalidEndpoint, method: "GET")
            XCTFail("Expected error to be thrown")
        } catch {
            // Then
            XCTAssertTrue(error is NetworkError)
        }
    }
    
    func testOfflineModeIntegration() async throws {
        // Given
        let steps = [StepEntry(count: 100, timestamp: Date(), type: .dance)]
        try cacheManager.cacheSteps(steps)
        
        // When
        // Simuliere Offline-Modus
        networkManager.isOffline = true
        try await syncManager.syncData()
        
        // Then
        let cachedSteps = cacheManager.getCachedSteps()
        XCTAssertNotNil(cachedSteps)
        XCTAssertEqual(cachedSteps?.count, steps.count)
    }
    
    func testDataPersistenceIntegration() async throws {
        // Given
        let userProfile = UserProfile(
            id: UUID(),
            username: "testuser",
            email: "test@example.com",
            createdAt: Date(),
            isPremium: false
        )
        
        // When
        try cacheManager.cacheUserProfile(userProfile)
        
        // Then
        let cachedProfile = cacheManager.getCachedUserProfile()
        XCTAssertNotNil(cachedProfile)
        XCTAssertEqual(cachedProfile?.username, userProfile.username)
    }
    
    // MARK: - Stream Integration Tests
    
    func testStreamingWithRewards() async throws {
        // Benutzer erstellen
        let user = createTestUser()
        
        // Challenge erstellen
        let challenge = createTestChallenge()
        
        // Stream starten
        try await videoStreamingService.startVideoStream(for: challenge)
        
        // Stomps hinzufügen
        try await videoStreamingService.addStomps(1000, for: user.id)
        
        // Belohnungen berechnen
        let rewards = try await rewardsService.calculateRewards(for: user.id, in: challenge)
        
        // Überprüfen
        XCTAssertGreaterThan(rewards.stompCoins, 0)
        XCTAssertGreaterThan(rewards.omCoins, 0)
    }
    
    func testChallengeWithMultipleUsers() async throws {
        // Benutzer erstellen
        let users = (0..<5).map { _ in createTestUser() }
        
        // Challenge erstellen
        let challenge = createTestChallenge()
        
        // Benutzer zur Challenge hinzufügen
        for user in users {
            try await challengeService.joinChallenge(challenge, user: user)
        }
        
        // Stream für jeden Benutzer starten
        for user in users {
            try await videoStreamingService.startVideoStream(for: challenge)
            try await videoStreamingService.addStomps(1000, for: user.id)
        }
        
        // Challenge-Status überprüfen
        let updatedChallenge = try await challengeService.getChallenge(id: challenge.id)
        XCTAssertEqual(updatedChallenge.participants.count, users.count)
    }
    
    func testPremiumFeatures() async throws {
        // Premium-Benutzer erstellen
        let premiumUser = createTestUser(isVIPPremium: true)
        
        // Challenge erstellen
        let challenge = createTestChallenge()
        
        // Stream mit Premium-Features starten
        try await videoStreamingService.startVideoStream(for: challenge)
        try await videoStreamingService.addStomps(1000, for: premiumUser.id)
        
        // Premium-Belohnungen überprüfen
        let rewards = try await rewardsService.calculateRewards(for: premiumUser.id, in: challenge)
        XCTAssertGreaterThan(rewards.stompCoins, 1000)
    }
    
    func testLocalizationIntegration() async throws {
        // Sprachen testen
        let languages = ["de", "en"]
        
        for language in languages {
            // Sprache setzen
            localizationService.setLanguage(language)
            
            // Challenge erstellen
            let challenge = createTestChallenge()
            
            // Stream starten
            try await videoStreamingService.startVideoStream(for: challenge)
            
            // Überprüfen
            XCTAssertEqual(localizationService.currentLanguage, language)
        }
    }
    
    func testRewardsWithDifferentLevels() async throws {
        // Benutzer mit verschiedenen Levels erstellen
        let users = [
            createTestUser(level: 1),
            createTestUser(level: 5),
            createTestUser(level: 10)
        ]
        
        // Challenge erstellen
        let challenge = createTestChallenge()
        
        // Stream für jeden Benutzer starten
        for user in users {
            try await videoStreamingService.startVideoStream(for: challenge)
            try await videoStreamingService.addStomps(1000, for: user.id)
            
            // Belohnungen berechnen
            let rewards = try await rewardsService.calculateRewards(for: user.id, in: challenge)
            
            // Überprüfen
            XCTAssertGreaterThan(rewards.stompCoins, 0)
            XCTAssertGreaterThan(rewards.omCoins, 0)
        }
    }
    
    func testConcurrentStreams() async throws {
        // Benutzer erstellen
        let users = (0..<3).map { _ in createTestUser() }
        
        // Challenge erstellen
        let challenge = createTestChallenge()
        
        // Gleichzeitige Streams starten
        async let streams = users.map { user in
            try await videoStreamingService.startVideoStream(for: challenge)
            try await videoStreamingService.addStomps(1000, for: user.id)
        }
        
        // Warten auf alle Streams
        try await streams
        
        // Überprüfen
        let updatedChallenge = try await challengeService.getChallenge(id: challenge.id)
        XCTAssertEqual(updatedChallenge.participants.count, users.count)
    }
    
    // MARK: - Helper Methods
    
    private func createTestUser(level: Int = 1, isVIPPremium: Bool = false) -> UserManager.User {
        return UserManager.User(
            id: UUID(),
            username: "Test User",
            email: "test@example.com",
            level: level,
            tokens: 100,
            stompCoins: 100,
            isVIPPremium: isVIPPremium,
            isDeveloper: false,
            vipPremiumGrantedBy: nil,
            vipPremiumGrantDate: nil,
            isDeveloperVIPPremium: false
        )
    }
    
    private func createTestChallenge() -> ChallengeService.Challenge {
        return ChallengeService.Challenge(
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
            status: .active,
            type: .event
        )
    }
} 