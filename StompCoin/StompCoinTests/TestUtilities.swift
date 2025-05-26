import XCTest
@testable import StompCoin

// MARK: - Test Utilities

class TestUtilities {
    static let shared = TestUtilities()
    
    private init() {}
    
    // MARK: - Date Utilities
    
    func createDate(daysFromNow: Int) -> Date {
        return Calendar.current.date(byAdding: .day, value: daysFromNow, to: Date())!
    }
    
    func createDateRange(startDaysFromNow: Int, endDaysFromNow: Int) -> (start: Date, end: Date) {
        let startDate = createDate(daysFromNow: startDaysFromNow)
        let endDate = createDate(daysFromNow: endDaysFromNow)
        return (startDate, endDate)
    }
    
    // MARK: - UUID Utilities
    
    func createUUIDs(count: Int) -> [UUID] {
        return (0..<count).map { _ in UUID() }
    }
    
    // MARK: - String Utilities
    
    func createRandomString(length: Int) -> String {
        let letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
        return String((0..<length).map { _ in letters.randomElement()! })
    }
    
    func createRandomEmail() -> String {
        return "\(createRandomString(length: 8))@example.com"
    }
    
    // MARK: - Array Utilities
    
    func createRandomArray<T>(count: Int, generator: () -> T) -> [T] {
        return (0..<count).map { _ in generator() }
    }
    
    // MARK: - Dictionary Utilities
    
    func createRandomDictionary<K, V>(count: Int, keyGenerator: () -> K, valueGenerator: () -> V) -> [K: V] {
        var dictionary: [K: V] = [:]
        for _ in 0..<count {
            dictionary[keyGenerator()] = valueGenerator()
        }
        return dictionary
    }
    
    // MARK: - Test Data Generators
    
    func generateTestUsers(count: Int) -> [UserManager.User] {
        return createRandomArray(count: count) {
            UserManager.User(
                id: UUID(),
                username: createRandomString(length: 8),
                email: createRandomEmail(),
                level: Int.random(in: 1...100),
                tokens: Int.random(in: 0...1000),
                stompCoins: Int.random(in: 0...1000),
                isVIPPremium: Bool.random(),
                isDeveloper: Bool.random(),
                vipPremiumGrantedBy: Bool.random() ? UUID() : nil,
                vipPremiumGrantDate: Bool.random() ? Date() : nil,
                isDeveloperVIPPremium: Bool.random()
            )
        }
    }
    
    func generateTestChallenges(count: Int) -> [ChallengeService.Challenge] {
        return createRandomArray(count: count) {
            ChallengeService.Challenge(
                id: UUID(),
                title: createRandomString(length: 10),
                description: createRandomString(length: 50),
                eventId: UUID(),
                startDate: createDate(daysFromNow: Int.random(in: -7...0)),
                endDate: createDate(daysFromNow: Int.random(in: 1...7)),
                requiredTokens: Int.random(in: 0...1000),
                requiredStompCoins: Int.random(in: 0...1000),
                participants: [],
                confirmedParticipants: [],
                status: ChallengeService.ChallengeStatus.allCases.randomElement()!,
                type: ChallengeService.ChallengeType.allCases.randomElement()!
            )
        }
    }
    
    func generateTestStreams(count: Int, userId: UUID, challengeId: UUID) -> [VideoStreamingService.Stream] {
        return createRandomArray(count: count) {
            VideoStreamingService.Stream(
                id: UUID(),
                userId: userId,
                challengeId: challengeId,
                isActive: Bool.random(),
                startTime: createDate(daysFromNow: Int.random(in: -7...0)),
                endTime: Bool.random() ? createDate(daysFromNow: Int.random(in: 0...7)) : nil,
                totalStomps: Int.random(in: 0...10000),
                earnedStompCoins: Int.random(in: 0...10000),
                earnedOMCoins: Int.random(in: 0...100),
                isPrivate: Bool.random(),
                viewers: []
            )
        }
    }
    
    func generateTestRewards() -> RewardsService.Rewards {
        return RewardsService.Rewards(
            stompCoins: Int.random(in: 0...10000),
            omCoins: Int.random(in: 0...100),
            level: Int.random(in: 1...100),
            experience: Int.random(in: 0...1000)
        )
    }
    
    // MARK: - Test Assertions
    
    func assertUser(_ user: UserManager.User, hasLevel level: Int) {
        XCTAssertEqual(user.level, level)
    }
    
    func assertUser(_ user: UserManager.User, hasStompCoins stompCoins: Int) {
        XCTAssertEqual(user.stompCoins, stompCoins)
    }
    
    func assertChallenge(_ challenge: ChallengeService.Challenge, hasStatus status: ChallengeService.ChallengeStatus) {
        XCTAssertEqual(challenge.status, status)
    }
    
    func assertChallenge(_ challenge: ChallengeService.Challenge, hasParticipantCount count: Int) {
        XCTAssertEqual(challenge.participants.count, count)
    }
    
    func assertStream(_ stream: VideoStreamingService.Stream, isActive active: Bool) {
        XCTAssertEqual(stream.isActive, active)
    }
    
    func assertStream(_ stream: VideoStreamingService.Stream, hasStomps stomps: Int) {
        XCTAssertEqual(stream.totalStomps, stomps)
    }
    
    func assertRewards(_ rewards: RewardsService.Rewards, hasStompCoins stompCoins: Int) {
        XCTAssertEqual(rewards.stompCoins, stompCoins)
    }
    
    func assertRewards(_ rewards: RewardsService.Rewards, hasOMCoins omCoins: Int) {
        XCTAssertEqual(rewards.omCoins, omCoins)
    }
} 