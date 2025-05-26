import XCTest
@testable import StompCoin

// MARK: - Test Extensions

extension XCTestCase {
    // MARK: - Timeout Helpers
    
    func waitForTimeout(_ timeout: TimeInterval = TestConstants.defaultTimeout) {
        let expectation = XCTestExpectation(description: "Timeout")
        DispatchQueue.main.asyncAfter(deadline: .now() + timeout) {
            expectation.fulfill()
        }
        wait(for: [expectation], timeout: timeout + 1)
    }
    
    func waitForShortTimeout() {
        waitForTimeout(TestConstants.shortTimeout)
    }
    
    func waitForLongTimeout() {
        waitForTimeout(TestConstants.longTimeout)
    }
    
    // MARK: - Async Helpers
    
    func asyncExpectation(description: String) -> XCTestExpectation {
        return expectation(description: description)
    }
    
    func asyncWait(for expectations: [XCTestExpectation], timeout: TimeInterval = TestConstants.defaultTimeout) {
        wait(for: expectations, timeout: timeout)
    }
    
    func asyncWaitForShortTimeout(for expectations: [XCTestExpectation]) {
        asyncWait(for: expectations, timeout: TestConstants.shortTimeout)
    }
    
    func asyncWaitForLongTimeout(for expectations: [XCTestExpectation]) {
        asyncWait(for: expectations, timeout: TestConstants.longTimeout)
    }
    
    // MARK: - Error Helpers
    
    func XCTAssertThrowsError<T>(_ expression: @autoclosure () throws -> T, _ message: @autoclosure () -> String = "", file: StaticString = #file, line: UInt = #line, _ errorHandler: (_ error: Error) -> Void = { _ in }) {
        do {
            _ = try expression()
            XCTFail(message(), file: file, line: line)
        } catch {
            errorHandler(error)
        }
    }
    
    func XCTAssertNoThrow<T>(_ expression: @autoclosure () throws -> T, _ message: @autoclosure () -> String = "", file: StaticString = #file, line: UInt = #line) {
        do {
            _ = try expression()
        } catch {
            XCTFail(message(), file: file, line: line)
        }
    }
    
    // MARK: - User Helpers
    
    func createStandardUser() -> UserManager.User {
        return TestFixtures.shared.standardUser
    }
    
    func createPremiumUser() -> UserManager.User {
        return TestFixtures.shared.premiumUser
    }
    
    func createDeveloperUser() -> UserManager.User {
        return TestFixtures.shared.developerUser
    }
    
    // MARK: - Challenge Helpers
    
    func createActiveChallenge() -> ChallengeService.Challenge {
        return TestFixtures.shared.activeChallenge
    }
    
    func createCompletedChallenge() -> ChallengeService.Challenge {
        return TestFixtures.shared.completedChallenge
    }
    
    func createCancelledChallenge() -> ChallengeService.Challenge {
        return TestFixtures.shared.cancelledChallenge
    }
    
    // MARK: - Stream Helpers
    
    func createActiveStream(userId: UUID, challengeId: UUID) -> VideoStreamingService.Stream {
        return TestFixtures.shared.createActiveStream(userId: userId, challengeId: challengeId)
    }
    
    func createEndedStream(userId: UUID, challengeId: UUID) -> VideoStreamingService.Stream {
        return TestFixtures.shared.createEndedStream(userId: userId, challengeId: challengeId)
    }
    
    func createPrivateStream(userId: UUID, challengeId: UUID) -> VideoStreamingService.Stream {
        return TestFixtures.shared.createPrivateStream(userId: userId, challengeId: challengeId)
    }
    
    // MARK: - Rewards Helpers
    
    func createStandardRewards() -> RewardsService.Rewards {
        return TestFixtures.shared.standardRewards
    }
    
    func createPremiumRewards() -> RewardsService.Rewards {
        return TestFixtures.shared.premiumRewards
    }
    
    func createDeveloperRewards() -> RewardsService.Rewards {
        return TestFixtures.shared.developerRewards
    }
    
    // MARK: - Localization Helpers
    
    func createGermanLocalization() -> [String: String] {
        return TestFixtures.shared.germanLocalization
    }
    
