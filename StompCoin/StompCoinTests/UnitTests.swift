import XCTest
@testable import StompCoin

class UnitTests: XCTestCase {
    var videoStreamingService: VideoStreamingService!
    var userManager: UserManager!
    var challengeService: ChallengeService!
    var rewardsService: RewardsService!
    var localizationService: LocalizationService!
    
    override func setUp() {
        super.setUp()
        videoStreamingService = VideoStreamingService.shared
        userManager = UserManager.shared
        challengeService = ChallengeService.shared
        rewardsService = RewardsService.shared
        localizationService = LocalizationService.shared
    }
    
    override func tearDown() {
        videoStreamingService = nil
        userManager = nil
        challengeService = nil
        rewardsService = nil
        localizationService = nil
        super.tearDown()
    }
    
    // MARK: - VideoStreamingService Tests
    
    func testStartVideoStream() async throws {
        let user = createTestUser()
        let challenge = createTestChallenge()
        
        try await videoStreamingService.startVideoStream(for: challenge)
        
        let stream = videoStreamingService.getUserStream(user.id)
        XCTAssertNotNil(stream)
        XCTAssertTrue(stream?.isActive ?? false)
    }
    
    func testEndVideoStream() async throws {
        let user = createTestUser()
        let challenge = createTestChallenge()
        
        try await videoStreamingService.startVideoStream(for: challenge)
        try await videoStreamingService.endVideoStream()
        
        let stream = videoStreamingService.getUserStream(user.id)
        XCTAssertNotNil(stream)
        XCTAssertFalse(stream?.isActive ?? true)
    }
    
    func testAddStomps() async throws {
        let user = createTestUser()
        let challenge = createTestChallenge()
        
        try await videoStreamingService.startVideoStream(for: challenge)
        try await videoStreamingService.addStomps(1000, for: user.id)
        
        let stream = videoStreamingService.getUserStream(user.id)
        XCTAssertEqual(stream?.totalStomps, 1000)
    }
    
    func testSwitchCamera() async throws {
        let user = createTestUser()
        let challenge = createTestChallenge()
        
        try await videoStreamingService.startVideoStream(for: challenge)
        try await videoStreamingService.switchCamera()
        
        let stream = videoStreamingService.getUserStream(user.id)
        XCTAssertNotNil(stream)
    }
    
    // MARK: - UserManager Tests
    
    func testCreateUser() async throws {
        let user = createTestUser()
        
        try await userManager.createUser(user)
        
        let createdUser = try await userManager.getUser(id: user.id)
        XCTAssertEqual(createdUser.id, user.id)
        XCTAssertEqual(createdUser.username, user.username)
    }
    
    func testUpdateUser() async throws {
        let user = createTestUser()
        try await userManager.createUser(user)
        
        var updatedUser = user
        updatedUser.username = "Updated Username"
        try await userManager.updateUser(updatedUser)
        
        let fetchedUser = try await userManager.getUser(id: user.id)
        XCTAssertEqual(fetchedUser.username, "Updated Username")
    }
    
    func testDeleteUser() async throws {
        let user = createTestUser()
        try await userManager.createUser(user)
        
        try await userManager.deleteUser(id: user.id)
        
        do {
            _ = try await userManager.getUser(id: user.id)
            XCTFail("User should be deleted")
        } catch {
            XCTAssertTrue(error is UserManagerError)
        }
    }
    
    // MARK: - ChallengeService Tests
    
    func testCreateChallenge() async throws {
        let challenge = createTestChallenge()
        
        try await challengeService.createChallenge(challenge)
        
        let createdChallenge = try await challengeService.getChallenge(id: challenge.id)
        XCTAssertEqual(createdChallenge.id, challenge.id)
        XCTAssertEqual(createdChallenge.title, challenge.title)
    }
    
    func testJoinChallenge() async throws {
        let user = createTestUser()
        let challenge = createTestChallenge()
        try await challengeService.createChallenge(challenge)
        
        try await challengeService.joinChallenge(challenge, user: user)
        
        let updatedChallenge = try await challengeService.getChallenge(id: challenge.id)
        XCTAssertTrue(updatedChallenge.participants.contains { $0.id == user.id })
    }
    
    func testLeaveChallenge() async throws {
        let user = createTestUser()
        let challenge = createTestChallenge()
        try await challengeService.createChallenge(challenge)
        try await challengeService.joinChallenge(challenge, user: user)
        
        try await challengeService.leaveChallenge(challenge, user: user)
        
        let updatedChallenge = try await challengeService.getChallenge(id: challenge.id)
        XCTAssertFalse(updatedChallenge.participants.contains { $0.id == user.id })
    }
    
    // MARK: - RewardsService Tests
    
    func testCalculateRewards() async throws {
        let user = createTestUser()
        let challenge = createTestChallenge()
        
        let rewards = try await rewardsService.calculateRewards(for: user.id, in: challenge)
        
        XCTAssertGreaterThan(rewards.stompCoins, 0)
        XCTAssertGreaterThan(rewards.omCoins, 0)
    }
    
    func testPremiumRewards() async throws {
        let premiumUser = createTestUser(isVIPPremium: true)
        let challenge = createTestChallenge()
        
        let rewards = try await rewardsService.calculateRewards(for: premiumUser.id, in: challenge)
        
        XCTAssertGreaterThan(rewards.stompCoins, 1000)
        XCTAssertGreaterThan(rewards.omCoins, 1)
    }
    
    // MARK: - LocalizationService Tests
    
    func testSetLanguage() {
        localizationService.setLanguage("en")
        XCTAssertEqual(localizationService.currentLanguage, "en")
        
        localizationService.setLanguage("de")
        XCTAssertEqual(localizationService.currentLanguage, "de")
    }
    
    func testLocalizedString() {
        localizationService.setLanguage("en")
        XCTAssertEqual(localizationService.localizedString("streaming.start"), "Start Streaming")
        
        localizationService.setLanguage("de")
        XCTAssertEqual(localizationService.localizedString("streaming.start"), "Streaming starten")
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