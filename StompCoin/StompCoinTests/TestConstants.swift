import XCTest
@testable import StompCoin

// MARK: - Test Constants

enum TestConstants {
    // MARK: - Time Constants
    
    static let defaultTimeout: TimeInterval = 5.0
    static let longTimeout: TimeInterval = 10.0
    static let shortTimeout: TimeInterval = 1.0
    
    // MARK: - User Constants
    
    static let defaultLevel = 1
    static let premiumLevel = 10
    static let developerLevel = 20
    
    static let defaultTokens = 100
    static let premiumTokens = 1000
    static let developerTokens = 2000
    
    static let defaultStompCoins = 100
    static let premiumStompCoins = 1000
    static let developerStompCoins = 2000
    
    // MARK: - Challenge Constants
    
    static let defaultRequiredTokens = 100
    static let defaultRequiredStompCoins = 200
    
    static let challengeDurationDays = 7
    static let challengeStartOffsetDays = -7
    
    // MARK: - Stream Constants
    
    static let defaultStomps = 1000
    static let premiumStomps = 2000
    static let developerStomps = 3000
    
    static let defaultEarnedStompCoins = 1000
    static let premiumEarnedStompCoins = 2000
    static let developerEarnedStompCoins = 3000
    
    static let defaultEarnedOMCoins = 1
    static let premiumEarnedOMCoins = 2
    static let developerEarnedOMCoins = 3
    
    // MARK: - Rewards Constants
    
    static let defaultRewardStompCoins = 1000
    static let premiumRewardStompCoins = 2000
    static let developerRewardStompCoins = 3000
    
    static let defaultRewardOMCoins = 10
    static let premiumRewardOMCoins = 20
    static let developerRewardOMCoins = 30
    
    static let defaultRewardLevel = 1
    static let premiumRewardLevel = 2
    static let developerRewardLevel = 3
    
    static let defaultRewardExperience = 100
    static let premiumRewardExperience = 200
    static let developerRewardExperience = 300
    
    // MARK: - Localization Constants
    
    static let defaultLanguage = "de"
    static let alternativeLanguage = "en"
    
    // MARK: - Error Constants
    
    static let defaultError = "An error occurred"
    static let networkError = "Network error occurred"
    static let validationError = "Validation error occurred"
    
    // MARK: - Test Data Constants
    
    static let defaultTestCount = 5
    static let largeTestCount = 100
    static let smallTestCount = 3
    
    // MARK: - UI Constants
    
    static let defaultButtonTitle = "Button"
    static let defaultLabelText = "Label"
    static let defaultTextFieldText = "Text"
    
    // MARK: - Network Constants
    
    static let defaultRetryCount = 3
    static let defaultRetryDelay: TimeInterval = 1.0
    
    // MARK: - Storage Constants
    
    static let defaultCacheSize = 100
    static let defaultMaxCacheAge: TimeInterval = 3600
    
    // MARK: - Camera Constants
    
    static let defaultCameraPosition = "front"
    static let alternativeCameraPosition = "back"
    
    // MARK: - Stream Quality Constants
    
    static let defaultStreamQuality = "high"
    static let alternativeStreamQuality = "low"
    
    // MARK: - Challenge Type Constants
    
    static let defaultChallengeType = ChallengeService.ChallengeType.event
    static let alternativeChallengeType = ChallengeService.ChallengeType.weekly
    
    // MARK: - Challenge Status Constants
    
    static let defaultChallengeStatus = ChallengeService.ChallengeStatus.active
    static let alternativeChallengeStatus = ChallengeService.ChallengeStatus.completed
    
    // MARK: - User Type Constants
    
    static let defaultUserType = "standard"
    static let premiumUserType = "premium"
    static let developerUserType = "developer"
    
    // MARK: - Stream Type Constants
    
    static let defaultStreamType = "public"
    static let alternativeStreamType = "private"
    
    // MARK: - Reward Type Constants
    
    static let defaultRewardType = "standard"
    static let premiumRewardType = "premium"
    static let developerRewardType = "developer"
    
    // MARK: - Localization Type Constants
    
    static let defaultLocalizationType = "system"
    static let alternativeLocalizationType = "custom"
} 