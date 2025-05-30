import Foundation
import FirebaseAnalytics
import FirebasePerformance
import FirebaseCrashlytics

class AnalyticsManager {
    static let shared = AnalyticsManager()
    
    private var experiments: [String: Experiment] = [:]
    private var metrics: [String: Metric] = [:]
    
    // MARK: - Initialisierung
    
    func configure() {
        // Firebase Analytics konfigurieren
        Analytics.setAnalyticsCollectionEnabled(true)
        
        // Performance Monitoring konfigurieren
        Performance.sharedInstance().isDataCollectionEnabled = true
        
        // Crashlytics konfigurieren
        Crashlytics.crashlytics().setCrashlyticsCollectionEnabled(true)
    }
    
    // MARK: - Nutzungsstatistiken
    
    func trackScreenView(screenName: String, screenClass: String) {
        Analytics.logEvent(AnalyticsEventScreenView, parameters: [
            AnalyticsParameterScreenName: screenName,
            AnalyticsParameterScreenClass: screenClass
        ])
    }
    
    func trackUserAction(action: String, parameters: [String: Any]?) {
        Analytics.logEvent(action, parameters: parameters)
    }
    
    func trackUserProperty(name: String, value: String) {
        Analytics.setUserProperty(value, forName: name)
    }
    
    func trackFitnessActivity(steps: Int, calories: Double, distance: Double) {
        Analytics.logEvent("fitness_activity", parameters: [
            "steps": steps,
            "calories": calories,
            "distance": distance
        ])
    }
    
    func trackTradingActivity(type: String, amount: Double, success: Bool) {
        Analytics.logEvent("trading_activity", parameters: [
            "type": type,
            "amount": amount,
            "success": success
        ])
    }
    
    // MARK: - Performance Monitoring
    
    func startTrace(name: String) -> Trace {
        return Performance.sharedInstance().trace(name: name)
    }
    
    func trackMetric(name: String, value: Double) {
        let metric = Metric(name: name, value: value, timestamp: Date())
        metrics[name] = metric
    }
    
    func trackNetworkRequest(url: String, method: String, duration: TimeInterval) {
        let trace = startTrace(name: "network_request")
        trace.setValue(url, forAttribute: "url")
        trace.setValue(method, forAttribute: "method")
        trace.setValue(duration, forAttribute: "duration")
        trace.stop()
    }
    
    func trackAppStartup(duration: TimeInterval) {
        let trace = startTrace(name: "app_startup")
        trace.setValue(duration, forAttribute: "duration")
        trace.stop()
    }
    
    // MARK: - Fehler-Tracking
    
    func logError(_ error: Error, context: [String: Any]? = nil) {
        Crashlytics.crashlytics().record(error: error, userInfo: context)
    }
    
    func logCustomError(name: String, reason: String, context: [String: Any]? = nil) {
        let error = NSError(domain: "com.stompcoin", code: -1, userInfo: [
            NSLocalizedDescriptionKey: reason,
            "context": context ?? [:]
        ])
        logError(error)
    }
    
    func setUserIdentifier(_ identifier: String) {
        Crashlytics.crashlytics().setUserID(identifier)
    }
    
    // MARK: - A/B Testing
    
    func startExperiment(name: String, variants: [String]) {
        let experiment = Experiment(name: name, variants: variants)
        experiments[name] = experiment
    }
    
    func assignVariant(for experiment: String) -> String? {
        guard let experiment = experiments[experiment] else { return nil }
        return experiment.assignVariant()
    }
    
    func trackExperimentEvent(experiment: String, variant: String, event: String) {
        Analytics.logEvent("experiment_event", parameters: [
            "experiment": experiment,
            "variant": variant,
            "event": event
        ])
    }
    
    // MARK: - Berichte
    
    func generateUsageReport() -> UsageReport {
        return UsageReport(
            activeUsers: calculateActiveUsers(),
            averageSessionDuration: calculateAverageSessionDuration(),
            mostUsedFeatures: getMostUsedFeatures(),
            conversionRate: calculateConversionRate()
        )
    }
    
    func generatePerformanceReport() -> PerformanceReport {
        return PerformanceReport(
            averageResponseTime: calculateAverageResponseTime(),
            errorRate: calculateErrorRate(),
            memoryUsage: getMemoryUsage(),
            cpuUsage: getCPUUsage()
        )
    }
    
    // MARK: - Hilfsmethoden
    
    private func calculateActiveUsers() -> Int {
        let calendar = Calendar.current
        let now = Date()
        let startOfDay = calendar.startOfDay(for: now)
        
        let descriptor = FetchDescriptor<User>(
            predicate: #Predicate<User> { $0.lastActive >= startOfDay }
        )
        
        return (try? modelContext.fetch(descriptor).count) ?? 0
    }
    
