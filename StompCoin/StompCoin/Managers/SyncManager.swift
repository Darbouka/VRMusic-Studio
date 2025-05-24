import Foundation
import Combine

class SyncManager: ObservableObject {
    static let shared = SyncManager()
    
    @Published var isSyncing = false
    @Published var lastSyncDate: Date?
    @Published var syncError: Error?
    
    private let networkManager = NetworkManager.shared
    private let cacheManager = CacheManager.shared
    private var syncTimer: Timer?
    
    private init() {
        setupSyncTimer()
    }
    
    // MARK: - Sync Setup
    private func setupSyncTimer() {
        syncTimer = Timer.scheduledTimer(withTimeInterval: 300, repeats: true) { [weak self] _ in
            Task {
                await self?.syncData()
            }
        }
    }
    
    // MARK: - Sync Methods
    @MainActor
    func syncData() async {
        guard !isSyncing else { return }
        
        isSyncing = true
        syncError = nil
        
        do {
            // Sync Steps
            if let cachedSteps = cacheManager.getCachedSteps() {
                for stepEntry in cachedSteps {
                    try await networkManager.syncSteps(stepEntry.count)
                }
            }
            
            // Sync Events
            let events = try await networkManager.getNearbyEvents()
            try cacheManager.cacheEvents(events)
            
            // Sync Leaderboard
            for timeFrame in TimeFrame.allCases {
                let entries = try await networkManager.getLeaderboard(timeFrame: timeFrame)
                try cacheManager.cacheLeaderboard(entries, for: timeFrame)
            }
            
            lastSyncDate = Date()
        } catch {
            syncError = error
        }
        
        isSyncing = false
    }
    
    // MARK: - Manual Sync
    func forceSync() async {
        await syncData()
    }
    
    // MARK: - Conflict Resolution
    private func resolveStepConflicts(local: [StepEntry], remote: [StepEntry]) -> [StepEntry] {
        var mergedSteps = local
        
        for remoteStep in remote {
            if !mergedSteps.contains(where: { $0.timestamp == remoteStep.timestamp }) {
                mergedSteps.append(remoteStep)
            }
        }
        
        return mergedSteps.sorted { $0.timestamp < $1.timestamp }
    }
    
    // MARK: - Error Handling
    func handleSyncError(_ error: Error) {
        syncError = error
        
        // Automatischer Retry nach 5 Minuten bei Fehler
        DispatchQueue.main.asyncAfter(deadline: .now() + 300) { [weak self] in
            Task {
                await self?.syncData()
            }
        }
    }
}

// MARK: - Supporting Types
extension TimeFrame: CaseIterable {
    static var allCases: [TimeFrame] = [.daily, .weekly, .monthly, .allTime]
} 