    func createEnglishLocalization() -> [String: String] {
        return TestFixtures.shared.englishLocalization
    }
    
    // MARK: - Error Helpers
    
    func createGeneralError() -> TestErrors {
        return .generalError
    }
    
    func createTimeoutError() -> TestErrors {
        return .timeoutError
    }
    
    func createValidationError() -> TestErrors {
        return .validationError
    }
    
    func createNetworkError() -> TestErrors {
        return .networkError
    }
    
    func createStorageError() -> TestErrors {
        return .storageError
    }
    
    // MARK: - User Error Helpers
    
    func createUserNotFoundError() -> TestErrors {
        return .userNotFound
    }
    
    func createUserAlreadyExistsError() -> TestErrors {
        return .userAlreadyExists
    }
    
    func createInvalidUserDataError() -> TestErrors {
        return .invalidUserData
    }
    
    func createUserNotAuthorizedError() -> TestErrors {
        return .userNotAuthorized
    }
    
    func createUserNotPremiumError() -> TestErrors {
        return .userNotPremium
    }
    
    func createUserNotDeveloperError() -> TestErrors {
        return .userNotDeveloper
    }
    
    // MARK: - Challenge Error Helpers
    
    func createChallengeNotFoundError() -> TestErrors {
        return .challengeNotFound
    }
    
    func createChallengeAlreadyExistsError() -> TestErrors {
        return .challengeAlreadyExists
    }
    
    func createInvalidChallengeDataError() -> TestErrors {
        return .invalidChallengeData
    }
    
    func createChallengeNotActiveError() -> TestErrors {
        return .challengeNotActive
    }
    
    func createChallengeAlreadyCompletedError() -> TestErrors {
        return .challengeAlreadyCompleted
    }
    
    func createChallengeAlreadyCancelledError() -> TestErrors {
        return .challengeAlreadyCancelled
    }
    
    func createChallengeNotStartedError() -> TestErrors {
        return .challengeNotStarted
    }
    
    func createChallengeAlreadyStartedError() -> TestErrors {
        return .challengeAlreadyStarted
    }
    
    func createChallengeNotEndedError() -> TestErrors {
        return .challengeNotEnded
    }
    
    func createChallengeAlreadyEndedError() -> TestErrors {
        return .challengeAlreadyEnded
    }
    
    // MARK: - Stream Error Helpers
    
    func createStreamNotFoundError() -> TestErrors {
        return .streamNotFound
    }
    
    func createStreamAlreadyExistsError() -> TestErrors {
        return .streamAlreadyExists
    }
    
    func createInvalidStreamDataError() -> TestErrors {
        return .invalidStreamData
    }
    
    func createStreamNotActiveError() -> TestErrors {
        return .streamNotActive
    }
    
    func createStreamAlreadyActiveError() -> TestErrors {
        return .streamAlreadyActive
    }
    
    func createStreamNotEndedError() -> TestErrors {
        return .streamNotEnded
    }
    
    func createStreamAlreadyEndedError() -> TestErrors {
        return .streamAlreadyEnded
    }
    
    func createStreamNotPrivateError() -> TestErrors {
        return .streamNotPrivate
    }
    
    func createStreamAlreadyPrivateError() -> TestErrors {
        return .streamAlreadyPrivate
    }
    
    func createStreamNotPublicError() -> TestErrors {
        return .streamNotPublic
    }
    
    func createStreamAlreadyPublicError() -> TestErrors {
        return .streamAlreadyPublic
    }
    
    // MARK: - Rewards Error Helpers
    
    func createRewardsNotFoundError() -> TestErrors {
        return .rewardsNotFound
    }
    
    func createRewardsAlreadyExistsError() -> TestErrors {
        return .rewardsAlreadyExists
    }
    
    func createInvalidRewardsDataError() -> TestErrors {
        return .invalidRewardsData
    }
    
    func createRewardsNotCalculatedError() -> TestErrors {
        return .rewardsNotCalculated
    }
    
    func createRewardsAlreadyCalculatedError() -> TestErrors {
        return .rewardsAlreadyCalculated
    }
    
    func createRewardsNotDistributedError() -> TestErrors {
        return .rewardsNotDistributed
    }
    
