import XCTest
@testable import StompCoin

// MARK: - Test Fixtures

class TestFixtures {
    static let shared = TestFixtures()
    
    private init() {}
    
    // MARK: - User Fixtures
    
    let standardUser = UserManager.User(
        id: UUID(),
        username: "Standard User",
        email: "standard@example.com",
        level: 1,
        tokens: 100,
        stompCoins: 100,
        isVIPPremium: false,
        isDeveloper: false,
        vipPremiumGrantedBy: nil,
        vipPremiumGrantDate: nil,
        isDeveloperVIPPremium: false
    )
    
    let premiumUser = UserManager.User(
        id: UUID(),
        username: "Premium User",
        email: "premium@example.com",
        level: 10,
        tokens: 1000,
        stompCoins: 1000,
        isVIPPremium: true,
        isDeveloper: false,
        vipPremiumGrantedBy: nil,
        vipPremiumGrantDate: nil,
        isDeveloperVIPPremium: false
    )
    
    let developerUser = UserManager.User(
        id: UUID(),
        username: "Developer User",
        email: "developer@example.com",
        level: 20,
        tokens: 2000,
        stompCoins: 2000,
        isVIPPremium: true,
        isDeveloper: true,
        vipPremiumGrantedBy: nil,
        vipPremiumGrantDate: nil,
        isDeveloperVIPPremium: true
    )
    
    // MARK: - Challenge Fixtures
    
    let activeChallenge = ChallengeService.Challenge(
        id: UUID(),
        title: "Active Challenge",
        description: "An active challenge",
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
    
    let completedChallenge = ChallengeService.Challenge(
        id: UUID(),
        title: "Completed Challenge",
        description: "A completed challenge",
        eventId: UUID(),
        startDate: Calendar.current.date(byAdding: .day, value: -7, to: Date())!,
        endDate: Date(),
        requiredTokens: 100,
        requiredStompCoins: 200,
        participants: [],
        confirmedParticipants: [],
        status: .completed,
        type: .event
    )
    
    let cancelledChallenge = ChallengeService.Challenge(
        id: UUID(),
        title: "Cancelled Challenge",
        description: "A cancelled challenge",
        eventId: UUID(),
        startDate: Date(),
        endDate: Calendar.current.date(byAdding: .day, value: 7, to: Date())!,
        requiredTokens: 100,
        requiredStompCoins: 200,
        participants: [],
        confirmedParticipants: [],
        status: .cancelled,
        type: .event
    )
    
    // MARK: - Stream Fixtures
    
    func createActiveStream(userId: UUID, challengeId: UUID) -> VideoStreamingService.Stream {
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
    
    func createEndedStream(userId: UUID, challengeId: UUID) -> VideoStreamingService.Stream {
        return VideoStreamingService.Stream(
            id: UUID(),
            userId: userId,
            challengeId: challengeId,
            isActive: false,
            startTime: Calendar.current.date(byAdding: .hour, value: -1, to: Date())!,
            endTime: Date(),
            totalStomps: 1000,
            earnedStompCoins: 1000,
            earnedOMCoins: 1,
            isPrivate: false,
            viewers: []
        )
    }
    
    func createPrivateStream(userId: UUID, challengeId: UUID) -> VideoStreamingService.Stream {
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
            isPrivate: true,
            viewers: []
        )
    }
    
    // MARK: - Rewards Fixtures
    
    let standardRewards = RewardsService.Rewards(
        stompCoins: 1000,
        omCoins: 10,
        level: 1,
        experience: 100
    )
    
    let premiumRewards = RewardsService.Rewards(
        stompCoins: 2000,
        omCoins: 20,
        level: 2,
        experience: 200
    )
    
    let developerRewards = RewardsService.Rewards(
        stompCoins: 3000,
        omCoins: 30,
        level: 3,
        experience: 300
    )
    
    // MARK: - Localization Fixtures
    
    let germanLocalization = [
        "streaming.start": "Streaming starten",
        "streaming.end": "Streaming beenden",
        "streaming.camera.switch": "Kamera wechseln",
        "streaming.private": "Privater Stream",
        "streaming.viewers": "Zuschauer",
        "rewards.title": "Belohnungen",
        "rewards.top.streamer": "Top Streamer",
        "rewards.most.viewers": "Meiste Zuschauer",
        "settings": "Einstellungen",
        "language": "Sprache",
        "premium.features": "Premium-Features",
        "premium.vip": "VIP Premium",
        "premium.standard": "Standard",
        "challenge.title": "Challenge",
        "challenge.status": "Status",
        "join": "Beitreten",
        "stats.stomps": "Stomps",
        "stats.stompcoins": "StompCoins",
        "stats.omcoins": "OMCoins",
        "stats.progress": "Fortschritt",
        "stats.time.remaining": "Verbleibende Zeit"
    ]
    
    let englishLocalization = [
        "streaming.start": "Start Streaming",
        "streaming.end": "End Streaming",
        "streaming.camera.switch": "Switch Camera",
        "streaming.private": "Private Stream",
        "streaming.viewers": "Viewers",
        "rewards.title": "Rewards",
        "rewards.top.streamer": "Top Streamer",
        "rewards.most.viewers": "Most Viewers",
        "settings": "Settings",
        "language": "Language",
        "premium.features": "Premium Features",
        "premium.vip": "VIP Premium",
        "premium.standard": "Standard",
        "challenge.title": "Challenge",
        "challenge.status": "Status",
        "join": "Join",
        "stats.stomps": "Stomps",
        "stats.stompcoins": "StompCoins",
        "stats.omcoins": "OMCoins",
        "stats.progress": "Progress",
        "stats.time.remaining": "Time Remaining"
    ]
} 