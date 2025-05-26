import XCTest
@testable import StompCoin

// MARK: - Test Mocks

class TestMocks {
    static let shared = TestMocks()
    
    private init() {}
    
    // MARK: - VideoStreamingService Mock
    
    class MockVideoStreamingService: VideoStreamingService {
        var streams: [UUID: Stream] = [:]
        var isUsingFrontCamera: Bool = true
        var shouldThrowError: Bool = false
        var error: Error = VideoStreamingServiceError.streamNotFound
        
        override func startVideoStream(for challenge: ChallengeService.Challenge) async throws {
            if shouldThrowError {
                throw error
            }
            
            let stream = Stream(
                id: UUID(),
                userId: UUID(),
                challengeId: challenge.id,
                isActive: true,
                startTime: Date(),
                endTime: nil,
                totalStomps: 0,
                earnedStompCoins: 0,
                earnedOMCoins: 0,
                isPrivate: false,
                viewers: []
            )
            
            streams[stream.id] = stream
        }
        
        override func endVideoStream() async throws {
            if shouldThrowError {
                throw error
            }
            
            streams.values.forEach { stream in
                var updatedStream = stream
                updatedStream.isActive = false
                updatedStream.endTime = Date()
                streams[stream.id] = updatedStream
            }
        }
        
        override func addStomps(_ count: Int, for userId: UUID) async throws {
            if shouldThrowError {
                throw error
            }
            
            streams.values.forEach { stream in
                var updatedStream = stream
                updatedStream.totalStomps += count
                updatedStream.earnedStompCoins += count
                updatedStream.earnedOMCoins += count / 1000
                streams[stream.id] = updatedStream
            }
        }
        
        override func switchCamera() async throws {
            if shouldThrowError {
                throw error
            }
            
            isUsingFrontCamera.toggle()
        }
        
        override func getUserStream(_ userId: UUID) -> Stream? {
            return streams.values.first { $0.userId == userId }
        }
        
        override func getViewableStreams() -> [Stream] {
            return streams.values.filter { !$0.isPrivate }
        }
        
        override func getTopStreamer(for challenge: ChallengeService.Challenge) -> Stream? {
            return streams.values
                .filter { $0.challengeId == challenge.id }
                .max { $0.totalStomps < $1.totalStomps }
        }
    }
    
    // MARK: - UserManager Mock
    
    class MockUserManager: UserManager {
        var users: [UUID: User] = [:]
        var shouldThrowError: Bool = false
        var error: Error = UserManagerError.userNotFound
        
        override func createUser(_ user: User) async throws {
            if shouldThrowError {
                throw error
            }
            
            users[user.id] = user
        }
        
        override func getUser(id: UUID) async throws -> User {
            if shouldThrowError {
                throw error
            }
            
            guard let user = users[id] else {
                throw UserManagerError.userNotFound
            }
            
            return user
        }
        
        override func updateUser(_ user: User) async throws {
            if shouldThrowError {
                throw error
            }
            
            users[user.id] = user
        }
        
        override func deleteUser(id: UUID) async throws {
            if shouldThrowError {
                throw error
            }
            
            users.removeValue(forKey: id)
        }
        
        override func getCurrentUser() -> User? {
            return users.values.first
        }
    }
    
    // MARK: - ChallengeService Mock
    
    class MockChallengeService: ChallengeService {
        var challenges: [UUID: Challenge] = [:]
        var shouldThrowError: Bool = false
        var error: Error = ChallengeServiceError.challengeNotFound
        
        override func createChallenge(_ challenge: Challenge) async throws {
            if shouldThrowError {
                throw error
            }
            
            challenges[challenge.id] = challenge
        }
        
        override func getChallenge(id: UUID) async throws -> Challenge {
            if shouldThrowError {
                throw error
            }
            
            guard let challenge = challenges[id] else {
                throw ChallengeServiceError.challengeNotFound
            }
            
            return challenge
        }
        
        override func joinChallenge(_ challenge: Challenge, user: UserManager.User) async throws {
            if shouldThrowError {
                throw error
            }
            
            var updatedChallenge = challenge
            updatedChallenge.participants.append(user)
            challenges[challenge.id] = updatedChallenge
        }
        
