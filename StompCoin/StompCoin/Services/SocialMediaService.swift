import Foundation
import AVFoundation

class SocialMediaService: ObservableObject {
    static let shared = SocialMediaService()
    
    enum Platform: String, CaseIterable {
        case instagram = "Instagram"
        case facebook = "Facebook"
        case twitter = "Twitter"
        case tiktok = "TikTok"
        case youtube = "YouTube"
        case twitch = "Twitch"
    }
    
    struct LiveStream: Identifiable, Codable {
        let id: UUID
        let userId: UUID
        let challengeId: UUID
        let platform: Platform
        let streamUrl: String
        let startTime: Date
        var endTime: Date?
        var isActive: Bool
        var viewerCount: Int
    }
    
    @Published private(set) var activeStreams: [LiveStream] = []
    @Published private(set) var userStreams: [UUID: LiveStream] = [:]
    
    private let userDefaults = UserDefaults.standard
    private let streamsKey = "activeStreams"
    private let userStreamsKey = "userStreams"
    
    init() {
        loadStreams()
    }
    
    private func loadStreams() {
        if let data = userDefaults.data(forKey: streamsKey),
           let loadedStreams = try? JSONDecoder().decode([LiveStream].self, from: data) {
            activeStreams = loadedStreams
        }
        
        if let data = userDefaults.data(forKey: userStreamsKey),
           let loadedUserStreams = try? JSONDecoder().decode([UUID: LiveStream].self, from: data) {
            userStreams = loadedUserStreams
        }
    }
    
    private func saveStreams() {
        if let data = try? JSONEncoder().encode(activeStreams) {
            userDefaults.set(data, forKey: streamsKey)
        }
        
        if let data = try? JSONEncoder().encode(userStreams) {
            userDefaults.set(data, forKey: userStreamsKey)
        }
    }
    
    func startLiveStream(for challenge: ChallengeService.Challenge, platform: Platform) async throws {
        guard let userId = UserManager.shared.currentUser?.id else {
            throw NSError(domain: "SocialMediaService", code: 1, userInfo: [NSLocalizedDescriptionKey: "Benutzer nicht angemeldet"])
        }
        
        // Prüfen ob Benutzer an Challenge teilnimmt
        guard challenge.participants.contains(userId) else {
            throw NSError(domain: "SocialMediaService", code: 2, userInfo: [NSLocalizedDescriptionKey: "Nicht an Challenge teilgenommen"])
        }
        
        // Prüfen ob Benutzer bereits streamt
        if userStreams[userId] != nil {
            throw NSError(domain: "SocialMediaService", code: 3, userInfo: [NSLocalizedDescriptionKey: "Bereits am Streamen"])
        }
        
        // Stream-URL generieren
        let streamUrl = try await generateStreamUrl(for: platform)
        
        // LiveStream erstellen
        let stream = LiveStream(
            id: UUID(),
            userId: userId,
            challengeId: challenge.id,
            platform: platform,
            streamUrl: streamUrl,
            startTime: Date(),
            endTime: nil,
            isActive: true,
            viewerCount: 0
        )
        
        // Stream speichern
        await MainActor.run {
            activeStreams.append(stream)
            userStreams[userId] = stream
            saveStreams()
        }
    }
    
    func endLiveStream() async throws {
        guard let userId = UserManager.shared.currentUser?.id,
              let stream = userStreams[userId] else {
            throw NSError(domain: "SocialMediaService", code: 4, userInfo: [NSLocalizedDescriptionKey: "Kein aktiver Stream"])
        }
        
        // Stream beenden
        var updatedStream = stream
        updatedStream.endTime = Date()
        updatedStream.isActive = false
        
        await MainActor.run {
            if let index = activeStreams.firstIndex(where: { $0.id == stream.id }) {
                activeStreams[index] = updatedStream
            }
            userStreams[userId] = updatedStream
            saveStreams()
        }
    }
    
    func shareChallenge(_ challenge: ChallengeService.Challenge, to platforms: [Platform]) async throws {
        guard let userId = UserManager.shared.currentUser?.id else {
            throw NSError(domain: "SocialMediaService", code: 1, userInfo: [NSLocalizedDescriptionKey: "Benutzer nicht angemeldet"])
        }
        
        // Challenge-Content erstellen
        let content = try await generateChallengeContent(challenge)
        
        // Auf allen Plattformen teilen
        for platform in platforms {
            try await shareContent(content, to: platform)
        }
    }
    
    private func generateStreamUrl(for platform: Platform) async throws -> String {
        // Hier die tatsächliche API-Integration implementieren
        // Beispiel:
        return "https://stream.\(platform.rawValue.lowercased()).com/\(UUID().uuidString)"
    }
    
    private func generateChallengeContent(_ challenge: ChallengeService.Challenge) async throws -> String {
        // Hier die tatsächliche Content-Generierung implementieren
        // Beispiel:
        return """
        🎉 Neue Challenge: \(challenge.title)
        
        \(challenge.description)
        
        Start: \(challenge.startDate.formatted())
        Ende: \(challenge.endDate.formatted())
        
        #StompCoin #Challenge #\(challenge.type.rawValue)
        """
    }
    
    private func shareContent(_ content: String, to platform: Platform) async throws {
        // Hier die tatsächliche API-Integration implementieren
        // Beispiel:
        print("Teile auf \(platform.rawValue): \(content)")
    }
    
    func getActiveStreams(for challenge: ChallengeService.Challenge? = nil) -> [LiveStream] {
        if let challenge = challenge {
            return activeStreams.filter { $0.challengeId == challenge.id && $0.isActive }
        }
        return activeStreams.filter { $0.isActive }
    }
    
    func getUserStream(_ userId: UUID) -> LiveStream? {
        return userStreams[userId]
    }
} 