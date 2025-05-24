import Foundation

class CacheManager {
    static let shared = CacheManager()
    
    private let fileManager = FileManager.default
    private let cacheDirectory: URL
    
    private init() {
        let urls = fileManager.urls(for: .cachesDirectory, in: .userDomainMask)
        cacheDirectory = urls[0].appendingPathComponent("StompCoin")
        
        try? fileManager.createDirectory(at: cacheDirectory, withIntermediateDirectories: true)
    }
    
    // MARK: - Data Caching
    func cacheData(_ data: Data, for key: String) throws {
        let fileURL = cacheDirectory.appendingPathComponent(key)
        try data.write(to: fileURL)
    }
    
    func getCachedData(for key: String) -> Data? {
        let fileURL = cacheDirectory.appendingPathComponent(key)
        return try? Data(contentsOf: fileURL)
    }
    
    func removeCachedData(for key: String) {
        let fileURL = cacheDirectory.appendingPathComponent(key)
        try? fileManager.removeItem(at: fileURL)
    }
    
    // MARK: - Object Caching
    func cacheObject<T: Encodable>(_ object: T, for key: String) throws {
        let data = try JSONEncoder().encode(object)
        try cacheData(data, for: key)
    }
    
    func getCachedObject<T: Decodable>(for key: String) -> T? {
        guard let data = getCachedData(for: key) else { return nil }
        return try? JSONDecoder().decode(T.self, from: data)
    }
    
    // MARK: - Cache Management
    func clearCache() {
        try? fileManager.removeItem(at: cacheDirectory)
        try? fileManager.createDirectory(at: cacheDirectory, withIntermediateDirectories: true)
    }
    
    func getCacheSize() throws -> Int64 {
        let resourceValues = try cacheDirectory.resourceValues(forKeys: [.totalFileAllocatedSizeKey])
        return Int64(resourceValues.totalFileAllocatedSize ?? 0)
    }
    
    // MARK: - Specific Cache Methods
    func cacheUserProfile(_ profile: UserProfile) throws {
        try cacheObject(profile, for: "user_profile")
    }
    
    func getCachedUserProfile() -> UserProfile? {
        return getCachedObject(for: "user_profile")
    }
    
    func cacheSteps(_ steps: [StepEntry]) throws {
        try cacheObject(steps, for: "steps_history")
    }
    
    func getCachedSteps() -> [StepEntry]? {
        return getCachedObject(for: "steps_history")
    }
    
    func cacheEvents(_ events: [Event]) throws {
        try cacheObject(events, for: "nearby_events")
    }
    
    func getCachedEvents() -> [Event]? {
        return getCachedObject(for: "nearby_events")
    }
    
    func cacheLeaderboard(_ entries: [LeaderboardEntry], for timeFrame: TimeFrame) throws {
        try cacheObject(entries, for: "leaderboard_\(timeFrame.rawValue)")
    }
    
    func getCachedLeaderboard(for timeFrame: TimeFrame) -> [LeaderboardEntry]? {
        return getCachedObject(for: "leaderboard_\(timeFrame.rawValue)")
    }
}

// MARK: - Supporting Types
struct UserProfile: Codable {
    let id: UUID
    let username: String
    let email: String
    let createdAt: Date
    let isPremium: Bool
}

struct StepEntry: Codable {
    let count: Int
    let timestamp: Date
    let type: StepType
}

enum StepType: String, Codable {
    case dance
    case fitness
    case walk
} 