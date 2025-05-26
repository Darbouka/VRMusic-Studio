import Foundation
import AVFoundation
import UIKit

class VideoStreamingService: ObservableObject {
    static let shared = VideoStreamingService()
    
    struct VideoStream: Identifiable, Codable {
        let id: UUID
        let userId: UUID
        let challengeId: UUID
        let streamUrl: String
        let startTime: Date
        var endTime: Date?
        var isActive: Bool
        var viewerCount: Int
        var isPrivate: Bool
        var allowedViewers: [UUID]
        var totalStomps: Int
        var earnedStompCoins: Int
        var earnedOMCoins: Int
        var isUsingFrontCamera: Bool
        var lastOMCoinEarned: Date?
    }
    
    @Published private(set) var activeStreams: [VideoStream] = []
    @Published private(set) var userStreams: [UUID: VideoStream] = [:]
    @Published private(set) var previewLayer: AVCaptureVideoPreviewLayer?
    @Published private(set) var isUsingFrontCamera = true
    
    private let captureSession = AVCaptureSession()
    private var videoOutput: AVCaptureMovieFileOutput?
    private var currentVideoInput: AVCaptureDeviceInput?
    private let userDefaults = UserDefaults.standard
    private let streamsKey = "activeVideoStreams"
    private let userStreamsKey = "userVideoStreams"
    
    // OM Coin Konstanten
    private let stompsPerOMCoin = 100_000
    private let stompCoinPerStomp = 1
    private let omCoinTimeWindow: TimeInterval = 24 * 60 * 60 // 24 Stunden in Sekunden
    
    init() {
        loadStreams()
        setupCaptureSession()
    }
    
    private func setupCaptureSession() {
        captureSession.beginConfiguration()
        
        // Standardmäßig Frontkamera verwenden
        guard let videoDevice = AVCaptureDevice.default(.builtInWideAngleCamera, for: .video, position: .front),
              let videoInput = try? AVCaptureDeviceInput(device: videoDevice),
              captureSession.canAddInput(videoInput) else {
            return
        }
        
        captureSession.addInput(videoInput)
        currentVideoInput = videoInput
        
        let videoOutput = AVCaptureMovieFileOutput()
        if captureSession.canAddOutput(videoOutput) {
            captureSession.addOutput(videoOutput)
            self.videoOutput = videoOutput
        }
        
        captureSession.commitConfiguration()
        
        let previewLayer = AVCaptureVideoPreviewLayer(session: captureSession)
        previewLayer.videoGravity = .resizeAspectFill
        self.previewLayer = previewLayer
    }
    
