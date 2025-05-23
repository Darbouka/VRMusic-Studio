import Foundation
import SwiftUI

class TradingMarketManager: ObservableObject {
    static let shared = TradingMarketManager()
    
    @Published var isPoolMiningActive = false
    @Published var isSoloMiningActive = false
    @Published var poolMiningStats: PoolMiningStats?
    @Published var soloMiningStats: SoloMiningStats?
    @Published var error: String?
    
    private var miningTimer: Timer?
    private let miningInterval: TimeInterval = 60 // 1 Minute
    
    struct PoolMiningStats {
        var totalParticipants: Int
        var combinedResearchPower: Double
        var blockWinChance: Double
        var lastBlockFound: Date?
        var totalBlocksFound: Int
    }
    
    struct SoloMiningStats {
        var researchPower: Double
        var blockWinChance: Double
        var lastBlockFound: Date?
        var totalBlocksFound: Int
    }
    
    private init() {
        // Initialisierung
    }
    
    func startPoolMining() {
        guard !isSoloMiningActive else {
            error = "Solo Mining muss zuerst beendet werden"
            return
        }
        
        isPoolMiningActive = true
        poolMiningStats = PoolMiningStats(
            totalParticipants: 1,
            combinedResearchPower: 1.0,
            blockWinChance: 0.01,
            lastBlockFound: nil,
            totalBlocksFound: 0
        )
        
        startMiningTimer()
    }
    
    func stopPoolMining() {
        isPoolMiningActive = false
        poolMiningStats = nil
        stopMiningTimer()
    }
    
    func startSoloMining() {
        guard !isPoolMiningActive else {
            error = "Pool Mining muss zuerst beendet werden"
            return
        }
        
        isSoloMiningActive = true
        soloMiningStats = SoloMiningStats(
            researchPower: 1.0,
            blockWinChance: 0.005,
            lastBlockFound: nil,
            totalBlocksFound: 0
        )
        
        startMiningTimer()
    }
    
    func stopSoloMining() {
        isSoloMiningActive = false
        soloMiningStats = nil
        stopMiningTimer()
    }
    
    private func startMiningTimer() {
        miningTimer = Timer.scheduledTimer(withTimeInterval: miningInterval, repeats: true) { [weak self] _ in
            self?.updateMiningStats()
        }
    }
    
    private func stopMiningTimer() {
        miningTimer?.invalidate()
        miningTimer = nil
    }
    
    private func updateMiningStats() {
        if isPoolMiningActive {
            updatePoolMiningStats()
        } else if isSoloMiningActive {
            updateSoloMiningStats()
        }
    }
    
    private func updatePoolMiningStats() {
        guard var stats = poolMiningStats else { return }
        
        // Simuliere zufällige Teilnehmer und Forschungskraft
        stats.totalParticipants = Int.random(in: 1...100)
        stats.combinedResearchPower = Double(stats.totalParticipants) * Double.random(in: 0.8...1.2)
        stats.blockWinChance = min(stats.combinedResearchPower * 0.01, 0.5)
        
        // Simuliere Block-Fund
        if Double.random(in: 0...1) < stats.blockWinChance {
            stats.lastBlockFound = Date()
            stats.totalBlocksFound += 1
        }
        
        DispatchQueue.main.async {
            self.poolMiningStats = stats
        }
    }
    
    private func updateSoloMiningStats() {
        guard var stats = soloMiningStats else { return }
        
        // Simuliere Forschungskraft
        stats.researchPower = Double.random(in: 0.8...1.2)
        stats.blockWinChance = stats.researchPower * 0.005
        
        // Simuliere Block-Fund
        if Double.random(in: 0...1) < stats.blockWinChance {
            stats.lastBlockFound = Date()
            stats.totalBlocksFound += 1
        }
        
        DispatchQueue.main.async {
            self.soloMiningStats = stats
        }
    }
} 