        override func leaveChallenge(_ challenge: Challenge, user: UserManager.User) async throws {
            if shouldThrowError {
                throw error
            }
            
            var updatedChallenge = challenge
            updatedChallenge.participants.removeAll { $0.id == user.id }
            challenges[challenge.id] = updatedChallenge
        }
        
        override func getUserChallenges() -> [Challenge] {
            return Array(challenges.values)
        }
    }
    
    // MARK: - RewardsService Mock
    
    class MockRewardsService: RewardsService {
        var shouldThrowError: Bool = false
        var error: Error = RewardsServiceError.rewardsNotFound
        var rewards: Rewards = Rewards(stompCoins: 1000, omCoins: 10, level: 1, experience: 100)
        
        override func calculateRewards(for userId: UUID, in challenge: ChallengeService.Challenge) async throws -> Rewards {
            if shouldThrowError {
                throw error
            }
            
            return rewards
        }
    }
    
    // MARK: - LocalizationService Mock
    
    class MockLocalizationService: LocalizationService {
        var currentLanguage: String = "de"
        var localizedStrings: [String: [String: String]] = [
            "de": [
                "streaming.start": "Streaming starten",
                "streaming.end": "Streaming beenden"
            ],
            "en": [
                "streaming.start": "Start Streaming",
                "streaming.end": "End Streaming"
            ]
        ]
        
        override func setLanguage(_ language: String) {
            currentLanguage = language
        }
        
        override func localizedString(_ key: String) -> String {
            return localizedStrings[currentLanguage]?[key] ?? key
        }
    }
}

// MARK: - User Manager Mock

class UserManagerMock: UserManager {
    var users: [UUID: User] = [:]
    var currentUser: User?
    var error: Error?
    
    override func getUser(id: UUID) throws -> User {
        if let error = error {
            throw error
        }
        guard let user = users[id] else {
            throw TestErrors.userNotFound
        }
        return user
    }
    
    override func createUser(username: String, email: String) throws -> User {
        if let error = error {
            throw error
        }
        let user = User(
            id: UUID(),
            username: username,
            email: email,
            level: TestConstants.defaultLevel,
            tokens: TestConstants.defaultTokens,
            stompCoins: TestConstants.defaultStompCoins,
            isVIPPremium: false,
            isDeveloper: false,
            vipPremiumGrantedBy: nil,
            vipPremiumGrantDate: nil,
            isDeveloperVIPPremium: false
        )
        users[user.id] = user
        return user
    }
    
    override func updateUser(_ user: User) throws {
        if let error = error {
            throw error
        }
        users[user.id] = user
    }
    
    override func deleteUser(id: UUID) throws {
        if let error = error {
            throw error
        }
        guard users.removeValue(forKey: id) != nil else {
            throw TestErrors.userNotFound
        }
    }
    
    override func getCurrentUser() throws -> User {
        if let error = error {
            throw error
        }
        guard let user = currentUser else {
            throw TestErrors.userNotFound
        }
        return user
    }
    
    override func setCurrentUser(_ user: User) throws {
        if let error = error {
            throw error
        }
        currentUser = user
    }
    
    override func clearCurrentUser() throws {
        if let error = error {
            throw error
        }
        currentUser = nil
    }
}

// MARK: - Challenge Service Mock

class ChallengeServiceMock: ChallengeService {
    var challenges: [UUID: Challenge] = [:]
    var error: Error?
    
    override func getChallenge(id: UUID) throws -> Challenge {
        if let error = error {
            throw error
        }
        guard let challenge = challenges[id] else {
            throw TestErrors.challengeNotFound
        }
        return challenge
    }
    
    override func createChallenge(title: String, description: String, eventId: UUID, startDate: Date, endDate: Date, requiredTokens: Int, requiredStompCoins: Int) throws -> Challenge {
        if let error = error {
            throw error
        }
        let challenge = Challenge(
            id: UUID(),
            title: title,
            description: description,
            eventId: eventId,
            startDate: startDate,
            endDate: endDate,
            requiredTokens: requiredTokens,
            requiredStompCoins: requiredStompCoins,
            participants: [],
            confirmedParticipants: [],
            status: .active,
            type: .event
        )
        challenges[challenge.id] = challenge
        return challenge
    }
    
    override func updateChallenge(_ challenge: Challenge) throws {
        if let error = error {
            throw error
        }
        challenges[challenge.id] = challenge
    }
    
