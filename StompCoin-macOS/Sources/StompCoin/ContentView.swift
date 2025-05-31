import SwiftUI

struct ContentView: View {
    @EnvironmentObject var appState: AppState
    @State private var selectedTab = 0
    @State private var showMusicPlayer = false
    
    var body: some View {
        ZStack {
            // Hintergrund
            Image("background_main")
                .resizable()
                .aspectRatio(contentMode: .fill)
                .edgesIgnoringSafeArea(.all)
            
            if appState.isLoggedIn {
                VStack(spacing: 0) {
                    // Top Bar
                    TopBarView(showMusicPlayer: $showMusicPlayer)
                    
                    // Hauptinhalt
                    TabView(selection: $selectedTab) {
                        DashboardView()
                            .tabItem {
                                Label("Dashboard", systemImage: "chart.bar.fill")
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
                        
                        WalletView()
                            .tabItem {
                                Label("Wallet", systemImage: "creditcard.fill")
                            }
                            .tag(3)
                        
                        SettingsView()
                            .tabItem {
                                Label("Einstellungen", systemImage: "gear")
                            }
                            .tag(4)
                    }
                    .frame(minWidth: 800, minHeight: 600)
                }
            } else {
                LoginView()
                    .frame(width: 400, height: 500)
            }
            
            // Musik-Player Overlay
            if showMusicPlayer {
                MusicPlayerView(isShowing: $showMusicPlayer)
                    .transition(.move(edge: .bottom))
            }
        }
    }
}

struct TopBarView: View {
    @EnvironmentObject var appState: AppState
    @Binding var showMusicPlayer: Bool
    
    var body: some View {
        HStack {
            // Logo
            Image("logo")
                .resizable()
                .scaledToFit()
                .frame(height: 40)
            
            Spacer()
            
            // Wallet Balance
            HStack {
                Image(systemName: "creditcard.fill")
                    .foregroundColor(.green)
                Text("\(String(format: "%.2f", appState.walletBalance)) STOMP")
                    .font(.headline)
                    .foregroundColor(.white)
            }
            .padding(.horizontal)
            .background(Color.black.opacity(0.5))
            .cornerRadius(20)
            
            // Musik-Player Toggle
            Button(action: {
                withAnimation {
                    showMusicPlayer.toggle()
                }
            }) {
                Image(systemName: "music.note")
                    .font(.title2)
                    .foregroundColor(.white)
                    .padding(10)
                    .background(Color.black.opacity(0.5))
                    .clipShape(Circle())
            }
            
            // Benutzer-Menü
            Menu {
                Button(action: {
                    appState.logout()
                }) {
                    Label("Abmelden", systemImage: "rectangle.portrait.and.arrow.right")
                }
            } label: {
                Image(systemName: "person.circle.fill")
                    .font(.title2)
                    .foregroundColor(.white)
            }
        }
        .padding()
        .background(Color.black.opacity(0.3))
    }
}

struct MusicPlayerView: View {
    @Binding var isShowing: Bool
    @State private var isPlaying = false
    @State private var currentTime: Double = 0
    @State private var volume: Double = 0.8
    
    var body: some View {
        VStack(spacing: 20) {
            // Titel
            Text("Aktuelle Musik")
                .font(.title2)
                .bold()
            
            // Album-Cover
            Image("album_cover")
                .resizable()
                .scaledToFit()
                .frame(width: 200, height: 200)
                .cornerRadius(10)
            
            // Song-Info
            VStack(spacing: 5) {
                Text("Song Titel")
                    .font(.headline)
                Text("Künstler")
                    .foregroundColor(.gray)
            }
            
            // Progress Bar
            Slider(value: $currentTime, in: 0...100)
                .padding(.horizontal)
            
            // Zeit
            HStack {
                Text("0:00")
                Spacer()
                Text("3:45")
            }
            .padding(.horizontal)
            
            // Steuerung
            HStack(spacing: 30) {
                Button(action: {}) {
                    Image(systemName: "backward.fill")
                        .font(.title)
                }
                
                Button(action: {
                    isPlaying.toggle()
                }) {
                    Image(systemName: isPlaying ? "pause.fill" : "play.fill")
                        .font(.title)
                }
                
                Button(action: {}) {
                    Image(systemName: "forward.fill")
                        .font(.title)
                }
            }
            
            // Lautstärke
            HStack {
                Image(systemName: "speaker.fill")
                Slider(value: $volume, in: 0...1)
                Image(systemName: "speaker.wave.3.fill")
            }
            .padding(.horizontal)
            
            // Schließen-Button
            Button(action: {
                withAnimation {
                    isShowing = false
                }
            }) {
                Image(systemName: "xmark.circle.fill")
                    .font(.title)
                    .foregroundColor(.gray)
            }
        }
        .padding()
        .background(Color(.windowBackgroundColor))
        .cornerRadius(20)
        .shadow(radius: 10)
    }
}

struct DashboardView: View {
    @EnvironmentObject var appState: AppState
    
