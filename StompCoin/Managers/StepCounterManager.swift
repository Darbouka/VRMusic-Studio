import Foundation
import CoreMotion
import SwiftUI

class StepCounterManager: NSObject, ObservableObject {
    static let shared = StepCounterManager()
    
    @Published var steps: Int = 0
    @Published var isCounting = false
    @Published var lastUpdate: Date?
    @Published var error: String?
    @Published var isFitnessFloorActive = false
    
    private let pedometer = CMPedometer()
    private var startDate: Date?
    private var fitnessFloorStartDate: Date?
    
    override private init() {
        super.init()
    }
    
    func startCounting() {
        guard CMPedometer.isStepCountingAvailable() else {
            error = "Schrittzähler ist auf diesem Gerät nicht verfügbar"
            return
        }
        
        startDate = Date()
        isCounting = true
        error = nil
        
        pedometer.startUpdates(from: startDate!) { [weak self] data, error in
            guard let self = self, let data = data, error == nil else {
                self?.error = error?.localizedDescription
                return
            }
            
            DispatchQueue.main.async {
                self.steps = data.numberOfSteps.intValue
                self.lastUpdate = Date()
            }
        }
    }
    
    func stopCounting() {
        pedometer.stopUpdates()
        isCounting = false
        startDate = nil
    }
    
    func activateFitnessFloor() {
        isFitnessFloorActive = true
        fitnessFloorStartDate = Date()
    }
    
    func deactivateFitnessFloor() {
        isFitnessFloorActive = false
        fitnessFloorStartDate = nil
    }
    
    func getStepsForToday() {
        guard CMPedometer.isStepCountingAvailable() else {
            error = "Schrittzähler ist auf diesem Gerät nicht verfügbar"
            return
        }
        
        let calendar = Calendar.current
        let now = Date()
        let startOfDay = calendar.startOfDay(for: now)
        
        pedometer.queryPedometerData(from: startOfDay, to: now) { [weak self] data, error in
            guard let self = self, let data = data, error == nil else {
                self?.error = error?.localizedDescription
                return
            }
            
            DispatchQueue.main.async {
                self.steps = data.numberOfSteps.intValue
                self.lastUpdate = now
            }
        }
    }
    
    func getStepsForDateRange(from startDate: Date, to endDate: Date) {
        guard CMPedometer.isStepCountingAvailable() else {
            error = "Schrittzähler ist auf diesem Gerät nicht verfügbar"
            return
        }
        
        pedometer.queryPedometerData(from: startDate, to: endDate) { [weak self] data, error in
            guard let self = self, let data = data, error == nil else {
                self?.error = error?.localizedDescription
                return
            }
            
            DispatchQueue.main.async {
                self.steps = data.numberOfSteps.intValue
                self.lastUpdate = endDate
            }
        }
    }
    
    // Hilfsfunktion zum Formatieren der Schritte
    func formatSteps(_ steps: Int) -> String {
        let formatter = NumberFormatter()
        formatter.numberStyle = .decimal
        formatter.groupingSeparator = "."
        return formatter.string(from: NSNumber(value: steps)) ?? "0"
    }
    
    // Fitness-Floor spezifische Funktionen
    func calculateFitnessDistance() -> Double? {
        guard isFitnessFloorActive, let startDate = fitnessFloorStartDate else { return nil }
        // Durchschnittliche Schrittlänge in Metern (ca. 0,762m)
        let averageStepLength = 0.762
        return Double(steps) * averageStepLength
    }
    
    func calculateFitnessCalories() -> Double? {
        guard isFitnessFloorActive, let startDate = fitnessFloorStartDate else { return nil }
        // Durchschnittlicher Kalorienverbrauch pro Schritt (ca. 0,04 kcal)
        let caloriesPerStep = 0.04
        return Double(steps) * caloriesPerStep
    }
    
    func getFitnessDuration() -> TimeInterval? {
        guard isFitnessFloorActive, let startDate = fitnessFloorStartDate else { return nil }
        return Date().timeIntervalSince(startDate)
    }
} 