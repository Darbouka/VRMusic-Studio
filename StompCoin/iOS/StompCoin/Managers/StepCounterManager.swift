import Foundation
import CoreMotion

class StepCounterManager {
    static let shared = StepCounterManager()
    private let pedometer = CMPedometer()
    private var isCounting = false
    
    private init() {}
    
    func startCounting() {
        guard CMPedometer.isStepCountingAvailable() else {
            NotificationCenter.default.post(
                name: .stepCounterError,
                object: nil,
                userInfo: ["error": "Schrittzähler nicht verfügbar"]
            )
            return
        }
        
        isCounting = true
        pedometer.startUpdates(from: Date()) { [weak self] data, error in
            guard let self = self, self.isCounting else { return }
            
            if let error = error {
                NotificationCenter.default.post(
                    name: .stepCounterError,
                    object: nil,
                    userInfo: ["error": error.localizedDescription]
                )
                return
            }
            
            if let steps = data?.numberOfSteps.intValue {
                NotificationCenter.default.post(
                    name: .stepsUpdated,
                    object: nil,
                    userInfo: ["steps": steps]
                )
            }
        }
    }
    
    func stopCounting() {
        isCounting = false
        pedometer.stopUpdates()
    }
    
    func getTodaySteps(completion: @escaping (Int) -> Void) {
        let calendar = Calendar.current
        let now = Date()
        let startOfDay = calendar.startOfDay(for: now)
        
        pedometer.queryPedometerData(from: startOfDay, to: now) { data, error in
            if let error = error {
                print("Fehler beim Abrufen der Schritte: \(error.localizedDescription)")
                completion(0)
                return
            }
            
            completion(data?.numberOfSteps.intValue ?? 0)
        }
    }
    
    func isStepCountingAvailable() -> Bool {
        return CMPedometer.isStepCountingAvailable()
    }
} 