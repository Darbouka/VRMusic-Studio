import Foundation
import CoreLocation
import SwiftData

class EventDetectionManager: NSObject, ObservableObject, CLLocationManagerDelegate {
    static let shared = EventDetectionManager()
    
    @Published var isEventDetectionEnabled = false
    @Published var currentEvent: Event?
    @Published var availableBoosts = 4
    @Published var activeBoost: BoostType?
    @Published var boostEndTime: Date?
    @Published var lastWinnerAnnouncement: Date?
    
    private let locationManager = CLLocationManager()
    private var eventCheckTimer: Timer?
    
    enum BoostType {
        case double
        case triple
        
        var multiplier: Double {
            switch self {
            case .double: return 2.0
            case .triple: return 3.0
            }
        }
        
        var duration: TimeInterval {
            return 30 * 60 // 30 Minuten
        }
    }
    
    override private init() {
        super.init()
        locationManager.delegate = self
        locationManager.desiredAccuracy = kCLLocationAccuracyBest
        setupWinnerAnnouncementTimer()
    }
    
    func startEventDetection() {
        isEventDetectionEnabled = true
        locationManager.requestWhenInUseAuthorization()
        locationManager.startUpdatingLocation()
        
        // Event-Check alle 5 Minuten
        eventCheckTimer = Timer.scheduledTimer(withTimeInterval: 300, repeats: true) { [weak self] _ in
            self?.checkForNearbyEvents()
        }
    }
    
    func stopEventDetection() {
        isEventDetectionEnabled = false
        locationManager.stopUpdatingLocation()
        eventCheckTimer?.invalidate()
        eventCheckTimer = nil
    }
    
    func checkForNearbyEvents() {
        guard let location = locationManager.location else { return }
        
        Task {
            do {
                let events = try await EventManager.shared.getNearbyEvents(
                    latitude: location.coordinate.latitude,
                    longitude: location.coordinate.longitude,
                    radius: 5000 // 5km Radius
                )
                
                if let nearestEvent = events.first {
                    DispatchQueue.main.async {
                        self.currentEvent = nearestEvent
                        NotificationCenter.default.post(name: .eventDetected, object: nearestEvent)
                    }
                }
            } catch {
                print("Fehler beim Laden der Events: \(error)")
            }
        }
    }
    
    func joinEvent(_ event: Event) {
        // Hier Event-Teilnahme-Logik implementieren
        // Beispiel:
        print("Teilnahme am Event: \(event.name)")
        
        // Benachrichtigung senden
        NotificationCenter.default.post(name: .eventJoined, object: event)
    }
    
    func activateBoost(_ type: BoostType) {
        activeBoost = type
        availableBoosts -= 1
        boostEndTime = Date().addingTimeInterval(type.duration)
        
        // Timer für Boost-Ende
        Timer.scheduledTimer(withTimeInterval: type.duration, repeats: false) { [weak self] _ in
            self?.activeBoost = nil
            self?.boostEndTime = nil
        }
        
        // Benachrichtigung senden
        NotificationCenter.default.post(name: .boostActivated, object: type)
    }
    
    private func setupWinnerAnnouncementTimer() {
        // Timer für Gewinner-Bekanntgabe alle 6 Stunden
        Timer.scheduledTimer(withTimeInterval: 6 * 60 * 60, repeats: true) { [weak self] _ in
            self?.announceWinners()
        }
    }
    
    private func announceWinners() {
        // Hier Gewinner-Bekanntgabe-Logik implementieren
        // Beispiel:
        print("Gewinner werden bekannt gegeben")
        lastWinnerAnnouncement = Date()
        
        // Benachrichtigung senden
        NotificationCenter.default.post(name: .winnersAnnounced, object: nil)
    }
    
    // MARK: - CLLocationManagerDelegate
    
    func locationManager(_ manager: CLLocationManager, didUpdateLocations locations: [CLLocation]) {
        guard isEventDetectionEnabled else { return }
        checkForNearbyEvents()
    }
    
    func locationManager(_ manager: CLLocationManager, didFailWithError error: Error) {
        print("Standortfehler: \(error.localizedDescription)")
    }
}

// MARK: - Notification Names

extension Notification.Name {
    static let eventDetected = Notification.Name("eventDetected")
    static let eventJoined = Notification.Name("eventJoined")
    static let boostActivated = Notification.Name("boostActivated")
    static let winnersAnnounced = Notification.Name("winnersAnnounced")
} 