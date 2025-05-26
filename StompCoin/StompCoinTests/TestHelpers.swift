import XCTest
@testable import StompCoin

// MARK: - Test Helpers

class TestHelpers {
    static let shared = TestHelpers()
    
    private init() {}
    
    // MARK: - User Helpers
    
    func createUser(id: UUID = UUID(), username: String = "Test User", email: String = "test@example.com", level: Int = TestConstants.defaultLevel, tokens: Int = TestConstants.defaultTokens, stompCoins: Int = TestConstants.defaultStompCoins, isVIPPremium: Bool = false, isDeveloper: Bool = false, vipPremiumGrantedBy: UUID? = nil, vipPremiumGrantDate: Date? = nil, isDeveloperVIPPremium: Bool = false) -> UserManager.User {
        return UserManager.User(
            id: id,
            username: username,
            email: email,
            level: level,
            tokens: tokens,
            stompCoins: stompCoins,
            isVIPPremium: isVIPPremium,
            isDeveloper: isDeveloper,
            vipPremiumGrantedBy: vipPremiumGrantedBy,
            vipPremiumGrantDate: vipPremiumGrantDate,
            isDeveloperVIPPremium: isDeveloperVIPPremium
        )
    }
    
    func createStandardUser() -> UserManager.User {
        return createUser()
    }
    
    func createPremiumUser() -> UserManager.User {
        return createUser(
            username: "Premium User",
            email: "premium@example.com",
            level: TestConstants.premiumLevel,
            tokens: TestConstants.premiumTokens,
            stompCoins: TestConstants.premiumStompCoins,
            isVIPPremium: true
        )
    }
    
    func createDeveloperUser() -> UserManager.User {
        return createUser(
            username: "Developer User",
            email: "developer@example.com",
            level: TestConstants.developerLevel,
            tokens: TestConstants.developerTokens,
            stompCoins: TestConstants.developerStompCoins,
            isVIPPremium: true,
            isDeveloper: true,
            isDeveloperVIPPremium: true
        )
    }
    
    // MARK: - Challenge Helpers
    
    func createChallenge(id: UUID = UUID(), title: String = "Test Challenge", description: String = "Test Description", eventId: UUID = UUID(), startDate: Date = Date(), endDate: Date = Calendar.current.date(byAdding: .day, value: TestConstants.challengeDurationDays, to: Date())!, requiredTokens: Int = TestConstants.defaultRequiredTokens, requiredStompCoins: Int = TestConstants.defaultRequiredStompCoins, participants: [UUID] = [], confirmedParticipants: [UUID] = [], status: ChallengeService.ChallengeStatus = .active, type: ChallengeService.ChallengeType = .event) -> ChallengeService.Challenge {
        return ChallengeService.Challenge(
            id: id,
            title: title,
            description: description,
            eventId: eventId,
            startDate: startDate,
            endDate: endDate,
            requiredTokens: requiredTokens,
            requiredStompCoins: requiredStompCoins,
            participants: participants,
            confirmedParticipants: confirmedParticipants,
            status: status,
            type: type
        )
    }
    
    func createActiveChallenge() -> ChallengeService.Challenge {
        return createChallenge()
    }
    
    func createCompletedChallenge() -> ChallengeService.Challenge {
        return createChallenge(
            title: "Completed Challenge",
            description: "A completed challenge",
            startDate: Calendar.current.date(byAdding: .day, value: TestConstants.challengeStartOffsetDays, to: Date())!,
            endDate: Date(),
            status: .completed
        )
    }
    
    func createCancelledChallenge() -> ChallengeService.Challenge {
        return createChallenge(
            title: "Cancelled Challenge",
            description: "A cancelled challenge",
            status: .cancelled
        )
    }
    
    // MARK: - Stream Helpers
    
    func createStream(id: UUID = UUID(), userId: UUID = UUID(), challengeId: UUID = UUID(), isActive: Bool = true, startTime: Date = Date(), endTime: Date? = nil, totalStomps: Int = 0, earnedStompCoins: Int = 0, earnedOMCoins: Int = 0, isPrivate: Bool = false, viewers: [UUID] = []) -> VideoStreamingService.Stream {
        return VideoStreamingService.Stream(
            id: id,
            userId: userId,
            challengeId: challengeId,
            isActive: isActive,
            startTime: startTime,
            endTime: endTime,
            totalStomps: totalStomps,
            earnedStompCoins: earnedStompCoins,
            earnedOMCoins: earnedOMCoins,
            isPrivate: isPrivate,
            viewers: viewers
        )
    }
    
    func createActiveStream(userId: UUID = UUID(), challengeId: UUID = UUID()) -> VideoStreamingService.Stream {
        return createStream(userId: userId, challengeId: challengeId)
    }
    
