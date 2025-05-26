import Foundation
import AVFoundation
import Combine
import CoreMotion

@MainActor
class StreamingViewModel: ObservableObject {
    @Published var isStreaming = false
    @Published var isPrivateMode = false
    @Published var isMicrophoneEnabled = true
    @Published var totalStomps = 0
    @Published var viewerCount = 0
    @Published var streamDuration = "00:00"
    @Published var selectedChallenge: Challenge?
    @Published var availableChallenges: [Challenge] = []
    @Published var showError = false
    @Published var errorMessage = ""
    @Published var currentBPM: Double = 0
    
    let captureSession = AVCaptureSession()
    private var timer: Timer?
    private var startTime: Date?
    private var cancellables = Set<AnyCancellable>()
    
    private let videoStreamingService = VideoStreamingService.shared
    private let challengeService = ChallengeService.shared
    private let stompDetectionService = StompDetectionService.shared
    
    init() {
        setupCaptureSession()
        loadAvailableChallenges()
        setupStompDetection()
    }
    
    private func setupStompDetection() {
        stompDetectionService.onStompDetected = { [weak self] stompCount in
            self?.totalStomps += stompCount
        }
    }
    
    private func setupCaptureSession() {
        captureSession.beginConfiguration()
        
        // Kamera einrichten
        guard let videoDevice = AVCaptureDevice.default(.builtInWideAngleCamera, for: .video, position: .front),
              let videoInput = try? AVCaptureDeviceInput(device: videoDevice) else {
            showError(message: "Kamera konnte nicht initialisiert werden")
            return
        }
        
        if captureSession.canAddInput(videoInput) {
            captureSession.addInput(videoInput)
        }
        
        // Mikrofon einrichten
        guard let audioDevice = AVCaptureDevice.default(for: .audio),
              let audioInput = try? AVCaptureDeviceInput(device: audioDevice) else {
            showError(message: "Mikrofon konnte nicht initialisiert werden")
            return
        }
        
        if captureSession.canAddInput(audioInput) {
            captureSession.addInput(audioInput)
        }
        
        captureSession.commitConfiguration()
    }
    
    private func loadAvailableChallenges() {
        Task {
            do {
                availableChallenges = try await challengeService.getActiveChallenges()
            } catch {
                showError(message: "Challenges konnten nicht geladen werden")
            }
        }
    }
    
    func startStream() {
        guard let challenge = selectedChallenge else { return }
        
        Task {
            do {
                try await videoStreamingService.startVideoStream(for: challenge)
                isStreaming = true
                startTime = Date()
                startTimer()
                
                // Stomp-Erkennung mit BPM starten
                if let musicBPM = challenge.musicBPM {
                    currentBPM = musicBPM
                    stompDetectionService.startStompDetection(withBPM: musicBPM)
                }
            } catch {
                showError(message: "Stream konnte nicht gestartet werden")
            }
        }
    }
    
    func endStream() {
        Task {
            do {
                try await videoStreamingService.endVideoStream()
                isStreaming = false
                stopTimer()
                stompDetectionService.stopStompDetection()
            } catch {
                showError(message: "Stream konnte nicht beendet werden")
            }
        }
    }
    
    func toggleCamera() {
        Task {
            do {
                try await videoStreamingService.switchCamera()
            } catch {
                showError(message: "Kamera konnte nicht gewechselt werden")
            }
        }
    }
    
    func toggleMicrophone() {
        isMicrophoneEnabled.toggle()
        // Mikrofon-Logik implementieren
    }
    
    func togglePrivateMode() {
        isPrivateMode.toggle()
        // Private-Mode-Logik implementieren
    }
    
    private func startTimer() {
        timer = Timer.scheduledTimer(withTimeInterval: 1.0, repeats: true) { [weak self] _ in
            self?.updateStreamDuration()
        }
    }
    
    private func stopTimer() {
        timer?.invalidate()
        timer = nil
        streamDuration = "00:00"
    }
    
    private func updateStreamDuration() {
        guard let startTime = startTime else { return }
        let duration = Date().timeIntervalSince(startTime)
        let minutes = Int(duration) / 60
        let seconds = Int(duration) % 60
        streamDuration = String(format: "%02d:%02d", minutes, seconds)
    }
    
    private func showError(message: String) {
        errorMessage = message
        showError = true
    }
} 