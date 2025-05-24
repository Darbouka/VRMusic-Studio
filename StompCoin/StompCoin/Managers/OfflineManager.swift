import Foundation
import CoreData

class OfflineManager {
    static let shared = OfflineManager()
    
    private let syncQueue = DispatchQueue(label: "com.stompcoin.sync", qos: .utility)
    private let storageManager: StorageManager
    private var pendingOperations: [SyncOperation] = []
    private var isSyncing = false
    
    // MARK: - Initialisierung
    
    init() {
        storageManager = StorageManager.shared
        setupNotificationObservers()
    }
    
    // MARK: - Offline-Datenspeicherung
    
    func saveData<T: Codable>(_ data: T, forKey key: String) throws {
        try storageManager.save(data, forKey: key)
    }
    
    func loadData<T: Codable>(forKey key: String) throws -> T? {
        return try storageManager.load(forKey: key)
    }
    
    func deleteData(forKey key: String) throws {
        try storageManager.delete(forKey: key)
    }
    
    // MARK: - Fitness-Tracking
    
    func trackStepsOffline(_ steps: Int) {
        let fitnessData = FitnessData(
            steps: steps,
            timestamp: Date(),
            isSynced: false
        )
        
        do {
            try saveFitnessData(fitnessData)
            addSyncOperation(.fitness(fitnessData))
        } catch {
            print("Fehler beim Speichern der Fitness-Daten: \(error)")
        }
    }
    
    func trackCaloriesOffline(_ calories: Double) {
        let fitnessData = FitnessData(
            calories: calories,
            timestamp: Date(),
            isSynced: false
        )
        
        do {
            try saveFitnessData(fitnessData)
            addSyncOperation(.fitness(fitnessData))
        } catch {
            print("Fehler beim Speichern der Kalorien-Daten: \(error)")
        }
    }
    
    // MARK: - Transaktionen
    
    func saveTransactionOffline(_ transaction: Transaction) {
        do {
            try saveTransaction(transaction)
            addSyncOperation(.transaction(transaction))
        } catch {
            print("Fehler beim Speichern der Transaktion: \(error)")
        }
    }
    
    // MARK: - Synchronisierung
    
    func sync() async throws {
        guard !isSyncing else { return }
        isSyncing = true
        
        do {
            try await syncFitnessData()
            try await syncTransactions()
            try await syncSocialData()
            isSyncing = false
        } catch {
            isSyncing = false
            throw error
        }
    }
    
    private func syncFitnessData() async throws {
        let unsyncedData = try loadUnsyncedFitnessData()
        for data in unsyncedData {
            try await syncFitnessData(data)
        }
    }
    
    private func syncTransactions() async throws {
        let unsyncedTransactions = try loadUnsyncedTransactions()
        for transaction in unsyncedTransactions {
            try await syncTransaction(transaction)
        }
    }
    
    private func syncSocialData() async throws {
        let unsyncedSocialData = try loadUnsyncedSocialData()
        for data in unsyncedSocialData {
            try await syncSocialData(data)
        }
    }
    
    // MARK: - Hilfsmethoden
    
    private func setupNotificationObservers() {
        NotificationCenter.default.addObserver(
            self,
            selector: #selector(handleNetworkStatusChange),
            name: .networkStatusChanged,
            object: nil
        )
    }
    
    @objc private func handleNetworkStatusChange(_ notification: Notification) {
        if let isConnected = notification.userInfo?["isConnected"] as? Bool, isConnected {
            Task {
                try? await sync()
            }
        }
    }
    
    private func addSyncOperation(_ operation: SyncOperation) {
        syncQueue.async {
            self.pendingOperations.append(operation)
        }
    }
    
    private func saveFitnessData(_ data: FitnessData) throws {
        try storageManager.save(data, forKey: "fitness_\(data.timestamp.timeIntervalSince1970)")
    }
    
    private func saveTransaction(_ transaction: Transaction) throws {
        try storageManager.save(transaction, forKey: "transaction_\(transaction.id)")
    }
    
    private func loadUnsyncedFitnessData() throws -> [FitnessData] {
        // Implementierung zum Laden nicht synchronisierter Fitness-Daten
        return []
    }
    
    private func loadUnsyncedTransactions() throws -> [Transaction] {
        // Implementierung zum Laden nicht synchronisierter Transaktionen
        return []
    }
    
    private func loadUnsyncedSocialData() throws -> [SocialData] {
        // Implementierung zum Laden nicht synchronisierter Social-Daten
        return []
    }
    
    private func syncFitnessData(_ data: FitnessData) async throws {
        // Implementierung der Fitness-Daten-Synchronisierung
    }
    
    private func syncTransaction(_ transaction: Transaction) async throws {
        // Implementierung der Transaktions-Synchronisierung
    }
    
    private func syncSocialData(_ data: SocialData) async throws {
        // Implementierung der Social-Daten-Synchronisierung
    }
}

// MARK: - Modelle

struct FitnessData: Codable {
    var steps: Int?
    var calories: Double?
    let timestamp: Date
    var isSynced: Bool
}

struct SocialData: Codable {
    let id: UUID
    let type: SocialDataType
    let content: String
    let timestamp: Date
    var isSynced: Bool
}

enum SocialDataType: String, Codable {
    case post
    case comment
    case like
    case friendRequest
}

enum SyncOperation {
    case fitness(FitnessData)
    case transaction(Transaction)
    case social(SocialData)
}

// MARK: - Storage Manager

class StorageManager {
    static let shared = StorageManager()
    
    private let fileManager = FileManager.default
    private let documentsPath: String
    
    init() {
        documentsPath = NSSearchPathForDirectoriesInDomains(.documentDirectory, .userDomainMask, true)[0]
    }
    
    func save<T: Codable>(_ data: T, forKey key: String) throws {
        let data = try JSONEncoder().encode(data)
        let filePath = (documentsPath as NSString).appendingPathComponent(key)
        try data.write(to: URL(fileURLWithPath: filePath))
    }
    
    func load<T: Codable>(forKey key: String) throws -> T? {
        let filePath = (documentsPath as NSString).appendingPathComponent(key)
        let data = try Data(contentsOf: URL(fileURLWithPath: filePath))
        return try JSONDecoder().decode(T.self, from: data)
    }
    
    func delete(forKey key: String) throws {
        let filePath = (documentsPath as NSString).appendingPathComponent(key)
        try fileManager.removeItem(atPath: filePath)
    }
} 