    var body: some View {
        ScrollView {
            VStack(spacing: 25) {
                // Header
                HStack {
                    VStack(alignment: .leading) {
                        Text("Willkommen, \(appState.currentUser?.username ?? "")")
                            .font(.title)
                            .bold()
                        Text("Dein Fitness-Dashboard")
                            .font(.subheadline)
                            .foregroundColor(.gray)
                    }
                    Spacer()
                }
                .padding(.horizontal)
                
                // Stats Grid
                LazyVGrid(columns: [GridItem(.flexible()), GridItem(.flexible()), GridItem(.flexible())], spacing: 20) {
                    StatCard(title: "Schritte", value: "\(appState.fitnessData?.steps ?? 0)", icon: "figure.walk", color: .blue)
                    StatCard(title: "Kalorien", value: "\(Int(appState.fitnessData?.calories ?? 0))", icon: "flame.fill", color: .orange)
                    StatCard(title: "Distanz", value: "\(String(format: "%.2f", appState.fitnessData?.distance ?? 0)) km", icon: "map.fill", color: .green)
                }
                .padding(.horizontal)
                
                // Wallet Card
                WalletCard(balance: appState.walletBalance)
                    .padding(.horizontal)
                
                // Recent Activity
                RecentActivityView()
                    .padding(.horizontal)
                
                // Music Genres Preview
                MusicGenresPreview()
                    .padding(.horizontal)
            }
            .padding(.vertical)
        }
    }
}

struct StatCard: View {
    let title: String
    let value: String
    let icon: String
    let color: Color
    
    var body: some View {
        VStack(spacing: 12) {
            Image(systemName: icon)
                .font(.system(size: 30))
                .foregroundColor(color)
            
            Text(title)
                .font(.headline)
                .foregroundColor(.gray)
            
            Text(value)
                .font(.system(size: 24, weight: .bold))
        }
        .frame(maxWidth: .infinity)
        .padding()
        .background(Color(.windowBackgroundColor))
        .cornerRadius(15)
        .shadow(radius: 5)
    }
}

struct WalletCard: View {
    let balance: Double
    
    var body: some View {
        VStack(spacing: 20) {
            HStack {
                Text("Wallet Balance")
                    .font(.headline)
                    .foregroundColor(.gray)
                Spacer()
                Image(systemName: "creditcard.fill")
                    .foregroundColor(.blue)
            }
            
            Text("\(String(format: "%.2f", balance)) STOMP")
                .font(.system(size: 36, weight: .bold))
                .foregroundColor(.green)
            
            HStack(spacing: 20) {
                Button(action: {}) {
                    Label("Senden", systemImage: "arrow.up.right")
                        .frame(maxWidth: .infinity)
                }
                .buttonStyle(.borderedProminent)
                
                Button(action: {}) {
                    Label("Empfangen", systemImage: "arrow.down.left")
                        .frame(maxWidth: .infinity)
                }
                .buttonStyle(.bordered)
            }
        }
        .padding()
        .background(Color(.windowBackgroundColor))
        .cornerRadius(15)
        .shadow(radius: 5)
    }
}

struct RecentActivityView: View {
    var body: some View {
        VStack(alignment: .leading, spacing: 15) {
            Text("Letzte Aktivitäten")
                .font(.title2)
                .bold()
            
            ForEach(0..<3) { _ in
                HStack {
                    Image(systemName: "figure.walk")
                        .font(.title2)
                        .foregroundColor(.blue)
                        .frame(width: 40)
                    
                    VStack(alignment: .leading, spacing: 4) {
                        Text("Tägliches Training")
                            .font(.headline)
                        Text("Vor 2 Stunden")
                            .font(.caption)
                            .foregroundColor(.gray)
                    }
                    
                    Spacer()
                    
                    Text("+50 STOMP")
                        .font(.headline)
                        .foregroundColor(.green)
                }
                .padding()
                .background(Color(.windowBackgroundColor))
                .cornerRadius(10)
            }
        }
    }
}

struct MusicGenresPreview: View {
    var body: some View {
        VStack(alignment: .leading, spacing: 15) {
            Text("Beliebte Musikgenres")
                .font(.title2)
                .bold()
            
            ScrollView(.horizontal, showsIndicators: false) {
                HStack(spacing: 15) {
                    ForEach(["Hip Hop", "House", "Techno", "Drum & Bass"], id: \.self) { genre in
                        GenrePreviewCard(name: genre)
                    }
                }
            }
        }
    }
}

struct GenrePreviewCard: View {
    let name: String
    
    var body: some View {
        VStack(spacing: 10) {
            Image(systemName: "music.note")
                .font(.system(size: 30))
                .foregroundColor(.blue)
            
            Text(name)
                .font(.headline)
        }
        .frame(width: 120, height: 120)
        .background(Color(.windowBackgroundColor))
        .cornerRadius(15)
    }
}

struct MusicGenreListView: View {
    @EnvironmentObject var appState: AppState
    
    var body: some View {
        ScrollView {
            LazyVGrid(columns: [GridItem(.adaptive(minimum: 300))], spacing: 20) {
                ForEach(appState.musicGenres) { genre in
                    NavigationLink(destination: MusicGenreView(genre: genre)) {
                        MusicGenreCard(genre: genre)
                    }
                }
            }
            .padding()
        }
    }
}

struct MusicGenreCard: View {
    let genre: MusicGenre
    
    var body: some View {
        VStack(spacing: 15) {
            Image(systemName: genre.icon)
                .font(.system(size: 40))
                .foregroundColor(.blue)
            
            Text(genre.name)
                .font(.title2)
                .bold()
            
            Text(genre.description)
                .multilineTextAlignment(.center)
                .foregroundColor(.gray)
            
            Text("\(genre.floors.count) Floors")
                .font(.caption)
                .foregroundColor(.blue)
        }
        .padding()
        .frame(maxWidth: .infinity)
        .background(Color(.windowBackgroundColor))
        .cornerRadius(15)
        .shadow(radius: 5)
    }
}

// Platzhalter-Views für die anderen Tabs
struct LeaderboardView: View {
    var body: some View {
        Text("Leaderboard View")
    }
}

struct WalletView: View {
    var body: some View {
        Text("Wallet View")
    }
}

struct SettingsView: View {
    var body: some View {
        Text("Settings View")
    }
} 