    func createEndedStream(userId: UUID = UUID(), challengeId: UUID = UUID()) -> VideoStreamingService.Stream {
        return createStream(
            userId: userId,
            challengeId: challengeId,
            isActive: false,
            startTime: Calendar.current.date(byAdding: .hour, value: -1, to: Date())!,
            endTime: Date(),
            totalStomps: TestConstants.defaultStomps,
            earnedStompCoins: TestConstants.defaultEarnedStompCoins,
            earnedOMCoins: TestConstants.defaultEarnedOMCoins
        )
    }
    
    func createPrivateStream(userId: UUID = UUID(), challengeId: UUID = UUID()) -> VideoStreamingService.Stream {
        return createStream(
            userId: userId,
            challengeId: challengeId,
            isPrivate: true
        )
    }
    
    // MARK: - Rewards Helpers
    
    func createRewards(stompCoins: Int = TestConstants.defaultRewardStompCoins, omCoins: Int = TestConstants.defaultRewardOMCoins, level: Int = TestConstants.defaultRewardLevel, experience: Int = TestConstants.defaultRewardExperience) -> RewardsService.Rewards {
        return RewardsService.Rewards(
            stompCoins: stompCoins,
            omCoins: omCoins,
            level: level,
            experience: experience
        )
    }
    
    func createStandardRewards() -> RewardsService.Rewards {
        return createRewards()
    }
    
    func createPremiumRewards() -> RewardsService.Rewards {
        return createRewards(
            stompCoins: TestConstants.premiumRewardStompCoins,
            omCoins: TestConstants.premiumRewardOMCoins,
            level: TestConstants.premiumRewardLevel,
            experience: TestConstants.premiumRewardExperience
        )
    }
    
    func createDeveloperRewards() -> RewardsService.Rewards {
        return createRewards(
            stompCoins: TestConstants.developerRewardStompCoins,
            omCoins: TestConstants.developerRewardOMCoins,
            level: TestConstants.developerRewardLevel,
            experience: TestConstants.developerRewardExperience
        )
    }
    
    // MARK: - Localization Helpers
    
    func createLocalization(language: String = TestConstants.defaultLanguage, translations: [String: String] = [:]) -> [String: String] {
        return translations
    }
    
    func createGermanLocalization() -> [String: String] {
        return TestFixtures.shared.germanLocalization
    }
    
    func createEnglishLocalization() -> [String: String] {
        return TestFixtures.shared.englishLocalization
    }
    
    // MARK: - Error Helpers
    
    func createError(_ error: TestErrors) -> Error {
        return error
    }
    
    func createGeneralError() -> Error {
        return createError(.generalError)
    }
    
    func createTimeoutError() -> Error {
        return createError(.timeoutError)
    }
    
    func createValidationError() -> Error {
        return createError(.validationError)
    }
    
    func createNetworkError() -> Error {
        return createError(.networkError)
    }
    
    func createStorageError() -> Error {
        return createError(.storageError)
    }
    
    // MARK: - User Error Helpers
    
    func createUserNotFoundError() -> Error {
        return createError(.userNotFound)
    }
    
    func createUserAlreadyExistsError() -> Error {
        return createError(.userAlreadyExists)
    }
    
    func createInvalidUserDataError() -> Error {
        return createError(.invalidUserData)
    }
    
    func createUserNotAuthorizedError() -> Error {
        return createError(.userNotAuthorized)
    }
    
    func createUserNotPremiumError() -> Error {
        return createError(.userNotPremium)
    }
    
    func createUserNotDeveloperError() -> Error {
        return createError(.userNotDeveloper)
    }
    
    // MARK: - Challenge Error Helpers
    
    func createChallengeNotFoundError() -> Error {
        return createError(.challengeNotFound)
    }
    
    func createChallengeAlreadyExistsError() -> Error {
        return createError(.challengeAlreadyExists)
    }
    
    func createInvalidChallengeDataError() -> Error {
        return createError(.invalidChallengeData)
    }
    
    func createChallengeNotActiveError() -> Error {
        return createError(.challengeNotActive)
    }
    
    func createChallengeAlreadyCompletedError() -> Error {
        return createError(.challengeAlreadyCompleted)
    }
    
    func createChallengeAlreadyCancelledError() -> Error {
        return createError(.challengeAlreadyCancelled)
    }
    
    func createChallengeNotStartedError() -> Error {
        return createError(.challengeNotStarted)
    }
    
    func createChallengeAlreadyStartedError() -> Error {
        return createError(.challengeAlreadyStarted)
    }
    
    func createChallengeNotEndedError() -> Error {
        return createError(.challengeNotEnded)
    }
    
    func createChallengeAlreadyEndedError() -> Error {
        return createError(.challengeAlreadyEnded)
    }
    
    // MARK: - Stream Error Helpers
    
    func createStreamNotFoundError() -> Error {
        return createError(.streamNotFound)
    }
    