    override func deleteChallenge(id: UUID) throws {
        if let error = error {
            throw error
        }
        guard challenges.removeValue(forKey: id) != nil else {
            throw TestErrors.challengeNotFound
        }
    }
    
    override func joinChallenge(id: UUID, userId: UUID) throws {
        if let error = error {
            throw error
        }
        guard var challenge = challenges[id] else {
            throw TestErrors.challengeNotFound
        }
        challenge.participants.append(userId)
        challenges[challenge.id] = challenge
    }
    
    override func leaveChallenge(id: UUID, userId: UUID) throws {
        if let error = error {
            throw error
        }
        guard var challenge = challenges[id] else {
            throw TestErrors.challengeNotFound
        }
        challenge.participants.removeAll { $0 == userId }
        challenges[challenge.id] = challenge
    }
    
    override func confirmParticipation(id: UUID, userId: UUID) throws {
        if let error = error {
            throw error
        }
        guard var challenge = challenges[id] else {
            throw TestErrors.challengeNotFound
        }
        challenge.confirmedParticipants.append(userId)
        challenges[challenge.id] = challenge
    }
    
    override func cancelParticipation(id: UUID, userId: UUID) throws {
        if let error = error {
            throw error
        }
        guard var challenge = challenges[id] else {
            throw TestErrors.challengeNotFound
        }
        challenge.confirmedParticipants.removeAll { $0 == userId }
        challenges[challenge.id] = challenge
    }
    
    override func startChallenge(id: UUID) throws {
        if let error = error {
            throw error
        }
        guard var challenge = challenges[id] else {
            throw TestErrors.challengeNotFound
        }
        challenge.status = .active
        challenges[challenge.id] = challenge
    }
    
    override func endChallenge(id: UUID) throws {
        if let error = error {
            throw error
        }
        guard var challenge = challenges[id] else {
            throw TestErrors.challengeNotFound
        }
        challenge.status = .completed
        challenges[challenge.id] = challenge
    }
    
    override func cancelChallenge(id: UUID) throws {
        if let error = error {
            throw error
        }
        guard var challenge = challenges[id] else {
            throw TestErrors.challengeNotFound
        }
        challenge.status = .cancelled
        challenges[challenge.id] = challenge
    }
}

// MARK: - Video Streaming Service Mock

class VideoStreamingServiceMock: VideoStreamingService {
    var streams: [UUID: Stream] = [:]
    var error: Error?
    
    override func getStream(id: UUID) throws -> Stream {
        if let error = error {
            throw error
        }
        guard let stream = streams[id] else {
            throw TestErrors.streamNotFound
        }
        return stream
    }
    
    override func createStream(userId: UUID, challengeId: UUID) throws -> Stream {
        if let error = error {
            throw error
        }
        let stream = Stream(
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
        streams[stream.id] = stream
        return stream
    }
    
    override func updateStream(_ stream: Stream) throws {
        if let error = error {
            throw error
        }
        streams[stream.id] = stream
    }
    
    override func deleteStream(id: UUID) throws {
        if let error = error {
            throw error
        }
        guard streams.removeValue(forKey: id) != nil else {
            throw TestErrors.streamNotFound
        }
    }
    
    override func startStream(id: UUID) throws {
        if let error = error {
            throw error
        }
        guard var stream = streams[id] else {
            throw TestErrors.streamNotFound
        }
        stream.isActive = true
        stream.startTime = Date()
        streams[stream.id] = stream
    }
    
    override func endStream(id: UUID) throws {
        if let error = error {
            throw error
        }
        guard var stream = streams[id] else {
            throw TestErrors.streamNotFound
        }
        stream.isActive = false
        stream.endTime = Date()
        streams[stream.id] = stream
    }
    
    override func makeStreamPrivate(id: UUID) throws {
        if let error = error {
            throw error
        }
        guard var stream = streams[id] else {
            throw TestErrors.streamNotFound
        }
        stream.isPrivate = true
        streams[stream.id] = stream
    }
    
    override func makeStreamPublic(id: UUID) throws {
        if let error = error {
            throw error
        }
        guard var stream = streams[id] else {
            throw TestErrors.streamNotFound
        }
        stream.isPrivate = false
        streams[stream.id] = stream
    }
    
    override func addViewer(id: UUID, userId: UUID) throws {
        if let error = error {
            throw error
        }
        guard var stream = streams[id] else {
            throw TestErrors.streamNotFound
        }
        stream.viewers.append(userId)
        streams[stream.id] = stream
    }
    
    override func removeViewer(id: UUID, userId: UUID) throws {
        if let error = error {
            throw error
        }
        guard var stream = streams[id] else {
            throw TestErrors.streamNotFound
        }
        stream.viewers.removeAll { $0 == userId }
        streams[stream.id] = stream
    }
    
    override func addStomp(id: UUID) throws {
        if let error = error {
            throw error
        }
        guard var stream = streams[id] else {
            throw TestErrors.streamNotFound
        }
        stream.totalStomps += 1
        streams[stream.id] = stream
    }
    
    override func calculateRewards(id: UUID) throws -> RewardsService.Rewards {
        if let error = error {
            throw error
        }
        guard let stream = streams[id] else {
            throw TestErrors.streamNotFound
        }
        return RewardsService.Rewards(
            stompCoins: stream.totalStomps,
            omCoins: stream.totalStomps / 1000,
            level: 1,
            experience: stream.totalStomps / 10
        )
    }
}

// MARK: - Rewards Service Mock

class RewardsServiceMock: RewardsService {
    var rewards: [UUID: Rewards] = [:]
    var error: Error?
    