    func createRewardsAlreadyDistributedError() -> TestErrors {
        return .rewardsAlreadyDistributed
    }
    
    // MARK: - Localization Error Helpers
    
    func createLocalizationNotFoundError() -> TestErrors {
        return .localizationNotFound
    }
    
    func createLocalizationAlreadyExistsError() -> TestErrors {
        return .localizationAlreadyExists
    }
    
    func createInvalidLocalizationDataError() -> TestErrors {
        return .invalidLocalizationData
    }
    
    func createLanguageNotSupportedError() -> TestErrors {
        return .languageNotSupported
    }
    
    func createLanguageAlreadySetError() -> TestErrors {
        return .languageAlreadySet
    }
    
    // MARK: - Camera Error Helpers
    
    func createCameraNotFoundError() -> TestErrors {
        return .cameraNotFound
    }
    
    func createCameraNotAvailableError() -> TestErrors {
        return .cameraNotAvailable
    }
    
    func createCameraNotAuthorizedError() -> TestErrors {
        return .cameraNotAuthorized
    }
    
    func createCameraAlreadyInUseError() -> TestErrors {
        return .cameraAlreadyInUse
    }
    
    func createCameraNotInitializedError() -> TestErrors {
        return .cameraNotInitialized
    }
    
    // MARK: - Network Error Helpers
    
    func createNetworkNotAvailableError() -> TestErrors {
        return .networkNotAvailable
    }
    
    func createNetworkNotConnectedError() -> TestErrors {
        return .networkNotConnected
    }
    
    func createNetworkNotAuthorizedError() -> TestErrors {
        return .networkNotAuthorized
    }
    
    func createNetworkNotInitializedError() -> TestErrors {
        return .networkNotInitialized
    }
    
    func createNetworkNotConfiguredError() -> TestErrors {
        return .networkNotConfigured
    }
    
    // MARK: - Storage Error Helpers
    
    func createStorageNotAvailableError() -> TestErrors {
        return .storageNotAvailable
    }
    
    func createStorageNotConnectedError() -> TestErrors {
        return .storageNotConnected
    }
    
    func createStorageNotAuthorizedError() -> TestErrors {
        return .storageNotAuthorized
    }
    
    func createStorageNotInitializedError() -> TestErrors {
        return .storageNotInitialized
    }
    
    func createStorageNotConfiguredError() -> TestErrors {
        return .storageNotConfigured
    }
    
    // MARK: - UI Error Helpers
    
    func createUINotAvailableError() -> TestErrors {
        return .uiNotAvailable
    }
    
    func createUINotConnectedError() -> TestErrors {
        return .uiNotConnected
    }
    
    func createUINotAuthorizedError() -> TestErrors {
        return .uiNotAuthorized
    }
    
    func createUINotInitializedError() -> TestErrors {
        return .uiNotInitialized
    }
    
    func createUINotConfiguredError() -> TestErrors {
        return .uiNotConfigured
    }
}

// MARK: - XCTestCase Extensions

extension XCTestCase {
    func waitForExpectations(timeout: TimeInterval = 5.0) {
        waitForExpectations(timeout: timeout) { error in
            if let error = error {
                XCTFail("Timeout waiting for expectations: \(error)")
            }
        }
    }
    
    func waitForMainQueue() {
        let expectation = XCTestExpectation(description: "Wait for main queue")
        DispatchQueue.main.async {
            expectation.fulfill()
        }
        wait(for: [expectation], timeout: 5.0)
    }
}

// MARK: - UserManager.User Extensions

