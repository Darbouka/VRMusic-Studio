import SwiftUI

struct LeaderboardView: View {
    @EnvironmentObject var appState: AppState
    @State private var selectedTimeFrame = "Woche"
    @State private var selectedCategory = "Gesamt"
    
    let timeFrames = ["Tag", "Woche", "Monat", "Jahr"]
    let categories = ["Gesamt", "Schritte", "Coins", "Floors"]
    
    var body: some View {
        NavigationView {
            // Sidebar mit Filter-Optionen
            VStack {
                // Zeit-Filter
                VStack(alignment: .leading) {
                    Text("Zeitraum")
                        .font(.headline)
                        .padding(.horizontal)
                    
                    Picker("Zeitraum", selection: $selectedTimeFrame) {
                        ForEach(timeFrames, id: \.self) { timeFrame in
                            Text(timeFrame).tag(timeFrame)
                        }
                    }
                    .pickerStyle(SegmentedPickerStyle())
                    .padding(.horizontal)
                }
                .padding(.vertical)
                
                // Kategorie-Filter
                VStack(alignment: .leading) {
                    Text("Kategorie")
                        .font(.headline)
                        .padding(.horizontal)
                    
                    Picker("Kategorie", selection: $selectedCategory) {
                        ForEach(categories, id: \.self) { category in
                            Text(category).tag(category)
                        }
                    }
                    .pickerStyle(SegmentedPickerStyle())
                    .padding(.horizontal)
                }
                .padding(.bottom)
                
                Spacer()
            }
            .frame(minWidth: 250)
            .background(Color(.systemBackground))
            
            // Hauptansicht mit Rangliste
            ScrollView {
                VStack(spacing: 20) {
                    // Top 3
                    HStack(spacing: 20) {
                        ForEach(0..<min(3, appState.leaderboardEntries.count), id: \.self) { index in
                            TopThreeCard(entry: appState.leaderboardEntries[index], rank: index + 1)
                        }
                    }
                    .padding(.horizontal)
                    
                    // Rest der Rangliste
                    VStack(spacing: 10) {
                        ForEach(3..<appState.leaderboardEntries.count, id: \.self) { index in
                            LeaderboardRow(entry: appState.leaderboardEntries[index], rank: index + 1)
                        }
                    }
                    .padding(.horizontal)
                }
                .padding(.vertical)
            }
        }
        .navigationViewStyle(DoubleColumnNavigationViewStyle())
    }
}

struct TopThreeCard: View {
    let entry: LeaderboardEntry
    let rank: Int
    
    var body: some View {
        VStack {
            // Rang
            Text("#\(rank)")
                .font(.title)
                .bold()
                .foregroundColor(rankColor)
            
            // Profilbild
            Image(systemName: "person.circle.fill")
                .font(.system(size: 60))
                .foregroundColor(.accentColor)
            
            // Benutzername
            Text(entry.username)
                .font(.headline)
            
            // Punktzahl
            Text("\(entry.score) Punkte")
                .font(.subheadline)
                .foregroundColor(.secondary)
        }
        .frame(maxWidth: .infinity)
        .padding()
        .background(Color(.systemBackground))
        .cornerRadius(10)
        .shadow(radius: 2)
    }
    
    var rankColor: Color {
        switch rank {
        case 1: return .yellow
        case 2: return .gray
        case 3: return .orange
        default: return .primary
        }
    }
}

struct LeaderboardRow: View {
    let entry: LeaderboardEntry
    let rank: Int
    
    var body: some View {
        HStack {
            // Rang
            Text("#\(rank)")
                .font(.headline)
                .foregroundColor(.secondary)
                .frame(width: 50)
            
            // Profilbild
            Image(systemName: "person.circle.fill")
                .font(.title2)
                .foregroundColor(.accentColor)
            
            // Benutzername
            Text(entry.username)
                .font(.body)
            
            Spacer()
            
            // Punktzahl
            Text("\(entry.score) Punkte")
                .font(.subheadline)
                .foregroundColor(.secondary)
        }
        .padding()
        .background(Color(.systemBackground))
        .cornerRadius(10)
        .shadow(radius: 1)
    }
} 