    override func getRewards(id: UUID) throws -> Rewards {
        if let error = error {
            throw error
        }
        guard let rewards = rewards[id] else {
            throw TestErrors.rewardsNotFound
        }
        return rewards
    }
    
    override func createRewards(stompCoins: Int, omCoins: Int, level: Int, experience: Int) throws -> Rewards {
        if let error = error {
            throw error
        }
        let rewards = Rewards(
            stompCoins: stompCoins,
            omCoins: omCoins,
            level: level,
            experience: experience
        )
        self.rewards[UUID()] = rewards
        return rewards
    }
    
    override func updateRewards(_ rewards: Rewards) throws {
        if let error = error {
            throw error
        }
        self.rewards[UUID()] = rewards
    }
    
    override func deleteRewards(id: UUID) throws {
        if let error = error {
            throw error
        }
        guard rewards.removeValue(forKey: id) != nil else {
            throw TestErrors.rewardsNotFound
        }
    }
    
    override func calculateRewards(stomps: Int, isPremium: Bool, isDeveloper: Bool) throws -> Rewards {
        if let error = error {
            throw error
        }
        let multiplier = isDeveloper ? 3 : (isPremium ? 2 : 1)
        return Rewards(
            stompCoins: stomps * multiplier,
            omCoins: (stomps / 1000) * multiplier,
            level: 1,
            experience: (stomps / 10) * multiplier
        )
    }
    
    override func distributeRewards(_ rewards: Rewards, userId: UUID) throws {
        if let error = error {
            throw error
        }
        self.rewards[userId] = rewards
    }
}

// MARK: - Localization Service Mock

class LocalizationServiceMock: LocalizationService {
    var localizations: [String: [String: String]] = [:]
    var currentLanguage: String = TestConstants.defaultLanguage
    var error: Error?
    
    override func getLocalization(language: String) throws -> [String: String] {
        if let error = error {
            throw error
        }
        guard let localization = localizations[language] else {
            throw TestErrors.localizationNotFound
        }
        return localization
    }
    
    override func createLocalization(language: String, translations: [String: String]) throws {
        if let error = error {
            throw error
        }
        localizations[language] = translations
    }
    
    override func updateLocalization(language: String, translations: [String: String]) throws {
        if let error = error {
            throw error
        }
        guard localizations[language] != nil else {
            throw TestErrors.localizationNotFound
        }
        localizations[language] = translations
    }
    
    override func deleteLocalization(language: String) throws {
        if let error = error {
            throw error
        }
        guard localizations.removeValue(forKey: language) != nil else {
            throw TestErrors.localizationNotFound
        }
    }
    
    override func getCurrentLanguage() throws -> String {
        if let error = error {
            throw error
        }
        return currentLanguage
    }
    
