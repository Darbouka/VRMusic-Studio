import Foundation
import AVFoundation
import CoreMotion

class StompDetectionService {
    static let shared = StompDetectionService()
    
    // BPM und Kick-Erkennung
    private var currentBPM: Double = 0
    private var lastKickTime: Date?
    private var kickIntervals: [TimeInterval] = []
    private let maxKickIntervals = 10 // Anzahl der letzten Kicks für BPM-Berechnung
    
    // Stomp-Erkennung
    private let motionManager = CMMotionManager()
    private var lastStompTime: Date?
    private var stompCooldown: TimeInterval = 0.1 // Minimale Zeit zwischen Stomps
    
    // Stomp-Tracking
    private var stompCount: Int = 0
    private var sessionStartTime: Date?
    private let targetStompsPerHour: Double = 10000 // Ziel: 100.000 Stomps in 10 Stunden
    
    // Audio-Analyse
    private var audioEngine: AVAudioEngine?
    private var audioPlayer: AVAudioPlayerNode?
    private var audioFile: AVAudioFile?
    
    // Callback für erkannte Stomps
    var onStompDetected: ((Int) -> Void)?
    
    private init() {
        setupAudioEngine()
    }
    
    private func setupAudioEngine() {
        audioEngine = AVAudioEngine()
        audioPlayer = AVAudioPlayerNode()
        
        guard let audioEngine = audioEngine,
              let audioPlayer = audioPlayer else { return }
        
        audioEngine.attach(audioPlayer)
        
        // Audio-Analyse-Node für Kick-Erkennung
        let format = audioEngine.mainMixerNode.outputFormat(forBus: 0)
        audioEngine.connect(audioPlayer, to: audioEngine.mainMixerNode, format: format)
        
        // Kick-Erkennung einrichten
        audioEngine.mainMixerNode.installTap(onBus: 0, bufferSize: 1024, format: format) { [weak self] buffer, time in
            self?.analyzeAudioBuffer(buffer)
        }
    }
    
    func startStompDetection(withBPM bpm: Double) {
        currentBPM = bpm
        let kickInterval = 60.0 / bpm // Zeit zwischen Kicks in Sekunden
        stompCooldown = kickInterval * 0.8 // 80% des Kick-Intervalls als Cooldown
        
        // Session-Timer starten
        sessionStartTime = Date()
        stompCount = 0
        
        guard motionManager.isAccelerometerAvailable else { return }
        
        motionManager.accelerometerUpdateInterval = 0.01 // 100Hz für präzise Erkennung
        motionManager.startAccelerometerUpdates(to: .main) { [weak self] data, error in
            guard let self = self,
                  let data = data,
                  error == nil else { return }
            
            self.detectStomp(with: data)
        }
    }
    
    private func detectStomp(with data: CMAccelerometerData) {
        // Vertikale Beschleunigung berechnen
        let verticalAcceleration = abs(data.acceleration.y)
        
        // Stomp-Erkennung basierend auf Beschleunigung und Cooldown
        if let lastStomp = lastStompTime {
            let timeSinceLastStomp = Date().timeIntervalSince(lastStomp)
            
            // Stomp-Erkennung mit angepasstem Schwellenwert
            if timeSinceLastStomp >= stompCooldown &&
               verticalAcceleration > calculateStompThreshold() {
                handleStomp()
            }
        } else if verticalAcceleration > calculateStompThreshold() {
            // Erster Stomp ohne Cooldown-Prüfung
            handleStomp()
        }
    }
    
    private func calculateStompThreshold() -> Double {
        // Dynamischer Schwellenwert basierend auf BPM
        let baseThreshold = 1.5
        let bpmFactor = currentBPM / 120.0 // Normalisierung auf 120 BPM
        
        // Anpassung des Schwellenwerts basierend auf der aktuellen Stomp-Rate
        if let startTime = sessionStartTime {
            let elapsedHours = Date().timeIntervalSince(startTime) / 3600
            let currentStompsPerHour = Double(stompCount) / elapsedHours
            
            // Wenn die Stomp-Rate zu hoch ist, erhöhen wir den Schwellenwert
            if currentStompsPerHour > targetStompsPerHour {
                let rateFactor = currentStompsPerHour / targetStompsPerHour
                return baseThreshold * bpmFactor * rateFactor
            }
        }
        
        return baseThreshold * bpmFactor
    }
    
    private func handleStomp() {
        lastStompTime = Date()
        stompCount += 1
        
        // Stomp an den Server senden
        Task {
            do {
                try await VideoStreamingService.shared.recordStomp()
                // Jeder Stomp gibt 1 StompCoin
                onStompDetected?(1)
            } catch {
                print("Fehler beim Senden des Stomps: \(error)")
            }
        }
    }
    
    private func analyzeAudioBuffer(_ buffer: AVAudioPCMBuffer) {
        // Kick-Erkennung im Audio-Buffer
        guard let channelData = buffer.floatChannelData?[0] else { return }
        let frameCount = Int(buffer.frameLength)
        
        // Einfache Kick-Erkennung basierend auf Amplitude
        var maxAmplitude: Float = 0
        for i in 0..<frameCount {
            maxAmplitude = max(maxAmplitude, abs(channelData[i]))
        }
        
        // Kick-Schwellenwert
        let kickThreshold: Float = 0.7
        
        if maxAmplitude > kickThreshold {
            let now = Date()
            if let lastKick = lastKickTime {
                let interval = now.timeIntervalSince(lastKick)
                kickIntervals.append(interval)
                
                // Nur die letzten N Intervalle behalten
                if kickIntervals.count > maxKickIntervals {
                    kickIntervals.removeFirst()
                }
                
                // BPM aktualisieren
                if kickIntervals.count >= 2 {
                    let averageInterval = kickIntervals.reduce(0, +) / Double(kickIntervals.count)
                    currentBPM = 60.0 / averageInterval
                }
            }
            lastKickTime = now
        }
    }
    
    func stopStompDetection() {
        motionManager.stopAccelerometerUpdates()
        audioEngine?.mainMixerNode.removeTap(onBus: 0)
        audioEngine?.stop()
        
        // Session-Statistiken zurücksetzen
        sessionStartTime = nil
        stompCount = 0
    }
} 