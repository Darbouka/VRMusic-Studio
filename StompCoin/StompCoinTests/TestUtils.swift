import XCTest
@testable import StompCoin

// MARK: - Test Utils

class TestUtils {
    static let shared = TestUtils()
    
    private init() {}
    
    // MARK: - Time Utils
    
    func createDate(daysFromNow: Int) -> Date {
        return Calendar.current.date(byAdding: .day, value: daysFromNow, to: Date())!
    }
    
    func createDate(hoursFromNow: Int) -> Date {
        return Calendar.current.date(byAdding: .hour, value: hoursFromNow, to: Date())!
    }
    
    func createDate(minutesFromNow: Int) -> Date {
        return Calendar.current.date(byAdding: .minute, value: minutesFromNow, to: Date())!
    }
    
    func createDate(secondsFromNow: Int) -> Date {
        return Calendar.current.date(byAdding: .second, value: secondsFromNow, to: Date())!
    }
    
    func createDate(daysAgo: Int) -> Date {
        return Calendar.current.date(byAdding: .day, value: -daysAgo, to: Date())!
    }
    
    func createDate(hoursAgo: Int) -> Date {
        return Calendar.current.date(byAdding: .hour, value: -hoursAgo, to: Date())!
    }
    
    func createDate(minutesAgo: Int) -> Date {
        return Calendar.current.date(byAdding: .minute, value: -minutesAgo, to: Date())!
    }
    
    func createDate(secondsAgo: Int) -> Date {
        return Calendar.current.date(byAdding: .second, value: -secondsAgo, to: Date())!
    }
    
    // MARK: - UUID Utils
    
    func createUUID() -> UUID {
        return UUID()
    }
    
    func createUUIDs(count: Int) -> [UUID] {
        return (0..<count).map { _ in createUUID() }
    }
    
    // MARK: - String Utils
    
    func createString(length: Int) -> String {
        let letters = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789"
        return String((0..<length).map { _ in letters.randomElement()! })
    }
    
    func createStrings(count: Int, length: Int) -> [String] {
        return (0..<count).map { _ in createString(length: length) }
    }
    
    // MARK: - Int Utils
    
    func createInt(min: Int = 0, max: Int = 100) -> Int {
        return Int.random(in: min...max)
    }
    
    func createInts(count: Int, min: Int = 0, max: Int = 100) -> [Int] {
        return (0..<count).map { _ in createInt(min: min, max: max) }
    }
    
    // MARK: - Double Utils
    
    func createDouble(min: Double = 0.0, max: Double = 100.0) -> Double {
        return Double.random(in: min...max)
    }
    
    func createDoubles(count: Int, min: Double = 0.0, max: Double = 100.0) -> [Double] {
        return (0..<count).map { _ in createDouble(min: min, max: max) }
    }
    
    // MARK: - Bool Utils
    
    func createBool() -> Bool {
        return Bool.random()
    }
    
    func createBools(count: Int) -> [Bool] {
        return (0..<count).map { _ in createBool() }
    }
    
    // MARK: - Array Utils
    
    func createArray<T>(count: Int, element: T) -> [T] {
        return Array(repeating: element, count: count)
    }
    
    func createArray<T>(count: Int, generator: () -> T) -> [T] {
        return (0..<count).map { _ in generator() }
    }
    
    // MARK: - Dictionary Utils
    
    func createDictionary<K, V>(count: Int, keyGenerator: () -> K, valueGenerator: () -> V) -> [K: V] {
        var dictionary: [K: V] = [:]
        for _ in 0..<count {
            dictionary[keyGenerator()] = valueGenerator()
        }
        return dictionary
    }
    
    // MARK: - Set Utils
    
    func createSet<T>(count: Int, element: T) -> Set<T> {
        return Set(createArray(count: count, element: element))
    }
    
    func createSet<T>(count: Int, generator: () -> T) -> Set<T> {
        return Set(createArray(count: count, generator: generator))
    }
    
    // MARK: - Optional Utils
    
    func createOptional<T>(_ value: T) -> T? {
        return createBool() ? value : nil
    }
    
    func createOptionals<T>(count: Int, value: T) -> [T?] {
        return (0..<count).map { _ in createOptional(value) }
    }
    
    // MARK: - Result Utils
    
    func createResult<T>(_ value: T) -> Result<T, Error> {
        return createBool() ? .success(value) : .failure(TestErrors.generalError)
    }
    
    func createResults<T>(count: Int, value: T) -> [Result<T, Error>] {
        return (0..<count).map { _ in createResult(value) }
    }
    
    // MARK: - Error Utils
    
    func createError() -> Error {
        return TestErrors.generalError
    }
    
    func createErrors(count: Int) -> [Error] {
        return (0..<count).map { _ in createError() }
    }
    
    // MARK: - User Utils
    
    func createUser() -> UserManager.User {
        return TestHelpers.shared.createUser()
    }
    
    func createUsers(count: Int) -> [UserManager.User] {
        return (0..<count).map { _ in createUser() }
    }
    
    // MARK: - Challenge Utils
    
    func createChallenge() -> ChallengeService.Challenge {
        return TestHelpers.shared.createChallenge()
    }
    
    func createChallenges(count: Int) -> [ChallengeService.Challenge] {
        return (0..<count).map { _ in createChallenge() }
    }
    
    // MARK: - Stream Utils
    
    func createStream() -> VideoStreamingService.Stream {
        return TestHelpers.shared.createStream()
    }
    
    func createStreams(count: Int) -> [VideoStreamingService.Stream] {
        return (0..<count).map { _ in createStream() }
    }
    
    // MARK: - Rewards Utils
    
    func createRewards() -> RewardsService.Rewards {
        return TestHelpers.shared.createRewards()
    }
    
    func createRewards(count: Int) -> [RewardsService.Rewards] {
        return (0..<count).map { _ in createRewards() }
    }
    
    // MARK: - Localization Utils
    
    func createLocalization() -> [String: String] {
        return TestHelpers.shared.createLocalization()
    }
    
    func createLocalizations(count: Int) -> [[String: String]] {
        return (0..<count).map { _ in createLocalization() }
    }
    
    // MARK: - Camera Utils
    
    func createCamera() -> CameraService {
        return CameraService()
    }
    
    func createCameras(count: Int) -> [CameraService] {
        return (0..<count).map { _ in createCamera() }
    }
    
    // MARK: - Network Utils
    
    func createNetwork() -> NetworkService {
        return NetworkService()
    }
    
    func createNetworks(count: Int) -> [NetworkService] {
        return (0..<count).map { _ in createNetwork() }
    }
    
    // MARK: - Storage Utils
    
    func createStorage() -> StorageService {
        return StorageService()
    }
    
    func createStorages(count: Int) -> [StorageService] {
        return (0..<count).map { _ in createStorage() }
    }
    
    // MARK: - UI Utils
    
    func createUI() -> UIService {
        return UIService()
    }
    
    func createUIs(count: Int) -> [UIService] {
        return (0..<count).map { _ in createUI() }
    }
} 