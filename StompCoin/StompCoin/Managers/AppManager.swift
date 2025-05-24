import Foundation
import SwiftData
import Combine

class AppManager: ObservableObject {
    static let shared = AppManager()
    
    @Published var isInitialized = false
    @Published var currentUser: User?
    
    private let stepManager = StepManager()
    private var cancellables = Set<AnyCancellable>()
    
    private init() {
        setupStepTracking()
    }
    
    // App initialisieren
    func initialize(modelContext: ModelContext) {
        // Beispiel-Benutzer erstellen, wenn keiner existiert
        let descriptor = FetchDescriptor<User>()
        if let users = try? modelContext.fetch(descriptor), users.isEmpty {
            let user = User(username: "Demo User", email: "demo@example.com")
            modelContext.insert(user)
            try? modelContext.save()
        }
        
        // Aktuellen Benutzer setzen
        if let user = try? modelContext.fetch(descriptor).first {
            currentUser = user
        }
        
        isInitialized = true
    }
    
    // Schritt-Tracking einrichten
    private func setupStepTracking() {
        stepManager.$currentSteps
            .sink { [weak self] steps in
                guard let self = self,
                      let user = self.currentUser,
                      let modelContext = user.modelContext else { return }
                
                // Schritte aktualisieren
                self.stepManager.updateUserSteps(user: user, modelContext: modelContext)
                
                // Coins generieren
                if steps > 0 {
                    _ = CoinManager.shared.generateCoins(from: steps, user: user, modelContext: modelContext)
                }
            }
            .store(in: &cancellables)
    }
    
    // Schritt-Tracking starten/stoppen
    func toggleStepTracking() {
        if stepManager.isTracking {
            stepManager.stopTracking()
        } else {
            stepManager.startTracking()
        }
    }
    
    // Video-Boost aktivieren
    func activateVideoBoost(user: User, modelContext: ModelContext) -> Bool {
        guard user.videoBoostsRemaining > 0 else { return false }
        
        user.videoBoostsRemaining -= 1
        
        // Boost-Multiplikator für eine Stunde aktivieren
        let boostMultiplier = 2.0
        let boostDuration: TimeInterval = 3600 // 1 Stunde
        
        // Boost-Timer starten
        Timer.scheduledTimer(withTimeInterval: boostDuration, repeats: false) { _ in
            // Boost-Effekt beenden
            user.currentBoostMultiplier = 1.0
            try? modelContext.save()
        }
        
        // Boost-Effekt aktivieren
        user.currentBoostMultiplier = boostMultiplier
        user.lastBoostActivation = Date()
        
        try? modelContext.save()
        return true
    }
    
    // Benutzerprofil aktualisieren
    func updateProfile(
        user: User,
        username: String,
        email: String,
        favoriteGenres: [MusicGenre],
        modelContext: ModelContext
    ) -> Bool {
        user.username = username
        user.email = email
        user.favoriteMusicGenres = favoriteGenres
        
        try? modelContext.save()
        return true
    }
    
    // Schritt-Limits aktualisieren
    func updateStepLimits(
        user: User,
        dailyLimit: Int,
        weeklyLimit: Int,
        weekendLimit: Int,
        modelContext: ModelContext
    ) -> Bool {
        user.dailyStepLimit = dailyLimit
        user.weeklyStepLimit = weeklyLimit
        user.weekendStepLimit = weekendLimit
        
        try? modelContext.save()
        return true
    }
    
    // App-Status überprüfen
    func checkAppStatus(modelContext: ModelContext) {
        guard let user = currentUser else { return }
        
        // Abonnement-Status überprüfen
        SubscriptionManager.shared.checkSubscriptionStatus(user: user, modelContext: modelContext)
        
        // Event-Status aktualisieren
        for participation in user.eventHistory {
            EventManager.shared.updateEventStatus(event: participation.event)
        }
    }
} 