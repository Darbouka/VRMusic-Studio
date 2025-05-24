import Foundation
import UserNotifications
import WatchConnectivity
import Intents
import WidgetKit

class AdvancedFeaturesManager: NSObject {
    static let shared = AdvancedFeaturesManager()
    
    private var wcSession: WCSession?
    private var notificationCenter: UNUserNotificationCenter
    
    // MARK: - Initialisierung
    
    override init() {
        notificationCenter = UNUserNotificationCenter.current()
        super.init()
        
        setupWatchConnectivity()
        setupNotificationCenter()
    }
    
    // MARK: - Push-Benachrichtigungen
    
    func requestNotificationPermission() async throws -> Bool {
        let options: UNAuthorizationOptions = [.alert, .sound, .badge]
        return try await notificationCenter.requestAuthorization(options: options)
    }
    
    func scheduleNotification(title: String, body: String, trigger: UNNotificationTrigger) {
        let content = UNMutableNotificationContent()
        content.title = title
        content.body = body
        content.sound = .default
        
        let request = UNNotificationRequest(
            identifier: UUID().uuidString,
            content: content,
            trigger: trigger
        )
        
        notificationCenter.add(request)
    }
    
    func scheduleFitnessReminder() {
        let trigger = UNTimeIntervalNotificationTrigger(timeInterval: 3600, repeats: true)
        scheduleNotification(
            title: "Zeit für Bewegung!",
            body: "Bewege dich für zusätzliche STOMP-Coins!",
            trigger: trigger
        )
    }
    
    func scheduleTradingAlert(price: Double) {
        let trigger = UNTimeIntervalNotificationTrigger(timeInterval: 1, repeats: false)
        scheduleNotification(
            title: "Trading-Alert",
            body: "Der STOMP-Coin Preis hat \(price) erreicht!",
            trigger: trigger
        )
    }
    
    // MARK: - Widgets
    
    func updateWidgetData() {
        let widgetData = WidgetData(
            steps: getCurrentSteps(),
            coins: getCurrentCoins(),
            lastUpdate: Date()
        )
        
        if let sharedDefaults = UserDefaults(suiteName: "group.com.stompcoin.widget") {
            sharedDefaults.set(try? JSONEncoder().encode(widgetData), forKey: "widgetData")
        }
        
        WidgetCenter.shared.reloadAllTimelines()
    }
    
    // MARK: - Apple Watch Integration
    
    private func setupWatchConnectivity() {
        if WCSession.isSupported() {
            wcSession = WCSession.default
            wcSession?.delegate = self
            wcSession?.activate()
        }
    }
    
    func sendFitnessDataToWatch(_ data: FitnessData) {
        guard let session = wcSession, session.activationState == .activated else { return }
        
        let watchData: [String: Any] = [
            "type": "fitness",
            "steps": data.steps ?? 0,
            "calories": data.calories ?? 0,
            "timestamp": data.timestamp.timeIntervalSince1970
        ]
        
        session.transferUserInfo(watchData)
    }
    
    func sendWalletDataToWatch(_ data: WalletData) {
        guard let session = wcSession, session.activationState == .activated else { return }
        
        let watchData: [String: Any] = [
            "type": "wallet",
            "balance": data.balance,
            "lastTransaction": data.lastTransaction?.amount ?? 0,
            "timestamp": Date().timeIntervalSince1970
        ]
        
        session.transferUserInfo(watchData)
    }
    
    // MARK: - Siri Integration
    
    func donateShortcuts() {
        // Fitness-Tracking Shortcut
        let fitnessIntent = TrackFitnessIntent()
        fitnessIntent.suggestedInvocationPhrase = "Tracke meine Schritte"
        let fitnessInteraction = INInteraction(intent: fitnessIntent, response: nil)
        fitnessInteraction.donate { error in
            if let error = error {
                print("Fehler beim Spenden des Fitness-Shortcuts: \(error)")
            }
        }
        
        // Wallet-Check Shortcut
        let walletIntent = CheckWalletIntent()
        walletIntent.suggestedInvocationPhrase = "Zeig mir mein Guthaben"
        let walletInteraction = INInteraction(intent: walletIntent, response: nil)
        walletInteraction.donate { error in
            if let error = error {
                print("Fehler beim Spenden des Wallet-Shortcuts: \(error)")
            }
        }
    }
    
    // MARK: - Hilfsmethoden
    
    private func setupNotificationCenter() {
        notificationCenter.delegate = self
    }
    
    private func getCurrentSteps() -> Int {
        // Implementierung zum Abrufen der aktuellen Schritte
        return 0
    }
    
    private func getCurrentCoins() -> Double {
        // Implementierung zum Abrufen der aktuellen Coins
        return 0.0
    }
}

// MARK: - WCSessionDelegate

extension AdvancedFeaturesManager: WCSessionDelegate {
    func session(_ session: WCSession, activationDidCompleteWith activationState: WCSessionActivationState, error: Error?) {
        if let error = error {
            print("Fehler bei der Watch-Connectivity-Aktivierung: \(error)")
        }
    }
    
    func sessionDidBecomeInactive(_ session: WCSession) {
        print("Watch-Connectivity-Session wurde inaktiv")
    }
    
    func sessionDidDeactivate(_ session: WCSession) {
        print("Watch-Connectivity-Session wurde deaktiviert")
        WCSession.default.activate()
    }
    
    func session(_ session: WCSession, didReceiveUserInfo userInfo: [String : Any] = [:]) {
        // Verarbeitung der von der Apple Watch empfangenen Daten
        if let type = userInfo["type"] as? String {
            switch type {
            case "fitness":
                handleFitnessData(userInfo)
            case "wallet":
                handleWalletData(userInfo)
            default:
                break
            }
        }
    }
    
    private func handleFitnessData(_ userInfo: [String: Any]) {
        // Implementierung der Fitness-Daten-Verarbeitung
    }
    
    private func handleWalletData(_ userInfo: [String: Any]) {
        // Implementierung der Wallet-Daten-Verarbeitung
    }
}

// MARK: - UNUserNotificationCenterDelegate

extension AdvancedFeaturesManager: UNUserNotificationCenterDelegate {
    func userNotificationCenter(_ center: UNUserNotificationCenter, willPresent notification: UNNotification, withCompletionHandler completionHandler: @escaping (UNNotificationPresentationOptions) -> Void) {
        completionHandler([.banner, .sound])
    }
    
    func userNotificationCenter(_ center: UNUserNotificationCenter, didReceive response: UNNotificationResponse, withCompletionHandler completionHandler: @escaping () -> Void) {
        // Verarbeitung der Benutzerinteraktion mit der Benachrichtigung
        completionHandler()
    }
}

// MARK: - Modelle

struct WidgetData: Codable {
    let steps: Int
    let coins: Double
    let lastUpdate: Date
}

struct WalletData {
    let balance: Double
    let lastTransaction: Transaction?
}

// MARK: - Siri Intents

class TrackFitnessIntent: INIntent {
    override init() {
        super.init()
    }
    
    required init?(coder: NSCoder) {
        super.init(coder: coder)
    }
}

class CheckWalletIntent: INIntent {
    override init() {
        super.init()
    }
    
    required init?(coder: NSCoder) {
        super.init(coder: coder)
    }
} 