import Foundation
import CoreMotion
import HealthKit

class FitnessManager {
    static let shared = FitnessManager()
    
    private let pedometer = CMPedometer()
    private let healthStore = HKHealthStore()
    private var isTracking = false
    
    @Published var currentSteps: Int = 0
    @Published var caloriesBurned: Double = 0
    @Published var distance: Double = 0
    @Published var activeMinutes: Int = 0
    
    private init() {
        requestHealthKitAuthorization()
    }
    
    private func requestHealthKitAuthorization() {
        let typesToRead: Set<HKObjectType> = [
            HKObjectType.quantityType(forIdentifier: .stepCount)!,
            HKObjectType.quantityType(forIdentifier: .activeEnergyBurned)!,
            HKObjectType.quantityType(forIdentifier: .distanceWalkingRunning)!
        ]
        
        healthStore.requestAuthorization(toShare: nil, read: typesToRead) { success, error in
            if let error = error {
                print("HealthKit Authorization Error: \(error)")
            }
        }
    }
    
    func startTracking() {
        guard !isTracking else { return }
        
        isTracking = true
        pedometer.startUpdates(from: Date()) { [weak self] data, error in
            guard let data = data, error == nil else { return }
            
            DispatchQueue.main.async {
                self?.currentSteps = data.numberOfSteps.intValue
                self?.updateCaloriesBurned()
                self?.updateDistance()
            }
        }
    }
    
    func stopTracking() {
        guard isTracking else { return }
        
        isTracking = false
        pedometer.stopUpdates()
    }
    
    private func updateCaloriesBurned() {
        // Durchschnittlicher Kalorienverbrauch pro Schritt
        let caloriesPerStep = 0.04
        caloriesBurned = Double(currentSteps) * caloriesPerStep
    }
    
    private func updateDistance() {
        // Durchschnittliche Schrittlänge in Metern
        let stepLength = 0.762
        distance = Double(currentSteps) * stepLength
    }
    
    func getDailyStats() async throws -> FitnessStats {
        let calendar = Calendar.current
        let now = Date()
        let startOfDay = calendar.startOfDay(for: now)
        
        let stepType = HKQuantityType.quantityType(forIdentifier: .stepCount)!
        let calorieType = HKQuantityType.quantityType(forIdentifier: .activeEnergyBurned)!
        
        let predicate = HKQuery.predicateForSamples(
            withStart: startOfDay,
            end: now,
            options: .strictStartDate
        )
        
        async let steps = try await withCheckedThrowingContinuation { continuation in
            let query = HKStatisticsQuery(
                quantityType: stepType,
                quantitySamplePredicate: predicate,
                options: .cumulativeSum
            ) { _, result, error in
                if let error = error {
                    continuation.resume(throwing: error)
                    return
                }
                
                guard let result = result,
                      let sum = result.sumQuantity() else {
                    continuation.resume(throwing: NSError(domain: "", code: -1))
                    return
                }
                
                let steps = sum.doubleValue(for: HKUnit.count())
                continuation.resume(returning: steps)
            }
            
            healthStore.execute(query)
        }
        
        async let calories = try await withCheckedThrowingContinuation { continuation in
            let query = HKStatisticsQuery(
                quantityType: calorieType,
                quantitySamplePredicate: predicate,
                options: .cumulativeSum
            ) { _, result, error in
                if let error = error {
                    continuation.resume(throwing: error)
                    return
                }
                
                guard let result = result,
                      let sum = result.sumQuantity() else {
                    continuation.resume(throwing: NSError(domain: "", code: -1))
                    return
                }
                
                let calories = sum.doubleValue(for: HKUnit.kilocalorie())
                continuation.resume(returning: calories)
            }
            
            healthStore.execute(query)
        }
        
        let (totalSteps, totalCalories) = try await (steps, calories)
        
        return FitnessStats(
            steps: Int(totalSteps),
            caloriesBurned: totalCalories,
            distance: Double(totalSteps) * 0.762,
            activeMinutes: Int(totalSteps / 100) // Schätzung
        )
    }
}

struct FitnessStats {
    let steps: Int
    let caloriesBurned: Double
    let distance: Double
    let activeMinutes: Int
} 