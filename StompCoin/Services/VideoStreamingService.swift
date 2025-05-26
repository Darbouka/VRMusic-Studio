import Foundation
import AVFoundation
import WebRTC

class VideoStreamingService {
    static let shared = VideoStreamingService()
    
    private var peerConnection: RTCPeerConnection?
    private var localVideoTrack: RTCVideoTrack?
    private var localAudioTrack: RTCAudioTrack?
    private var mediaStream: RTCMediaStream?
    
    private let webRTCClient: WebRTCClient
    private let signalingClient: SignalingClient
    
    private init() {
        self.webRTCClient = WebRTCClient()
        self.signalingClient = SignalingClient()
        setupWebRTC()
    }
    
    private func setupWebRTC() {
        webRTCClient.delegate = self
        signalingClient.delegate = self
    }
    
    func startVideoStream(for challenge: Challenge) async throws {
        // Berechtigungen prüfen
        try await checkPermissions()
        
        // WebRTC-Verbindung aufbauen
        try await webRTCClient.setup()
        
        // Lokale Medien-Streams erstellen
        let constraints = RTCMediaConstraints(
            mandatoryConstraints: [
                "minWidth": "640",
                "minHeight": "480",
                "minFrameRate": "30"
            ],
            optionalConstraints: nil
        )
        
        mediaStream = webRTCClient.createLocalMediaStream()
        
        // Video-Track hinzufügen
        if let videoTrack = webRTCClient.createVideoTrack() {
            localVideoTrack = videoTrack
            mediaStream?.addVideoTrack(videoTrack)
        }
        
        // Audio-Track hinzufügen
        if let audioTrack = webRTCClient.createAudioTrack() {
            localAudioTrack = audioTrack
            mediaStream?.addAudioTrack(audioTrack)
        }
        
        // Peer-Verbindung herstellen
        try await webRTCClient.createOffer()
        
        // Stream-ID generieren
        let streamId = UUID().uuidString
        
        // Stream beim Server registrieren
        try await signalingClient.registerStream(
            streamId: streamId,
            challengeId: challenge.id
        )
    }
    
    func endVideoStream() async throws {
        // WebRTC-Verbindung beenden
        webRTCClient.close()
        
        // Lokale Medien-Streams stoppen
        localVideoTrack?.isEnabled = false
        localAudioTrack?.isEnabled = false
        mediaStream = nil
        
        // Stream beim Server beenden
        try await signalingClient.endStream()
    }
    
    func switchCamera() async throws {
        try await webRTCClient.switchCamera()
    }
    
    func recordStomp() async throws {
        // Stomp an den Server senden
        try await signalingClient.sendStompEvent()
    }
    
    private func checkPermissions() async throws {
        // Kamera-Berechtigung
        let cameraStatus = AVCaptureDevice.authorizationStatus(for: .video)
        if cameraStatus == .notDetermined {
            let granted = await AVCaptureDevice.requestAccess(for: .video)
            if !granted {
                throw StreamingError.cameraPermissionDenied
            }
        } else if cameraStatus != .authorized {
            throw StreamingError.cameraPermissionDenied
        }
        
        // Mikrofon-Berechtigung
        let microphoneStatus = AVCaptureDevice.authorizationStatus(for: .audio)
        if microphoneStatus == .notDetermined {
            let granted = await AVCaptureDevice.requestAccess(for: .audio)
            if !granted {
                throw StreamingError.microphonePermissionDenied
            }
        } else if microphoneStatus != .authorized {
            throw StreamingError.microphonePermissionDenied
        }
    }
}

// MARK: - WebRTCClientDelegate
extension VideoStreamingService: WebRTCClientDelegate {
    func webRTCClient(_ client: WebRTCClient, didReceiveRemoteVideoTrack track: RTCVideoTrack) {
        // Remote-Video-Track verarbeiten
    }
    
    func webRTCClient(_ client: WebRTCClient, didChangeConnectionState state: RTCIceConnectionState) {
        // Verbindungsstatus aktualisieren
    }
}

// MARK: - SignalingClientDelegate
extension VideoStreamingService: SignalingClientDelegate {
    func signalingClient(_ client: SignalingClient, didReceiveIceCandidate candidate: RTCIceCandidate) {
        // ICE-Kandidaten verarbeiten
        webRTCClient.add(iceCandidate: candidate)
    }
    
    func signalingClient(_ client: SignalingClient, didReceiveOffer offer: RTCSessionDescription) {
        // Angebot verarbeiten
        Task {
            try await webRTCClient.handle(remoteSessionDescription: offer)
        }
    }
    
    func signalingClient(_ client: SignalingClient, didReceiveAnswer answer: RTCSessionDescription) {
        // Antwort verarbeiten
        Task {
            try await webRTCClient.handle(remoteSessionDescription: answer)
        }
    }
}

// MARK: - Errors
enum StreamingError: Error {
    case cameraPermissionDenied
    case microphonePermissionDenied
    case setupFailed
    case connectionFailed
} 