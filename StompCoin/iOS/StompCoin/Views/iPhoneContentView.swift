import SwiftUI

struct iPhoneContentView: View {
    @EnvironmentObject var appState: AppState
    @State private var selectedTab = 0
    
    var body: some View {
        TabView(selection: $selectedTab) {
            HomeView()
                .tabItem {
                    Label("Startseite", systemImage: "house.fill")
                }
                .tag(0)
            
            MusicGenreListView()
                .tabItem {
                    Label("Musik", systemImage: "music.note")
                }
                .tag(1)
            
            LeaderboardView()
                .tabItem {
                    Label("Rangliste", systemImage: "trophy.fill")
                }
                .tag(2)
            
            SettingsView()
                .tabItem {
                    Label("Einstellungen", systemImage: "gear")
                }
                .tag(3)
        }
    }
}

struct iPhoneHomeView: View {
    @EnvironmentObject var appState: AppState
    
    var body: some View {
        ScrollView {
            VStack(spacing: 20) {
                // Statistik-Karten
                LazyVGrid(columns: [
                    GridItem(.flexible()),
                    GridItem(.flexible())
                ], spacing: 15) {
                    StatCard(title: "Level", value: "\(appState.currentUser?.level ?? 1)", icon: "star.fill", color: .yellow)
                    StatCard(title: "Wallet", value: "\(appState.currentUser?.walletBalance ?? 0)", icon: "dollarsign.circle.fill", color: .green)
                    StatCard(title: "XP", value: "\(appState.currentUser?.xp ?? 0)", icon: "bolt.fill", color: .blue)
                }
                .padding(.horizontal)
                
                // Fitness-Karte
                FitnessCard()
                    .padding(.horizontal)
                
                // Aktuelle Aktivität
                VStack(alignment: .leading) {
                    Text("Aktuelle Aktivität")
                        .font(.title2)
                        .bold()
                        .padding(.horizontal)
                    
                    ScrollView(.horizontal, showsIndicators: false) {
                        LazyHGrid(rows: [GridItem(.fixed(180))], spacing: 15) {
                            ForEach(appState.musicGenres) { genre in
                                MusicGenreCard(genre: genre)
                            }
                        }
                        .padding(.horizontal)
                    }
                }
            }
            .padding(.vertical)
        }
    }
}

// Wiederverwendung der bestehenden Komponenten
struct StatCard: View {
    let title: String
    let value: String
    let icon: String
    let color: Color
    
    var body: some View {
        VStack {
            Image(systemName: icon)
                .font(.title2)
                .foregroundColor(color)
            Text(value)
                .font(.title3)
                .bold()
            Text(title)
                .font(.caption)
                .foregroundColor(.secondary)
        }
        .frame(maxWidth: .infinity)
        .padding()
        .background(Color(.systemBackground))
        .cornerRadius(10)
        .shadow(radius: 2)
    }
}

struct FitnessCard: View {
    var body: some View {
        VStack(alignment: .leading, spacing: 15) {
            Text("Fitness")
                .font(.title2)
                .bold()
            
            HStack {
                StatRow(title: "Schritte", value: "8,547", icon: "figure.walk")
                StatRow(title: "Kalorien", value: "320", icon: "flame.fill")
                StatRow(title: "Distanz", value: "5.2 km", icon: "map.fill")
            }
        }
        .padding()
        .background(Color(.systemBackground))
        .cornerRadius(10)
        .shadow(radius: 2)
    }
}

struct StatRow: View {
    let title: String
    let value: String
    let icon: String
    
    var body: some View {
        VStack {
            Image(systemName: icon)
                .font(.title3)
            Text(value)
                .font(.headline)
            Text(title)
                .font(.caption)
                .foregroundColor(.secondary)
        }
        .frame(maxWidth: .infinity)
    }
}

struct MusicGenreCard: View {
    let genre: MusicGenre
    
    var body: some View {
        VStack(alignment: .leading) {
            Image(systemName: genre.icon)
                .font(.title)
                .padding(.bottom, 5)
            
            Text(genre.name)
                .font(.headline)
            
            Text(genre.description)
                .font(.caption)
                .foregroundColor(.secondary)
            
            Text("\(genre.floors.count) Floors")
                .font(.caption2)
                .padding(.top, 5)
        }
        .padding()
        .frame(width: 160)
        .background(Color(.systemBackground))
        .cornerRadius(10)
        .shadow(radius: 2)
    }
} 