    private func loadStreams() {
        if let data = userDefaults.data(forKey: streamsKey),
           let loadedStreams = try? JSONDecoder().decode([VideoStream].self, from: data) {
            activeStreams = loadedStreams
        }
        
        if let data = userDefaults.data(forKey: userStreamsKey),
           let loadedUserStreams = try? JSONDecoder().decode([UUID: VideoStream].self, from: data) {
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
    
    func switchCamera() async throws {
        guard let currentInput = currentVideoInput else { return }
        
        // Neue Kamera-Position bestimmen
        let newPosition: AVCaptureDevice.Position = isUsingFrontCamera ? .back : .front
        
        // Neue Kamera konfigurieren
        guard let newVideoDevice = AVCaptureDevice.default(.builtInWideAngleCamera, for: .video, position: newPosition),
              let newVideoInput = try? AVCaptureDeviceInput(device: newVideoDevice) else {
            return
        }
        
        // Kamera wechseln
        captureSession.beginConfiguration()
        captureSession.removeInput(currentInput)
        
        if captureSession.canAddInput(newVideoInput) {
            captureSession.addInput(newVideoInput)
            currentVideoInput = newVideoInput
            isUsingFrontCamera = !isUsingFrontCamera
            
            // Stream aktualisieren
            if let userId = UserManager.shared.currentUser?.id,
               var stream = userStreams[userId] {
                stream.isUsingFrontCamera = isUsingFrontCamera
                await MainActor.run {
                    if let index = activeStreams.firstIndex(where: { $0.id == stream.id }) {
                        activeStreams[index] = stream
                    }
                    userStreams[userId] = stream
                    saveStreams()
                }
            }
        }
        
        captureSession.commitConfiguration()
    }
    
    func addStomps(_ count: Int, for userId: UUID) async {
        guard var stream = userStreams[userId],
              let user = UserManager.shared.currentUser else { return }
        
        // Stomps und StompCoins aktualisieren
        stream.totalStomps += count
        stream.earnedStompCoins += count * stompCoinPerStomp
        
        // OM Coins berechnen
        let newOMCoins = calculateOMCoins(for: stream, user: user)
        if newOMCoins > stream.earnedOMCoins {
            stream.earnedOMCoins = newOMCoins
            stream.lastOMCoinEarned = Date()
        }
        
        await MainActor.run {
            if let index = activeStreams.firstIndex(where: { $0.id == stream.id }) {
                activeStreams[index] = stream
            }
            userStreams[userId] = stream
            saveStreams()
        }
    }
    
    private func calculateOMCoins(for stream: VideoStream, user: UserManager.User) -> Int {
        // Premium und VIP Premium Benutzer bekommen OM Coins direkt
        if user.isVIPPremium || user.isDeveloperVIPPremium {
            return stream.totalStomps / stompsPerOMCoin
        }
        
        // Freemium Benutzer müssen 100.000 StompCoins innerhalb von 24 Stunden erreichen
        if let lastEarned = stream.lastOMCoinEarned {
            let timeSinceLastEarned = Date().timeIntervalSince(lastEarned)
            
            // Wenn innerhalb des 24-Stunden-Fensters
            if timeSinceLastEarned <= omCoinTimeWindow {
                // Prüfen ob genug StompCoins verdient wurden
                if stream.earnedStompCoins >= stompsPerOMCoin {
                    return stream.earnedOMCoins + 1
                }
            } else {
                // Zeitfenster abgelaufen, neue Berechnung starten
                if stream.earnedStompCoins >= stompsPerOMCoin {
                    return 1
                }
            }
        } else {
            // Erster OM Coin
            if stream.earnedStompCoins >= stompsPerOMCoin {
                return 1
            }
        }
        
        return stream.earnedOMCoins
    }
    
    func getOMCoinProgress(for userId: UUID) -> (earned: Int, progress: Double, timeRemaining: TimeInterval?) {
        guard let stream = userStreams[userId],
              let user = UserManager.shared.currentUser else {
            return (0, 0, nil)
        }
        
        // Premium und VIP Premium Benutzer haben keinen Fortschritt
        if user.isVIPPremium || user.isDeveloperVIPPremium {
            return (stream.earnedOMCoins, 1.0, nil)
        }
        
        // Freemium Benutzer Fortschritt
        if let lastEarned = stream.lastOMCoinEarned {
            let timeSinceLastEarned = Date().timeIntervalSince(lastEarned)
            let timeRemaining = max(0, omCoinTimeWindow - timeSinceLastEarned)
            
            let progress = min(1.0, Double(stream.earnedStompCoins) / Double(stompsPerOMCoin))
            
            return (stream.earnedOMCoins, progress, timeRemaining)
        }
        
        let progress = min(1.0, Double(stream.earnedStompCoins) / Double(stompsPerOMCoin))
        return (stream.earnedOMCoins, progress, omCoinTimeWindow)
    }
    
    func updateViewerCount(_ count: Int, for userId: UUID) async {
        guard var stream = userStreams[userId] else { return }
        stream.viewerCount = count
        
        await MainActor.run {
            if let index = activeStreams.firstIndex(where: { $0.id == stream.id }) {
                activeStreams[index] = stream
            }
            userStreams[userId] = stream
            saveStreams()
        }
    }
    
    func getTopStreamer(for challenge: ChallengeService.Challenge) -> VideoStream? {
        return activeStreams
            .filter { $0.challengeId == challenge.id }
            .max { $0.totalStomps < $1.totalStomps }
    }
    
    func getMostViewedStreamer(for challenge: ChallengeService.Challenge) -> VideoStream? {
        return activeStreams
            .filter { $0.challengeId == challenge.id }
            .max { $0.viewerCount < $1.viewerCount }
    }
    
    func startVideoStream(for challenge: ChallengeService.Challenge, isPrivate: Bool = false) async throws {
        guard let userId = UserManager.shared.currentUser?.id else {
            throw NSError(domain: "VideoStreamingService", code: 1, userInfo: [NSLocalizedDescriptionKey: "Benutzer nicht angemeldet"])
        }
        
        // Prüfen ob Benutzer an Challenge teilnimmt
        guard challenge.participants.contains(userId) else {
            throw NSError(domain: "VideoStreamingService", code: 2, userInfo: [NSLocalizedDescriptionKey: "Nicht an Challenge teilgenommen"])
        }
        
        // Prüfen ob Benutzer bereits streamt
        if userStreams[userId] != nil {
            throw NSError(domain: "VideoStreamingService", code: 3, userInfo: [NSLocalizedDescriptionKey: "Bereits am Streamen"])
        }
        
        // Stream-URL generieren
        let streamUrl = try await generateStreamUrl()
        
        // VideoStream erstellen
        let stream = VideoStream(
            id: UUID(),
            userId: userId,
            challengeId: challenge.id,
            streamUrl: streamUrl,
            startTime: Date(),
            endTime: nil,
            isActive: true,
            viewerCount: 0,
            isPrivate: isPrivate,
            allowedViewers: [],
            totalStomps: 0,
            earnedStompCoins: 0,
            earnedOMCoins: 0,
            isUsingFrontCamera: isUsingFrontCamera,
            lastOMCoinEarned: nil
        )
        
        // Stream speichern
        await MainActor.run {
            activeStreams.append(stream)
            userStreams[userId] = stream
            saveStreams()
        }
        
        // Kamera starten
        if !captureSession.isRunning {
            DispatchQueue.global(qos: .userInitiated).async { [weak self] in
                self?.captureSession.startRunning()
            }
        }
    }
    
    func endVideoStream() async throws {
        guard let userId = UserManager.shared.currentUser?.id,
              let stream = userStreams[userId] else {
            throw NSError(domain: "VideoStreamingService", code: 4, userInfo: [NSLocalizedDescriptionKey: "Kein aktiver Stream"])
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
        
        // Kamera stoppen
        if captureSession.isRunning {
            captureSession.stopRunning()
        }
    }
    
    func allowViewer(_ viewerId: UUID, for streamId: UUID) async throws {
        guard let userId = UserManager.shared.currentUser?.id,
              var stream = userStreams[userId],
              stream.id == streamId,
              stream.isPrivate else {
            throw NSError(domain: "VideoStreamingService", code: 5, userInfo: [NSLocalizedDescriptionKey: "Ungültiger Stream"])
        }
        
        // Viewer hinzufügen
        stream.allowedViewers.append(viewerId)
        
        await MainActor.run {
            if let index = activeStreams.firstIndex(where: { $0.id == streamId }) {
                activeStreams[index] = stream
            }
            userStreams[userId] = stream
            saveStreams()
        }
    }
    
    func canViewStream(_ streamId: UUID) -> Bool {
        guard let userId = UserManager.shared.currentUser?.id,
              let user = UserManager.shared.currentUser else {
            return false
        }
        
        // Premium und VIP Premium Benutzer können immer zuschauen
        if user.isVIPPremium || user.isDeveloperVIPPremium {
            return true
        }
        
        // Prüfen ob Benutzer für privaten Stream zugelassen ist
        if let stream = activeStreams.first(where: { $0.id == streamId }),
           stream.isPrivate {
            return stream.allowedViewers.contains(userId)
        }
        
        return false
    }
    
    private func generateStreamUrl() async throws -> String {
        // Hier die tatsächliche Stream-URL-Generierung implementieren
        return "https://stream.stompcoin.com/\(UUID().uuidString)"
    }
    
    func getActiveStreams(for challenge: ChallengeService.Challenge? = nil) -> [VideoStream] {
        if let challenge = challenge {
            return activeStreams.filter { $0.challengeId == challenge.id && $0.isActive }
        }
        return activeStreams.filter { $0.isActive }
    }
    
    func getUserStream(_ userId: UUID) -> VideoStream? {
        return userStreams[userId]
    }
    
    func getViewableStreams() -> [VideoStream] {
        guard let userId = UserManager.shared.currentUser?.id,
              let user = UserManager.shared.currentUser else {
            return []
        }
        
        // Premium und VIP Premium Benutzer sehen alle Streams
        if user.isVIPPremium || user.isDeveloperVIPPremium {
            return activeStreams.filter { $0.isActive }
        }
        
        // Free User sehen nur öffentliche Streams
        return activeStreams.filter { stream in
            stream.isActive && (!stream.isPrivate || stream.allowedViewers.contains(userId))
        }
    }
} 