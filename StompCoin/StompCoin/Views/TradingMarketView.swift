import SwiftUI

struct TradingMarketView: View {
    @StateObject private var marketManager = TradingMarketManager.shared
    @State private var showingMiningInfo = false
    
    var body: some View {
        VStack(spacing: 20) {
            // Mining-Status
            VStack(spacing: 15) {
                Text("Mining-Status")
                    .font(.headline)
                
                HStack(spacing: 20) {
                    MiningButton(
                        title: "Pool Mining",
                        isActive: marketManager.isPoolMiningActive,
                        action: {
                            if marketManager.isPoolMiningActive {
                                marketManager.stopPoolMining()
                            } else {
                                marketManager.startPoolMining()
                            }
                        }
                    )
                    
                    MiningButton(
                        title: "Solo Mining",
                        isActive: marketManager.isSoloMiningActive,
                        action: {
                            if marketManager.isSoloMiningActive {
                                marketManager.stopSoloMining()
                            } else {
                                marketManager.startSoloMining()
                            }
                        }
                    )
                }
            }
            .padding()
            .background(Color.secondary.opacity(0.1))
            .cornerRadius(15)
            
            // Mining-Statistiken
            if marketManager.isPoolMiningActive, let stats = marketManager.poolMiningStats {
                PoolMiningStatsView(stats: stats)
            } else if marketManager.isSoloMiningActive, let stats = marketManager.soloMiningStats {
                SoloMiningStatsView(stats: stats)
            }
            
            // Info-Button
            Button(action: {
                showingMiningInfo = true
            }) {
                Label("Mining-Informationen", systemImage: "info.circle")
                    .font(.headline)
                    .foregroundColor(.white)
                    .frame(maxWidth: .infinity)
                    .padding()
                    .background(Color.blue)
                    .cornerRadius(10)
            }
            
            if let error = marketManager.error {
                Text(error)
                    .foregroundColor(.red)
                    .padding()
            }
        }
        .padding()
        .sheet(isPresented: $showingMiningInfo) {
            MiningInfoView()
        }
    }
}

struct MiningButton: View {
    let title: String
    let isActive: Bool
    let action: () -> Void
    
    var body: some View {
        Button(action: action) {
            Text(title)
                .font(.headline)
                .foregroundColor(.white)
                .frame(maxWidth: .infinity)
                .padding()
                .background(isActive ? Color.green : Color.gray)
                .cornerRadius(10)
        }
    }
}

struct PoolMiningStatsView: View {
    let stats: TradingMarketManager.PoolMiningStats
    
    var body: some View {
        VStack(alignment: .leading, spacing: 10) {
            Text("Pool Mining Statistiken")
                .font(.headline)
            
            StatRow(title: "Teilnehmer", value: "\(stats.totalParticipants)")
            StatRow(title: "Kombinierte Forschungskraft", value: String(format: "%.2f", stats.combinedResearchPower))
            StatRow(title: "Block-Gewinnchance", value: String(format: "%.2f%%", stats.blockWinChance * 100))
            StatRow(title: "Gefundene Blöcke", value: "\(stats.totalBlocksFound)")
            
            if let lastBlock = stats.lastBlockFound {
                StatRow(title: "Letzter Block", value: lastBlock.formatted())
            }
        }
        .padding()
        .background(Color.blue.opacity(0.1))
        .cornerRadius(15)
    }
}

struct SoloMiningStatsView: View {
    let stats: TradingMarketManager.SoloMiningStats
    
    var body: some View {
        VStack(alignment: .leading, spacing: 10) {
            Text("Solo Mining Statistiken")
                .font(.headline)
            
            StatRow(title: "Forschungskraft", value: String(format: "%.2f", stats.researchPower))
            StatRow(title: "Block-Gewinnchance", value: String(format: "%.2f%%", stats.blockWinChance * 100))
            StatRow(title: "Gefundene Blöcke", value: "\(stats.totalBlocksFound)")
            
            if let lastBlock = stats.lastBlockFound {
                StatRow(title: "Letzter Block", value: lastBlock.formatted())
            }
        }
        .padding()
        .background(Color.orange.opacity(0.1))
        .cornerRadius(15)
    }
}

struct StatRow: View {
    let title: String
    let value: String
    
    var body: some View {
        HStack {
            Text(title)
                .foregroundColor(.secondary)
            Spacer()
            Text(value)
                .fontWeight(.semibold)
        }
    }
}

struct MiningInfoView: View {
    @Environment(\.dismiss) private var dismiss
    
    var body: some View {
        NavigationView {
            ScrollView {
                VStack(alignment: .leading, spacing: 20) {
                    InfoSection(
                        title: "Pool Mining",
                        description: "Beim Pool Mining schließen sich mehrere Nutzer zusammen, um ihre Forschungskraft zu bündeln. Dies erhöht die Chance auf Block-Gewinne, die dann unter allen Teilnehmern aufgeteilt werden."
                    )
                    
                    InfoSection(
                        title: "Solo Mining",
                        description: "Beim Solo Mining arbeitet man alleine. Die Gewinnchancen sind geringer, aber wenn ein Block gefunden wird, erhält man den gesamten Gewinn."
                    )
                    
                    InfoSection(
                        title: "Forschungskraft",
                        description: "Die Forschungskraft bestimmt die Wahrscheinlichkeit, einen Block zu finden. Sie wird durch aktive Teilnahme an Dance Challenges und Events erhöht."
                    )
                }
                .padding()
            }
            .navigationTitle("Mining-Informationen")
            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                ToolbarItem(placement: .navigationBarTrailing) {
                    Button("Fertig") {
                        dismiss()
                    }
                }
            }
        }
    }
}

struct InfoSection: View {
    let title: String
    let description: String
    
    var body: some View {
        VStack(alignment: .leading, spacing: 10) {
            Text(title)
                .font(.headline)
            
            Text(description)
                .foregroundColor(.secondary)
        }
    }
}

#Preview {
    TradingMarketView()
} 