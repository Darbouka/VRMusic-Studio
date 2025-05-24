import Foundation
import CoreMotion
import SwiftData

class StepManager: ObservableObject {
    private let pedometer = CMPedometer()
    @Published var currentSteps: Int = 0
    @Published var isTracking: Bool = false
    
    func startTracking() {
        guard CMPedometer.isStepCountingAvailable() else {
            print("Schritt-Tracking ist auf diesem Gerät nicht verfügbar")
            return
        }
        
        pedometer.startUpdates(from: Date()) { [weak self] data, error in
            guard let data = data, error == nil else {
                print("Fehler beim Schritt-Tracking: \(error?.localizedDescription ?? "Unbekannter Fehler")")
                return
            }
            
            DispatchQueue.main.async {
                self?.currentSteps = data.numberOfSteps.intValue
            }
        }
        
        isTracking = true
    }
    
    func stopTracking() {
        pedometer.stopUpdates()
        isTracking = false
    }
    
    func resetSteps() {
        currentSteps = 0
    }
    
    func updateUserSteps(user: User, modelContext: ModelContext) {
        user.currentSteps = currentSteps
        
        // Überprüfe und aktualisiere die Limits
        let calendar = Calendar.current
        let today = Date()
        
        // Tägliches Limit zurücksetzen
        if !calendar.isDate(user.lastStepReset, inSameDayAs: today) {
            user.currentSteps = 0
            user.lastStepReset = today
        }
        
        // Video-Boost zurücksetzen
        if !calendar.isDate(user.lastVideoBoostReset, inSameDayAs: today) {
            user.videoBoostsRemaining = 3
            user.lastVideoBoostReset = today
        }
        
        try? modelContext.save()
    }
} 