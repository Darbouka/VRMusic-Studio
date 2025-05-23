import SwiftUI
import SwiftData

struct MusicGenresView: View {
    @Environment(\.modelContext) private var modelContext
    @State private var selectedGenre: MusicGenre?
    @State private var showingGenreDetail = false
    @State private var selectedFloor: MusicGenreCategory = .goaFloor
    
    var body: some View {
        NavigationView {
            VStack(spacing: 0) {
                // Floor-Auswahl
                Picker("Floor", selection: $selectedFloor) {
                    Text("Goa Floor").tag(MusicGenreCategory.goaFloor)
                    Text("Techno Floor").tag(MusicGenreCategory.technoFloor)
                }
                .pickerStyle(SegmentedPickerStyle())
                .padding()
                
                // Genre-Grid
                ScrollView {
                    LazyVGrid(columns: [
                        GridItem(.flexible()),
                        GridItem(.flexible())
                    ], spacing: 20) {
                        ForEach(filteredGenres) { genre in
                            GenreCard(genre: genre)
                                .onTapGesture {
                                    selectedGenre = genre
                                    showingGenreDetail = true
                                }
                        }
                    }
                    .padding()
                }
            }
            .navigationTitle("Musikgenres")
            .sheet(isPresented: $showingGenreDetail) {
                if let genre = selectedGenre {
                    GenreDetailView(genre: genre)
                }
            }
        }
    }
    
    private var filteredGenres: [MusicGenre] {
        MusicGenre.allCases.filter { $0.category == selectedFloor }
    }
}

struct GenreCard: View {
    let genre: MusicGenre
    
    var body: some View {
        VStack {
            Image(systemName: genre.icon)
                .font(.system(size: 40))
                .foregroundColor(.blue)
                .padding()
            
            Text(genre.displayName)
                .font(.headline)
            
            Text("\(genre.activeUsers) aktive Nutzer")
                .font(.caption)
                .foregroundColor(.secondary)
            
            Text("\(genre.totalSteps) Schritte")
                .font(.caption)
                .foregroundColor(.secondary)
        }
        .padding()
        .background(Color.secondary.opacity(0.1))
        .cornerRadius(15)
    }
}

struct GenreDetailView: View {
    let genre: MusicGenre
    @Environment(\.dismiss) private var dismiss
    @State private var selectedTimeFrame: TimeFrame = .daily
    
    var body: some View {
        NavigationView {
            ScrollView {
                VStack(spacing: 20) {
                    // Genre Header
                    VStack {
                        Image(systemName: genre.icon)
                            .font(.system(size: 60))
                            .foregroundColor(.blue)
                        
                        Text(genre.displayName)
                            .font(.title)
                            .fontWeight(.bold)
                        
                        Text(genre.description)
                            .foregroundColor(.secondary)
                            .multilineTextAlignment(.center)
                            .padding()
                    }
                    
                    // Statistiken
                    HStack(spacing: 20) {
                        StatBox(title: "Aktive Nutzer", value: "\(genre.activeUsers)")
                        StatBox(title: "Gesamte Schritte", value: "\(genre.totalSteps)")
                    }
                    
                    // Rangliste
                    VStack(alignment: .leading) {
                        Text("Rangliste")
                            .font(.headline)
                            .padding(.horizontal)
                        
                        Picker("Zeitraum", selection: $selectedTimeFrame) {
                            ForEach([TimeFrame.daily, .weekly, .monthly, .allTime], id: \.self) { timeFrame in
                                Text(timeFrame.rawValue).tag(timeFrame)
                            }
                        }
                        .pickerStyle(SegmentedPickerStyle())
                        .padding()
                        
                        LeaderboardView(genre: genre, timeFrame: selectedTimeFrame)
                    }
                }
                .padding()
            }
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

struct StatBox: View {
    let title: String
    let value: String
    
    var body: some View {
        VStack {
            Text(title)
                .font(.caption)
                .foregroundColor(.secondary)
            
            Text(value)
                .font(.title2)
                .fontWeight(.bold)
        }
        .frame(maxWidth: .infinity)
        .padding()
        .background(Color.secondary.opacity(0.1))
        .cornerRadius(10)
    }
}

struct LeaderboardView: View {
    let genre: MusicGenre
    let timeFrame: TimeFrame
    @Query private var leaderboard: [Leaderboard]
    
    var body: some View {
        VStack(spacing: 10) {
            ForEach(leaderboard.filter { $0.genre?.id == genre.id && $0.timeFrame == timeFrame }) { board in
                ForEach(board.entries.sorted { $0.rank < $1.rank }) { entry in
                    LeaderboardRow(entry: entry)
                }
            }
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
    MusicGenresView()
} 