extension UserManager.User {
    static func createTestUser(level: Int = 1, isVIPPremium: Bool = false) -> UserManager.User {
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
    
    func withUpdatedUsername(_ username: String) -> UserManager.User {
        var updatedUser = self
        updatedUser.username = username
        return updatedUser
    }
    
    func withUpdatedLevel(_ level: Int) -> UserManager.User {
        var updatedUser = self
        updatedUser.level = level
        return updatedUser
    }
    
    func withUpdatedTokens(_ tokens: Int) -> UserManager.User {
        var updatedUser = self
        updatedUser.tokens = tokens
        return updatedUser
    }
    
    func withUpdatedStompCoins(_ stompCoins: Int) -> UserManager.User {
        var updatedUser = self
        updatedUser.stompCoins = stompCoins
        return updatedUser
    }
}

// MARK: - ChallengeService.Challenge Extensions

extension ChallengeService.Challenge {
    static func createTestChallenge() -> ChallengeService.Challenge {
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
    
    func withUpdatedTitle(_ title: String) -> ChallengeService.Challenge {
        var updatedChallenge = self
        updatedChallenge.title = title
        return updatedChallenge
    }
    
    func withUpdatedStatus(_ status: ChallengeService.ChallengeStatus) -> ChallengeService.Challenge {
        var updatedChallenge = self
        updatedChallenge.status = status
        return updatedChallenge
    }
    
    func withAddedParticipant(_ participant: UserManager.User) -> ChallengeService.Challenge {
        var updatedChallenge = self
        updatedChallenge.participants.append(participant)
        return updatedChallenge
    }
    
    func withRemovedParticipant(_ participant: UserManager.User) -> ChallengeService.Challenge {
        var updatedChallenge = self
        updatedChallenge.participants.removeAll { $0.id == participant.id }
        return updatedChallenge
    }
}

// MARK: - VideoStreamingService.Stream Extensions

extension VideoStreamingService.Stream {
    static func createTestStream(userId: UUID, challengeId: UUID) -> VideoStreamingService.Stream {
        return VideoStreamingService.Stream(
            id: UUID(),
            userId: userId,
            challengeId: challengeId,
            isActive: true,
            startTime: Date(),
            endTime: nil,
            totalStomps: 0,
            earnedStompCoins: 0,
            earnedOMCoins: 0,
            isPrivate: false,
            viewers: []
        )
    }
    
    func withUpdatedStomps(_ totalStomps: Int) -> VideoStreamingService.Stream {
        var updatedStream = self
        updatedStream.totalStomps = totalStomps
        updatedStream.earnedStompCoins = totalStomps
        updatedStream.earnedOMCoins = totalStomps / 1000
        return updatedStream
    }
    
    func withEndedStream() -> VideoStreamingService.Stream {
        var updatedStream = self
        updatedStream.isActive = false
        updatedStream.endTime = Date()
        return updatedStream
    }
    
    func withAddedViewer(_ viewer: UserManager.User) -> VideoStreamingService.Stream {
        var updatedStream = self
        updatedStream.viewers.append(viewer)
        return updatedStream
    }
    
    func withRemovedViewer(_ viewer: UserManager.User) -> VideoStreamingService.Stream {
        var updatedStream = self
        updatedStream.viewers.removeAll { $0.id == viewer.id }
        return updatedStream
    }
}

// MARK: - RewardsService.Rewards Extensions

extension RewardsService.Rewards {
    static func createTestRewards() -> RewardsService.Rewards {
        return RewardsService.Rewards(
            stompCoins: 1000,
            omCoins: 10,
            level: 1,
            experience: 100
        )
    }
    
    func withUpdatedStompCoins(_ stompCoins: Int) -> RewardsService.Rewards {
        var updatedRewards = self
        updatedRewards.stompCoins = stompCoins
        return updatedRewards
    }
    
    func withUpdatedOMCoins(_ omCoins: Int) -> RewardsService.Rewards {
        var updatedRewards = self
        updatedRewards.omCoins = omCoins
        return updatedRewards
    }
    
    func withUpdatedLevel(_ level: Int) -> RewardsService.Rewards {
        var updatedRewards = self
        updatedRewards.level = level
        return updatedRewards
    }
    
    func withUpdatedExperience(_ experience: Int) -> RewardsService.Rewards {
        var updatedRewards = self
        updatedRewards.experience = experience
        return updatedRewards
    }
}

// MARK: - LocalizationService Extensions

extension LocalizationService {
    static func createTestLocalizationService() -> LocalizationService {
        let service = LocalizationService()
        service.setLanguage("de")
        return service
    }
    
    func withLanguage(_ language: String) -> LocalizationService {
        let service = LocalizationService()
        service.setLanguage(language)
        return service
    }
} 