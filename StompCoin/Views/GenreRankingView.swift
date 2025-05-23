import SwiftUI
import SwiftData

struct GenreRankingView: View {
    @Environment(\.modelContext) private var modelContext
    @Environment(\.dismiss) private var dismiss
    let genre: MusicGenre
    @State private var selectedTab = 0
    
    var body: some View {
        NavigationView {
            VStack {
                // Genre-Header
                VStack(spacing: 8) {
                    Text(genre.rawValue.capitalized)
                        .font(.title)
                        .bold()
                    
                    Text(genre.category.rawValue)
                        .font(.subheadline)
                        .foregroundColor(.secondary)
                }
                .padding()
                
                // Ranglisten-Tabs
                Picker("Rangliste", selection: $selectedTab) {
                    Text("International").tag(0)
                    Text("National").tag(1)
                    Text("Lokal").tag(2)
                }
                .pickerStyle(.segmented)
                .padding(.horizontal)
                
                // Ranglisten-Inhalt
                TabView(selection: $selectedTab) {
                    RankingListView(rankings: getRankings(for: .international))
                        .tag(0)
                    
                    RankingListView(rankings: getRankings(for: .national))
                        .tag(1)
                    
                    RankingListView(rankings: getRankings(for: .local))
                        .tag(2)
                }
                .tabViewStyle(.page(indexDisplayMode: .never))
            }
            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                ToolbarItem(placement: .navigationBarTrailing) {
                    Button("Schließen") {
                        dismiss()
                    }
                }
            }
        }
    }
    
    private func getRankings(for type: RankingType) -> [UserRanking] {
        let descriptor = FetchDescriptor<GenreRanking>(
            predicate: #Predicate<GenreRanking> { $0.genre == genre }
        )
        
        guard let ranking = try? modelContext.fetch(descriptor).first else {
            return []
        }
        
        switch type {
        case .international:
            return ranking.internationalRankings
        case .national:
            return ranking.nationalRankings
        case .local:
            return ranking.localRankings
        }
    }
}

enum RankingType {
    case international
    case national
    case local
}

struct RankingListView: View {
    let rankings: [UserRanking]
    
    var body: some View {
        List {
            ForEach(rankings) { ranking in
                RankingRow(ranking: ranking)
            }
        }
    }
}

struct RankingRow: View {
    let ranking: UserRanking
    
    var body: some View {
        HStack {
            // Rang
            Text("#\(ranking.rank)")
                .font(.headline)
                .frame(width: 40)
            
            // Benutzer
            VStack(alignment: .leading) {
                Text(ranking.user.username)
                    .font(.subheadline)
                    .bold()
                
                Text("\(ranking.challengesWon) Challenges gewonnen")
                    .font(.caption)
                    .foregroundColor(.secondary)
            }
            
            Spacer()
            
            // StompCoins
            VStack(alignment: .trailing) {
                Text("\(Int(ranking.stompCoins))")
                    .font(.headline)
                    .bold()
                
                Text("StompCoins")
                    .font(.caption)
                    .foregroundColor(.secondary)
            }
        }
        .padding(.vertical, 4)
    }
}

struct GenreToggleView: View {
    @Environment(\.modelContext) private var modelContext
    let user: User
    let genre: MusicGenre
    @State private var isActive: Bool
    
    init(user: User, genre: MusicGenre) {
        self.user = user
        self.genre = genre
        self._isActive = State(initialValue: user.activeMusicGenres.contains(genre))
    }
    
    var body: some View {
        Toggle(isOn: $isActive) {
            VStack(alignment: .leading) {
                Text(genre.rawValue.capitalized)
                    .font(.headline)
                
                Text(genre.category.rawValue)
                    .font(.caption)
                    .foregroundColor(.secondary)
            }
        }
        .onChange(of: isActive) { _, newValue in
            if newValue {
                if user.canActivateMoreGenres {
                    _ = user.toggleGenre(genre)
                } else {
                    isActive = false
                }
            } else {
                _ = user.toggleGenre(genre)
            }
            try? modelContext.save()
        }
    }
}

#Preview {
    GenreRankingView(genre: .psytrance)
        .modelContainer(for: [GenreRanking.self, UserRanking.self, User.self], inMemory: true)
} 