import Foundation
import WebRTC

protocol SignalingClientDelegate: AnyObject {
    func signalingClient(_ client: SignalingClient, didReceiveIceCandidate candidate: RTCIceCandidate)
    func signalingClient(_ client: SignalingClient, didReceiveOffer offer: RTCSessionDescription)
    func signalingClient(_ client: SignalingClient, didReceiveAnswer answer: RTCSessionDescription)
}

class SignalingClient {
    weak var delegate: SignalingClientDelegate?
    
    private let webSocket: URLSessionWebSocketTask
    private let serverURL: URL
    private var streamId: String?
    
    init() {
        // WebSocket-Server-URL konfigurieren
        serverURL = URL(string: "wss://stompcoin-server.com/ws")!
        
        // WebSocket-Session erstellen
        let session = URLSession(configuration: .default)
        webSocket = session.webSocketTask(with: serverURL)
        
        // WebSocket-Verbindung starten
        connect()
    }
    
    private func connect() {
        webSocket.resume()
        receiveMessage()
    }
    
    private func receiveMessage() {
        webSocket.receive { [weak self] result in
            guard let self = self else { return }
            
            switch result {
            case .success(let message):
                switch message {
                case .string(let text):
                    self.handleMessage(text)
                case .data(let data):
                    if let text = String(data: data, encoding: .utf8) {
                        self.handleMessage(text)
                    }
                @unknown default:
                    break
                }
                
                // Nächste Nachricht empfangen
                self.receiveMessage()
                
            case .failure(let error):
                print("WebSocket-Fehler: \(error)")
                // Verbindung neu aufbauen
                DispatchQueue.main.asyncAfter(deadline: .now() + 5) {
                    self.connect()
                }
            }
        }
    }
    
    private func handleMessage(_ text: String) {
        guard let data = text.data(using: .utf8),
              let message = try? JSONDecoder().decode(SignalingMessage.self, from: data) else {
            return
        }
        
        switch message.type {
        case .iceCandidate:
            if let candidate = message.candidate {
                delegate?.signalingClient(self, didReceiveIceCandidate: candidate)
            }
        case .offer:
            if let offer = message.sdp {
                delegate?.signalingClient(self, didReceiveOffer: offer)
            }
        case .answer:
            if let answer = message.sdp {
                delegate?.signalingClient(self, didReceiveAnswer: answer)
            }
        }
    }
    
    func registerStream(streamId: String, challengeId: String) async throws {
        self.streamId = streamId
        
        let message = SignalingMessage(
            type: .register,
            streamId: streamId,
            challengeId: challengeId
        )
        
        try await send(message)
    }
    
    func endStream() async throws {
        guard let streamId = streamId else { return }
        
        let message = SignalingMessage(
            type: .end,
            streamId: streamId
        )
        
        try await send(message)
    }
    
    func sendStompEvent() async throws {
        guard let streamId = streamId else { return }
        
        let message = SignalingMessage(
            type: .stomp,
            streamId: streamId
        )
        
        try await send(message)
    }
    
    private func send(_ message: SignalingMessage) async throws {
        guard let data = try? JSONEncoder().encode(message),
              let text = String(data: data, encoding: .utf8) else {
            throw SignalingError.encodingFailed
        }
        
        let message = URLSessionWebSocketTask.Message.string(text)
        
        try await withCheckedThrowingContinuation { continuation in
            webSocket.send(message) { error in
                if let error = error {
                    continuation.resume(throwing: error)
                } else {
                    continuation.resume()
                }
            }
        }
    }
}

// MARK: - Signaling Message Types
enum SignalingMessageType: String, Codable {
    case register
    case offer
    case answer
    case iceCandidate
    case end
    case stomp
}

struct SignalingMessage: Codable {
    let type: SignalingMessageType
    let streamId: String
    let challengeId: String?
    let sdp: RTCSessionDescription?
    let candidate: RTCIceCandidate?
    
    init(type: SignalingMessageType, streamId: String, challengeId: String? = nil, sdp: RTCSessionDescription? = nil, candidate: RTCIceCandidate? = nil) {
        self.type = type
        self.streamId = streamId
        self.challengeId = challengeId
        self.sdp = sdp
        self.candidate = candidate
    }
}

// MARK: - Errors
enum SignalingError: Error {
    case encodingFailed
    case connectionFailed
    case invalidMessage
} 