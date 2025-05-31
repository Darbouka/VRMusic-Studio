import Foundation
import CoreData

class OfflineManager {
    static let shared = OfflineManager()
    private let notificationManager = NotificationManager.shared
    
    private init() {}
    
    func saveDataLocally(data: [String: Any]) {
        do {
            let jsonData = try JSONSerialization.data(withJSONObject: data)
            UserDefaults.standard.set(jsonData, forKey: "offlineData")
            notificationManager.showOfflineDataSavedNotification()
        } catch {
            notificationManager.showOfflineErrorNotification()
        }
    }
    
    func syncWhenOnline() {
        guard let jsonData = UserDefaults.standard.data(forKey: "offlineData"),
              let data = try? JSONSerialization.jsonObject(with: jsonData) as? [String: Any] else {
            return
        }
        
        // Synchronisiere mit dem Server
        APIManager.shared.syncData(data) { result in
            switch result {
            case .success:
                UserDefaults.standard.removeObject(forKey: "offlineData")
                self.notificationManager.showOfflineSyncNotification()
            case .failure:
                self.notificationManager.showOfflineErrorNotification()
            }
        }
    }
    
    func handleOfflineNotifications() {
        NotificationCenter.default.addObserver(
            self,
            selector: #selector(handleConnectivityChange),
            name: .connectivityChanged,
            object: nil
        )
    }
    
    @objc private func handleConnectivityChange(_ notification: Notification) {
        if let isConnected = notification.userInfo?["isConnected"] as? Bool, isConnected {
            syncWhenOnline()
        }
    }
} 