    private func calculateAverageSessionDuration() -> TimeInterval {
        let descriptor = FetchDescriptor<Session>(
            predicate: #Predicate<Session> { $0.endDate != nil }
        )
        
        guard let sessions = try? modelContext.fetch(descriptor),
              !sessions.isEmpty else {
            return 0
        }
        
        let totalDuration = sessions.reduce(0) { total, session in
            total + session.endDate!.timeIntervalSince(session.startDate)
        }
        
        return totalDuration / Double(sessions.count)
    }
    
    private func getMostUsedFeatures() -> [String: Int] {
        let descriptor = FetchDescriptor<FeatureUsage>()
        guard let usages = try? modelContext.fetch(descriptor) else {
            return [:]
        }
        
        var featureCounts: [String: Int] = [:]
        for usage in usages {
            featureCounts[usage.featureName, default: 0] += 1
        }
        
        return featureCounts
    }
    
    private func calculateConversionRate() -> Double {
        let descriptor = FetchDescriptor<User>()
        guard let users = try? modelContext.fetch(descriptor),
              !users.isEmpty else {
            return 0
        }
        
        let verifiedUsers = users.filter { $0.isVerified }.count
        return Double(verifiedUsers) / Double(users.count)
    }
    
    private func calculateAverageResponseTime() -> TimeInterval {
        let descriptor = FetchDescriptor<APIRequest>()
        guard let requests = try? modelContext.fetch(descriptor),
              !requests.isEmpty else {
            return 0
        }
        
        let totalResponseTime = requests.reduce(0) { total, request in
            total + request.responseTime
        }
        
        return totalResponseTime / Double(requests.count)
    }
    
    private func calculateErrorRate() -> Double {
        let descriptor = FetchDescriptor<APIRequest>()
        guard let requests = try? modelContext.fetch(descriptor),
              !requests.isEmpty else {
            return 0
        }
        
        let errorCount = requests.filter { $0.statusCode >= 400 }.count
        return Double(errorCount) / Double(requests.count)
    }
    
    private func getMemoryUsage() -> Double {
        var info = mach_task_basic_info()
        var count = mach_msg_type_number_t(MemoryLayout<mach_task_basic_info>.size)/4
        
        let kerr: kern_return_t = withUnsafeMutablePointer(to: &info) {
            $0.withMemoryRebound(to: integer_t.self, capacity: 1) {
                task_info(mach_task_self_,
                         task_flavor_t(MACH_TASK_BASIC_INFO),
                         $0,
                         &count)
            }
        }
        
        guard kerr == KERN_SUCCESS else {
            return 0
        }
        
        return Double(info.resident_size) / 1024.0 / 1024.0 // Convert to MB
    }
    
    private func getCPUUsage() -> Double {
        var totalUsage: Double = 0
        var threadList: thread_act_array_t?
        var threadCount: mach_msg_type_number_t = 0
        
        let result = task_threads(mach_task_self_, &threadList, &threadCount)
        guard result == KERN_SUCCESS else {
            return 0
        }
        
        defer {
            if let threadList = threadList {
                vm_deallocate(mach_task_self_,
                             vm_address_t(bitPattern: threadList),
                             vm_size_t(threadCount * MemoryLayout<thread_act_t>.stride))
            }
        }
        
        for i in 0..<Int(threadCount) {
            var threadInfo = thread_basic_info()
            var count = mach_msg_type_number_t(THREAD_BASIC_INFO_COUNT)
            
            let threadResult = withUnsafeMutablePointer(to: &threadInfo) {
                $0.withMemoryRebound(to: integer_t.self, capacity: 1) {
                    thread_info(threadList![i],
                              thread_flavor_t(THREAD_BASIC_INFO),
                              $0,
                              &count)
                }
            }
            
            if threadResult == KERN_SUCCESS {
                totalUsage += Double(threadInfo.cpu_usage) / Double(TH_USAGE_SCALE)
            }
        }
        
        return totalUsage * 100.0 // Convert to percentage
    }
}

// MARK: - Modelle

struct Experiment {
    let name: String
    let variants: [String]
    private var assignments: [String: String] = [:]
    
    mutating func assignVariant() -> String {
        let variant = variants.randomElement() ?? variants[0]
        assignments[UUID().uuidString] = variant
        return variant
    }
}

struct Metric {
    let name: String
    let value: Double
    let timestamp: Date
}

struct UsageReport {
    let activeUsers: Int
    let averageSessionDuration: TimeInterval
    let mostUsedFeatures: [String: Int]
    let conversionRate: Double
}

struct PerformanceReport {
    let averageResponseTime: TimeInterval
    let errorRate: Double
    let memoryUsage: Double
    let cpuUsage: Double
} 