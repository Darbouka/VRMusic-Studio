import SwiftUI
import Foundation

struct LeaderboardView: View {
    @EnvironmentObject var appState: AppState
    
    var body: some View {
        List {
            ForEach(appState.leaderboardEntries) { entry in
                LeaderboardRow(entry: entry)
            }
        }
        .listStyle(PlainListStyle())
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
            
            VStack(alignment: .leading) {
                Text(entry.username)
                    .font(.headline)
                
                Text("Score: \(entry.score)")
                    .font(.subheadline)
                    .foregroundColor(.secondary)
            }
            
            Spacer()
            
            Image(systemName: "trophy.fill")
                .foregroundColor(entry.rank <= 3 ? .yellow : .gray)
        }
        .padding(.vertical, 8)
    }
} 