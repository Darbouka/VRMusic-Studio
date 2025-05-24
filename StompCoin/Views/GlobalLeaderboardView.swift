import SwiftUI
import SwiftData

struct GlobalLeaderboardView: View {
    @Query private var leaderboard: [Leaderboard]
    @State private var selectedTimeFrame: TimeFrame = .daily
    @State private var selectedCategory: LeaderboardCategory = .steps
    
    enum LeaderboardCategory: String, CaseIterable {
        case steps = "Schritte"
        case coins = "Coins"
        case events = "Events"
    }
    
    var body: some View {
        NavigationView {
            VStack(spacing: 20) {
                // Zeitraum-Auswahl
                Picker("Zeitraum", selection: $selectedTimeFrame) {
                    ForEach([TimeFrame.daily, .weekly, .monthly, .allTime], id: \.self) { timeFrame in
                        Text(timeFrame.rawValue).tag(timeFrame)
                    }
                }
                .pickerStyle(SegmentedPickerStyle())
                .padding(.horizontal)
                
                // Kategorie-Auswahl
                Picker("Kategorie", selection: $selectedCategory) {
                    ForEach(LeaderboardCategory.allCases, id: \.self) { category in
                        Text(category.rawValue).tag(category)
                    }
                }
                .pickerStyle(SegmentedPickerStyle())
                .padding(.horizontal)
                
                // Top 3
                HStack(spacing: 20) {
                    ForEach(0..<3) { position in
                        TopThreeView(
                            position: position + 1,
                            entry: getTopEntry(for: position + 1)
                        )
                    }
                }
                .padding()
                
                // Restliche Rangliste
                ScrollView {
                    VStack(spacing: 10) {
                        ForEach(getRemainingEntries()) { entry in
                            LeaderboardRow(entry: entry)
                        }
                    }
                    .padding()
                }
            }
            .navigationTitle("Globale Rangliste")
        }
    }
    
    private func getTopEntry(for position: Int) -> LeaderboardEntry? {
        let entries = leaderboard
            .filter { $0.timeFrame == selectedTimeFrame }
            .flatMap { $0.entries }
            .sorted { sortEntries($0, $1) }
        
        return entries.count >= position ? entries[position - 1] : nil
    }
    
    private func getRemainingEntries() -> [LeaderboardEntry] {
        let entries = leaderboard
            .filter { $0.timeFrame == selectedTimeFrame }
            .flatMap { $0.entries }
            .sorted { sortEntries($0, $1) }
        
        return Array(entries.dropFirst(3))
    }
    
    private func sortEntries(_ first: LeaderboardEntry, _ second: LeaderboardEntry) -> Bool {
        switch selectedCategory {
        case .steps:
            return first.steps > second.steps
        case .coins:
            return first.coins > second.coins
        case .events:
            // Hier könnte man die Anzahl der gewonnenen Events vergleichen
            return first.steps > second.steps
        }
    }
}

struct TopThreeView: View {
    let position: Int
    let entry: LeaderboardEntry?
    
    var body: some View {
        VStack {
            if let entry = entry {
                ZStack {
                    Circle()
                        .fill(positionColor)
                        .frame(width: 80, height: 80)
                    
                    Text("#\(position)")
                        .font(.title)
                        .fontWeight(.bold)
                        .foregroundColor(.white)
                }
                
                Text(entry.user.username)
                    .font(.headline)
                    .lineLimit(1)
                
                Text("\(entry.steps) Schritte")
                    .font(.caption)
                    .foregroundColor(.secondary)
                
                Text("\(entry.coins) Coins")
                    .font(.caption)
                    .foregroundColor(.green)
            } else {
                ZStack {
                    Circle()
                        .fill(Color.gray.opacity(0.3))
                        .frame(width: 80, height: 80)
                    
                    Text("#\(position)")
                        .font(.title)
                        .fontWeight(.bold)
                        .foregroundColor(.gray)
                }
                
                Text("Platzhalter")
                    .font(.headline)
                    .foregroundColor(.gray)
            }
        }
    }
    
    private var positionColor: Color {
        switch position {
        case 1: return .yellow
        case 2: return .gray
        case 3: return .orange
        default: return .blue
        }
    }
}

struct LeaderboardRow: View {
    let entry: LeaderboardEntry
    
    var body: some View {
        HStack {
            Text("#\(entry.rank)")
                .font(.headline)
                .foregroundColor(.blue)
                .frame(width: 40)
            
            Text(entry.user.username)
                .font(.subheadline)
            
            Spacer()
            
            VStack(alignment: .trailing) {
                Text("\(entry.steps) Schritte")
                    .font(.caption)
                Text("\(entry.coins) Coins")
                    .font(.caption)
                    .foregroundColor(.green)
            }
        }
        .padding()
        .background(Color.secondary.opacity(0.1))
        .cornerRadius(10)
    }
}

#Preview {
    GlobalLeaderboardView()
} 