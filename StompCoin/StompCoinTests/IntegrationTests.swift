import XCTest
@testable import StompCoin

class IntegrationTests: XCTestCase {
    var networkManager: NetworkManager!
    var cacheManager: CacheManager!
    var syncManager: SyncManager!
    var paymentManager: PaymentManager!
    
    override func setUp() {
        super.setUp()
        networkManager = NetworkManager.shared
        cacheManager = CacheManager.shared
        syncManager = SyncManager.shared
        paymentManager = PaymentManager.shared
    }
    
    override func tearDown() {
        networkManager = nil
        cacheManager = nil
        syncManager = nil
        paymentManager = nil
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
} 