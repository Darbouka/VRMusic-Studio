import SwiftUI

struct LeaderboardView: View {
    @EnvironmentObject var appState: AppState
    @State private var selectedTimeFrame: TimeFrame = .week
    
    enum TimeFrame: String, CaseIterable {
        case day = "Tag"
        case week = "Woche"
        case month = "Monat"
        case total = "Gesamt"
    }
    
    var body: some View {
        NavigationView {
            VStack(spacing: 20) {
                // Zeitraum-Auswahl
                HStack(spacing: 20) {
                    ForEach(TimeFrame.allCases, id: \.self) { timeFrame in
                        Button(action: {
                            selectedTimeFrame = timeFrame
                        }) {
                            Text(timeFrame.rawValue)
                                .padding(.horizontal, 16)
                                .padding(.vertical, 8)
                                .background(selectedTimeFrame == timeFrame ? Color.blue : Color.clear)
                                .foregroundColor(selectedTimeFrame == timeFrame ? .white : .primary)
                                .cornerRadius(8)
                        }
                        .buttonStyle(PlainButtonStyle())
                    }
                }
                .padding(.top)
                
                // Rangliste
                ScrollView {
                    LazyVStack(spacing: 12) {
                        ForEach(appState.leaderboardEntries) { entry in
                            LeaderboardRow(
                                rank: entry.rank,
                                username: entry.username,
                                score: entry.score,
                                color: entry.rank <= 3 ? [.yellow, .gray, .orange][entry.rank - 1] : .gray
                            )
                        }
                    }
                    .padding()
                }
            }
            .navigationTitle("Rangliste")
        }
    }
}

struct LeaderboardRow: View {
    let rank: Int
    let username: String
    let score: Int
    let color: Color
    
    var body: some View {
        HStack(spacing: 16) {
            // Rang
            Text("#\(rank)")
                .font(.headline)
                .foregroundColor(.secondary)
                .frame(width: 40)
            
            // Profilbild
            Image(systemName: "person.circle.fill")
                .resizable()
                .aspectRatio(contentMode: .fill)
                .frame(width: 40, height: 40)
                .clipShape(Circle())
            
            // Benutzername
            Text(username)
                .font(.headline)
            
            Spacer()
            
            // Punktzahl
            Text("\(score)")
                .font(.title3)
                .bold()
                .foregroundColor(color)
        }
        .padding()
        .background(Color(.windowBackgroundColor))
        .cornerRadius(12)
    }
}

struct LeaderboardView_Previews: PreviewProvider {
    static var previews: some View {
        LeaderboardView()
            .environmentObject(AppState())
    }
} 