    func createStreamAlreadyExistsError() -> Error {
        return createError(.streamAlreadyExists)
    }
    
    func createInvalidStreamDataError() -> Error {
        return createError(.invalidStreamData)
    }
    
    func createStreamNotActiveError() -> Error {
        return createError(.streamNotActive)
    }
    
    func createStreamAlreadyActiveError() -> Error {
        return createError(.streamAlreadyActive)
    }
    
    func createStreamNotEndedError() -> Error {
        return createError(.streamNotEnded)
    }
    
    func createStreamAlreadyEndedError() -> Error {
        return createError(.streamAlreadyEnded)
    }
    
    func createStreamNotPrivateError() -> Error {
        return createError(.streamNotPrivate)
    }
    
    func createStreamAlreadyPrivateError() -> Error {
        return createError(.streamAlreadyPrivate)
    }
    
    func createStreamNotPublicError() -> Error {
        return createError(.streamNotPublic)
    }
    
    func createStreamAlreadyPublicError() -> Error {
        return createError(.streamAlreadyPublic)
    }
    
    // MARK: - Rewards Error Helpers
    
    func createRewardsNotFoundError() -> Error {
        return createError(.rewardsNotFound)
    }
    
    func createRewardsAlreadyExistsError() -> Error {
        return createError(.rewardsAlreadyExists)
    }
    
    func createInvalidRewardsDataError() -> Error {
        return createError(.invalidRewardsData)
    }
    
    func createRewardsNotCalculatedError() -> Error {
        return createError(.rewardsNotCalculated)
    }
    
    func createRewardsAlreadyCalculatedError() -> Error {
        return createError(.rewardsAlreadyCalculated)
    }
    
    func createRewardsNotDistributedError() -> Error {
        return createError(.rewardsNotDistributed)
    }
    
    func createRewardsAlreadyDistributedError() -> Error {
        return createError(.rewardsAlreadyDistributed)
    }
    
    // MARK: - Localization Error Helpers
    
    func createLocalizationNotFoundError() -> Error {
        return createError(.localizationNotFound)
    }
    
    func createLocalizationAlreadyExistsError() -> Error {
        return createError(.localizationAlreadyExists)
    }
    
    func createInvalidLocalizationDataError() -> Error {
        return createError(.invalidLocalizationData)
    }
    
    func createLanguageNotSupportedError() -> Error {
        return createError(.languageNotSupported)
    }
    
    func createLanguageAlreadySetError() -> Error {
        return createError(.languageAlreadySet)
    }
    
    // MARK: - Camera Error Helpers
    
    func createCameraNotFoundError() -> Error {
        return createError(.cameraNotFound)
    }
    
    func createCameraNotAvailableError() -> Error {
        return createError(.cameraNotAvailable)
    }
    
    func createCameraNotAuthorizedError() -> Error {
        return createError(.cameraNotAuthorized)
    }
    
    func createCameraAlreadyInUseError() -> Error {
        return createError(.cameraAlreadyInUse)
    }
    
    func createCameraNotInitializedError() -> Error {
        return createError(.cameraNotInitialized)
    }
    
    // MARK: - Network Error Helpers
    
    func createNetworkNotAvailableError() -> Error {
        return createError(.networkNotAvailable)
    }
    
    func createNetworkNotConnectedError() -> Error {
        return createError(.networkNotConnected)
    }
    
    func createNetworkNotAuthorizedError() -> Error {
        return createError(.networkNotAuthorized)
    }
    
    func createNetworkNotInitializedError() -> Error {
        return createError(.networkNotInitialized)
    }
    
    func createNetworkNotConfiguredError() -> Error {
        return createError(.networkNotConfigured)
    }
    
    // MARK: - Storage Error Helpers
    
    func createStorageNotAvailableError() -> Error {
        return createError(.storageNotAvailable)
    }
    
    func createStorageNotConnectedError() -> Error {
        return createError(.storageNotConnected)
    }
    
    func createStorageNotAuthorizedError() -> Error {
        return createError(.storageNotAuthorized)
    }
    
    func createStorageNotInitializedError() -> Error {
        return createError(.storageNotInitialized)
    }
    
    func createStorageNotConfiguredError() -> Error {
        return createError(.storageNotConfigured)
    }
    
    // MARK: - UI Error Helpers
    
    func createUINotAvailableError() -> Error {
        return createError(.uiNotAvailable)
    }
    
    func createUINotConnectedError() -> Error {
        return createError(.uiNotConnected)
    }
    
    func createUINotAuthorizedError() -> Error {
        return createError(.uiNotAuthorized)
    }
    
    func createUINotInitializedError() -> Error {
        return createError(.uiNotInitialized)
    }
    
    func createUINotConfiguredError() -> Error {
        return createError(.uiNotConfigured)
    }
} 