    override func setCurrentLanguage(_ language: String) throws {
        if let error = error {
            throw error
        }
        guard localizations[language] != nil else {
            throw TestErrors.languageNotSupported
        }
        currentLanguage = language
    }
    
    override func getTranslation(key: String) throws -> String {
        if let error = error {
            throw error
        }
        guard let localization = localizations[currentLanguage] else {
            throw TestErrors.localizationNotFound
        }
        guard let translation = localization[key] else {
            throw TestErrors.localizationNotFound
        }
        return translation
    }
}

// MARK: - Camera Service Mock

class CameraServiceMock: CameraService {
    var isAvailable: Bool = true
    var isAuthorized: Bool = true
    var isInitialized: Bool = true
    var isInUse: Bool = false
    var error: Error?
    
    override func checkAvailability() throws -> Bool {
        if let error = error {
            throw error
        }
        return isAvailable
    }
    
    override func requestAuthorization() throws -> Bool {
        if let error = error {
            throw error
        }
        return isAuthorized
    }
    
    override func initialize() throws {
        if let error = error {
            throw error
        }
        isInitialized = true
    }
    
    override func startCapture() throws {
        if let error = error {
            throw error
        }
        guard isInitialized else {
            throw TestErrors.cameraNotInitialized
        }
        guard !isInUse else {
            throw TestErrors.cameraAlreadyInUse
        }
        isInUse = true
    }
    
    override func stopCapture() throws {
        if let error = error {
            throw error
        }
        guard isInitialized else {
            throw TestErrors.cameraNotInitialized
        }
        guard isInUse else {
            throw TestErrors.cameraNotInUse
        }
        isInUse = false
    }
    
    override func switchCamera() throws {
        if let error = error {
            throw error
        }
        guard isInitialized else {
            throw TestErrors.cameraNotInitialized
        }
    }
}

// MARK: - Network Service Mock

class NetworkServiceMock: NetworkService {
    var isAvailable: Bool = true
    var isConnected: Bool = true
    var isAuthorized: Bool = true
    var isInitialized: Bool = true
    var isConfigured: Bool = true
    var error: Error?
    
    override func checkAvailability() throws -> Bool {
        if let error = error {
            throw error
        }
        return isAvailable
    }
    
    override func checkConnection() throws -> Bool {
        if let error = error {
            throw error
        }
        return isConnected
    }
    
    override func requestAuthorization() throws -> Bool {
        if let error = error {
            throw error
        }
        return isAuthorized
    }
    
    override func initialize() throws {
        if let error = error {
            throw error
        }
        isInitialized = true
    }
    
    override func configure() throws {
        if let error = error {
            throw error
        }
        isConfigured = true
    }
}

// MARK: - Storage Service Mock

class StorageServiceMock: StorageService {
    var isAvailable: Bool = true
    var isConnected: Bool = true
    var isAuthorized: Bool = true
    var isInitialized: Bool = true
    var isConfigured: Bool = true
    var error: Error?
    
    override func checkAvailability() throws -> Bool {
        if let error = error {
            throw error
        }
        return isAvailable
    }
    
    override func checkConnection() throws -> Bool {
        if let error = error {
            throw error
        }
        return isConnected
    }
    
    override func requestAuthorization() throws -> Bool {
        if let error = error {
            throw error
        }
        return isAuthorized
    }
    
    override func initialize() throws {
        if let error = error {
            throw error
        }
        isInitialized = true
    }
    
    override func configure() throws {
        if let error = error {
            throw error
        }
        isConfigured = true
    }
}

// MARK: - UI Service Mock

class UIServiceMock: UIService {
    var isAvailable: Bool = true
    var isConnected: Bool = true
    var isAuthorized: Bool = true
    var isInitialized: Bool = true
    var isConfigured: Bool = true
    var error: Error?
    
    override func checkAvailability() throws -> Bool {
        if let error = error {
            throw error
        }
        return isAvailable
    }
    
    override func checkConnection() throws -> Bool {
        if let error = error {
            throw error
        }
        return isConnected
    }
    
    override func requestAuthorization() throws -> Bool {
        if let error = error {
            throw error
        }
        return isAuthorized
    }
    
    override func initialize() throws {
        if let error = error {
            throw error
        }
        isInitialized = true
    }
    
    override func configure() throws {
        if let error = error {
            throw error
        }
        isConfigured = true
    }
} 