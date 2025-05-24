import SwiftUI
import CoreLocation
import SwiftData
import AVKit

class EventDetectionManager: ObservableObject {
    @Published var isEventDetectionEnabled = false
    @Published var currentEvent: Event?
    @Published var availableBoosts = 4
    @Published var activeBoost: BoostType?
    @Published var boostEndTime: Date?
    @Published var isWatchingAd = false
    
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
        
        var requiresAd: Bool {
            switch self {
            case .double: return false
            case .triple: return true
            }
        }
    }
}

struct EventDetectionView: View {
    @Environment(\.modelContext) private var modelContext
    @StateObject private var eventManager = EventDetectionManager()
    @State private var showingEventAlert = false
    @State private var showingBoostAlert = false
    @State private var showingAdAlert = false
    @State private var selectedBoost: EventDetectionManager.BoostType?
    @State private var adPlayer: AVPlayer?
    @State private var showingSuccess = false
    @State private var showingError = false
    @State private var errorMessage = ""
    
    var body: some View {
        VStack {
            // Event-Erkennung Status
            Section {
                Toggle("Event-Erkennung", isOn: $eventManager.isEventDetectionEnabled)
                    .onChange(of: eventManager.isEventDetectionEnabled) { _, newValue in
                        if newValue {
                            checkForNearbyEvents()
                        }
                    }
            }
            
            if let event = eventManager.currentEvent {
                // Aktuelles Event
                VStack(alignment: .leading, spacing: 10) {
                    Text("Aktuelles Event")
                        .font(.headline)
                    
                    Text(event.name)
                        .font(.title2)
                    
                    Text("Genre: \(event.musicGenres.map { $0.rawValue }.joined(separator: ", "))")
                    
                    if let challenge = event.challenges.first {
                        VStack(alignment: .leading) {
                            Text("Challenge: \(challenge.name)")
                                .font(.headline)
                            
                            Text("Preis: \(Int(challenge.reward)) StompCoins")
                            
                            if eventManager.availableBoosts > 0 {
                                Button("Boost aktivieren") {
                                    showingBoostAlert = true
                                }
                                .buttonStyle(.borderedProminent)
                            }
                            
                            if let activeBoost = eventManager.activeBoost {
                                Text("Aktiver Boost: \(activeBoost == .double ? "2x" : "3x")")
                                if let endTime = eventManager.boostEndTime {
                                    Text("Endet in: \(timeUntil(endTime))")
                                }
                            }
                        }
                    }
                }
                .padding()
                .background(Color.secondary.opacity(0.1))
                .cornerRadius(10)
            }
        }
        .padding()
        .alert("Event erkannt!", isPresented: $showingEventAlert) {
            Button("Teilnehmen") {
                joinEvent()
            }
            Button("Abbrechen", role: .cancel) {
                eventManager.isEventDetectionEnabled = false
            }
        } message: {
            if let event = eventManager.currentEvent {
                Text("Möchten Sie am Event '\(event.name)' teilnehmen und an der Challenge mitmachen?")
            }
        }
        .alert("Boost auswählen", isPresented: $showingBoostAlert) {
            Button("2x Boost (30 Min)") {
                activateBoost(.double)
            }
            Button("3x Boost (30 Min)") {
                activateBoost(.triple)
            }
            Button("Abbrechen", role: .cancel) { }
        } message: {
            Text("Wählen Sie einen Boost-Typ. Sie haben noch \(eventManager.availableBoosts) Boosts verfügbar.")
        }
        .alert("Werbevideo erforderlich", isPresented: $showingAdAlert) {
            Button("Video ansehen") {
                showAd()
            }
            Button("Abbrechen", role: .cancel) {
                selectedBoost = nil
            }
        } message: {
            Text("Für den 3x Boost müssen Sie ein Werbevideo ansehen. Möchten Sie fortfahren?")
        }
        .fullScreenCover(isPresented: $eventManager.isWatchingAd) {
            AdVideoView(player: adPlayer) {
                eventManager.isWatchingAd = false
                if let boost = selectedBoost {
                    activateBoost(boost)
                }
            }
        }
        .alert("Erfolgreich", isPresented: $showingSuccess) {
            Button("OK") { }
        } message: {
            Text("Event erfolgreich beigetreten!")
        }
        .alert("Fehler", isPresented: $showingError) {
            Button("OK") { }
        } message: {
            Text(errorMessage)
        }
    }
    
    private func checkForNearbyEvents() {
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
                        self.eventManager.currentEvent = nearestEvent
                        self.showingEventAlert = true
                    }
                }
            } catch {
                print("Fehler beim Laden der Events: \(error)")
            }
        }
    }
    
    private func joinEvent() {
        guard let event = eventManager.currentEvent else { return }
        
        Task {
            do {
                try await EventManager.shared.joinEvent(event)
                DispatchQueue.main.async {
                    self.showingSuccess = true
                }
            } catch {
                DispatchQueue.main.async {
                    self.showingError = true
                    self.errorMessage = "Event konnte nicht beigetreten werden"
                }
            }
        }
    }
    
    private func showAd() {
        Task {
            do {
                let adURL = try await AdManager.shared.getVideoAdURL()
                DispatchQueue.main.async {
                    self.adPlayer = AVPlayer(url: adURL)
                    self.eventManager.isWatchingAd = true
                }
            } catch {
                print("Fehler beim Laden des Werbevideos: \(error)")
            }
        }
    }
    
    private func activateBoost(_ type: EventDetectionManager.BoostType) {
        if type.requiresAd {
            selectedBoost = type
            showingAdAlert = true
        } else {
            eventManager.activeBoost = type
            eventManager.availableBoosts -= 1
            eventManager.boostEndTime = Date().addingTimeInterval(type.duration)
            
            // Timer für Boost-Ende
            Timer.scheduledTimer(withTimeInterval: type.duration, repeats: false) { _ in
                eventManager.activeBoost = nil
                eventManager.boostEndTime = nil
            }
        }
    }
    
    private func timeUntil(_ date: Date) -> String {
        let components = Calendar.current.dateComponents([.minute, .second], from: Date(), to: date)
        return String(format: "%02d:%02d", components.minute ?? 0, components.second ?? 0)
    }
}

struct AdVideoView: View {
    let player: AVPlayer?
    let onComplete: () -> Void
    @State private var isVideoFinished = false
    
    var body: some View {
        ZStack {
            if let player = player {
                VideoPlayer(player: player)
                    .onAppear {
                        player.play()
                    }
                    .onDisappear {
                        player.pause()
                    }
            }
            
            VStack {
                Spacer()
                
                if isVideoFinished {
                    Button("Fertig") {
                        onComplete()
                    }
                    .buttonStyle(.borderedProminent)
                    .padding()
                }
            }
        }
        .onReceive(NotificationCenter.default.publisher(for: .AVPlayerItemDidPlayToEndTime)) { _ in
            isVideoFinished = true
        }
    }
}

// Platzhalter-Modelle
struct Event {
    let name: String
    let musicGenres: [MusicGenre]
    let challenges: [Challenge]
}

struct Challenge {
    let name: String
    let reward: Double
}

#Preview {
    EventDetectionView()
        .modelContainer(for: User.self, inMemory: true)
} 