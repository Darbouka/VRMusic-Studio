import Foundation
import WebRTC

protocol WebRTCClientDelegate: AnyObject {
    func webRTCClient(_ client: WebRTCClient, didReceiveRemoteVideoTrack track: RTCVideoTrack)
    func webRTCClient(_ client: WebRTCClient, didChangeConnectionState state: RTCIceConnectionState)
}

class WebRTCClient {
    weak var delegate: WebRTCClientDelegate?
    
    private var peerConnection: RTCPeerConnection?
    private var factory: RTCPeerConnectionFactory?
    private var videoCapturer: RTCVideoCapturer?
    private var localVideoTrack: RTCVideoTrack?
    private var localAudioTrack: RTCAudioTrack?
    private var currentPosition: AVCaptureDevice.Position = .front
    
    private let configuration: RTCConfiguration = {
        let config = RTCConfiguration()
        config.iceServers = [
            RTCIceServer(urlStrings: ["stun:stun.l.google.com:19302"]),
            RTCIceServer(urlStrings: ["stun:stun1.l.google.com:19302"])
        ]
        config.sdpSemantics = .unifiedPlan
        config.continualGatheringPolicy = .gatherContinually
        return config
    }()
    
    private let constraints = RTCMediaConstraints(
        mandatoryConstraints: [
            "OfferToReceiveAudio": "true",
            "OfferToReceiveVideo": "true"
        ],
        optionalConstraints: nil
    )
    
    init() {
        setupPeerConnectionFactory()
    }
    
    private func setupPeerConnectionFactory() {
        RTCInitializeSSL()
        factory = RTCPeerConnectionFactory()
    }
    
    func setup() async throws {
        guard let factory = factory else {
            throw StreamingError.setupFailed
        }
        
        // Peer-Verbindung erstellen
        peerConnection = factory.peerConnection(
            with: configuration,
            constraints: constraints,
            delegate: nil
        )
        
        // Video-Capturer einrichten
        videoCapturer = RTCVideoCapturer()
        
        // Video-Quelle erstellen
        let videoSource = factory.videoSource()
        videoSource.adaptOutputFormat(
            toWidth: 640,
            height: 480,
            fps: 30
        )
        
        // Video-Track erstellen
        localVideoTrack = factory.videoTrack(
            with: videoSource,
            trackId: "video0"
        )
        
        // Audio-Track erstellen
        let audioConstrains = RTCMediaConstraints(
            mandatoryConstraints: nil,
            optionalConstraints: nil
        )
        let audioSource = factory.audioSource(with: audioConstrains)
        localAudioTrack = factory.audioTrack(
            with: audioSource,
            trackId: "audio0"
        )
    }
    
    func createLocalMediaStream() -> RTCMediaStream? {
        guard let factory = factory else { return nil }
        return factory.mediaStream(withStreamId: "stream0")
    }
    
    func createVideoTrack() -> RTCVideoTrack? {
        return localVideoTrack
    }
    
    func createAudioTrack() -> RTCAudioTrack? {
        return localAudioTrack
    }
    
    func createOffer() async throws {
        guard let peerConnection = peerConnection else {
            throw StreamingError.setupFailed
        }
        
        let offer = try await peerConnection.offer(for: constraints)
        try await peerConnection.setLocalDescription(offer)
    }
    
    func handle(remoteSessionDescription: RTCSessionDescription) async throws {
        guard let peerConnection = peerConnection else {
            throw StreamingError.setupFailed
        }
        
        try await peerConnection.setRemoteDescription(remoteSessionDescription)
        
        if remoteSessionDescription.type == .offer {
            let answer = try await peerConnection.answer(for: constraints)
            try await peerConnection.setLocalDescription(answer)
        }
    }
    
    func add(iceCandidate: RTCIceCandidate) {
        peerConnection?.add(iceCandidate)
    }
    
    func switchCamera() async throws {
        currentPosition = currentPosition == .front ? .back : .front
        
        guard let videoCapturer = videoCapturer as? RTCCameraVideoCapturer else {
            throw StreamingError.setupFailed
        }
        
        guard let device = AVCaptureDevice.devices(for: .video)
            .first(where: { $0.position == currentPosition }) else {
            throw StreamingError.setupFailed
        }
        
        try await videoCapturer.startCapture(
            with: device,
            format: device.activeFormat,
            fps: 30
        )
    }
    
    func close() {
        peerConnection?.close()
        peerConnection = nil
        localVideoTrack = nil
        localAudioTrack = nil
        videoCapturer = nil
    }
}

// MARK: - RTCPeerConnectionDelegate
extension WebRTCClient: RTCPeerConnectionDelegate {
    func peerConnection(_ peerConnection: RTCPeerConnection, didChange stateChanged: RTCSignalingState) {
        // Signaling-Status-Änderungen verarbeiten
    }
    
    func peerConnection(_ peerConnection: RTCPeerConnection, didAdd stream: RTCMediaStream) {
        // Remote-Stream hinzugefügt
        if let videoTrack = stream.videoTracks.first {
            delegate?.webRTCClient(self, didReceiveRemoteVideoTrack: videoTrack)
        }
    }
    
    func peerConnection(_ peerConnection: RTCPeerConnection, didRemove stream: RTCMediaStream) {
        // Remote-Stream entfernt
    }
    
    func peerConnectionShouldNegotiate(_ peerConnection: RTCPeerConnection) {
        // Neuverhandlung erforderlich
    }
    
    func peerConnection(_ peerConnection: RTCPeerConnection, didChange newState: RTCIceConnectionState) {
        // ICE-Verbindungsstatus geändert
        delegate?.webRTCClient(self, didChangeConnectionState: newState)
    }
    
    func peerConnection(_ peerConnection: RTCPeerConnection, didChange newState: RTCIceGatheringState) {
        // ICE-Sammlungsstatus geändert
    }
    
    func peerConnection(_ peerConnection: RTCPeerConnection, didGenerate candidate: RTCIceCandidate) {
        // ICE-Kandidat generiert
    }
    
    func peerConnection(_ peerConnection: RTCPeerConnection, didRemove candidates: [RTCIceCandidate]) {
        // ICE-Kandidaten entfernt
    }
    
    func peerConnection(_ peerConnection: RTCPeerConnection, didOpen dataChannel: RTCDataChannel) {
        // Datenkanal geöffnet
    }
} 