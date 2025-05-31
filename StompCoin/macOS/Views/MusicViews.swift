import SwiftUI

struct MusicGenreListView: View {
    @EnvironmentObject var appState: AppState
    let columns = [
        GridItem(.adaptive(minimum: 300, maximum: 400), spacing: 20)
    ]
    
    var body: some View {
        NavigationView {
            ScrollView {
                LazyVGrid(columns: columns, spacing: 20) {
                    ForEach(appState.musicGenres) { genre in
                        NavigationLink(destination: MusicGenreDetailView(genre: genre)) {
                            MusicGenreCard(
                                icon: genre.icon,
                                name: genre.name,
                                description: genre.description,
                                floorCount: genre.floors.count,
                                color: .blue
                            )
                        }
                        .buttonStyle(PlainButtonStyle())
                    }
                }
                .padding()
            }
            .navigationTitle("Musikgenres")
        }
    }
}

struct MusicGenreCard: View {
    let icon: String
    let name: String
    let description: String
    let floorCount: Int
    let color: Color
    
    var body: some View {
        VStack(alignment: .leading, spacing: 12) {
            Image(icon)
                .resizable()
                .aspectRatio(contentMode: .fill)
                .frame(height: 200)
                .clipped()
                .cornerRadius(12)
            
            VStack(alignment: .leading, spacing: 8) {
                Text(name)
                    .font(.title2)
                    .bold()
                
                Text(description)
                    .font(.subheadline)
                    .foregroundColor(.secondary)
                    .lineLimit(2)
                
                Text("\(floorCount) Floors")
                        .font(.caption)
                    .foregroundColor(color)
                }
            .padding(.horizontal)
            .padding(.bottom)
        }
        .background(Color(.windowBackgroundColor))
        .cornerRadius(12)
        .shadow(radius: 2)
    }
}

struct MusicGenreDetailView: View {
    let genre: MusicGenre
    @State private var selectedFloor: Floor?
    
    var body: some View {
        ScrollView {
            VStack(alignment: .leading, spacing: 20) {
                // Genre-Header
                VStack(alignment: .leading, spacing: 8) {
                    Text(genre.name)
                        .font(.largeTitle)
                        .bold()
                    
                    Text(genre.description)
                        .font(.title3)
                        .foregroundColor(.secondary)
                }
                .padding()
                    
                // Verfügbare Floors
                VStack(alignment: .leading, spacing: 16) {
                    Text("Verfügbare Floors")
                        .font(.title2)
                        .bold()
                        .padding(.horizontal)
                    
                    ForEach(genre.floors) { floor in
                        FloorCard(
                            image: floor.image,
                            name: floor.name,
                            description: floor.description,
                            requiredLevel: floor.requiredLevel,
                            reward: floor.reward,
                            color: .blue
                        )
                        }
                    }
            }
        }
        .navigationTitle(genre.name)
        .sheet(item: $selectedFloor) { floor in
                FloorDetailView(floor: floor)
        }
    }
}

struct FloorCard: View {
    let image: String
    let name: String
    let description: String
    let requiredLevel: Int
    let reward: Int
    let color: Color
    
    var body: some View {
        HStack(spacing: 16) {
            Image(image)
                .resizable()
                .aspectRatio(contentMode: .fill)
                .frame(width: 100, height: 100)
                .cornerRadius(8)
            
            VStack(alignment: .leading, spacing: 8) {
                Text(name)
                    .font(.headline)
                
                Text(description)
                    .font(.subheadline)
                    .foregroundColor(.secondary)
                    .lineLimit(2)
                
                HStack {
                    Label("Level \(requiredLevel)", systemImage: "star.fill")
                        .font(.caption)
                        .foregroundColor(.yellow)
                    
                    Spacer()
                    
                    Label("\(reward) Coins", systemImage: "dollarsign.circle.fill")
                        .font(.caption)
                        .foregroundColor(.green)
                }
            }
            
            Spacer()
            
            Image(systemName: "chevron.right")
                .foregroundColor(.secondary)
            }
            .padding()
        .background(Color(.windowBackgroundColor))
        .cornerRadius(12)
        .padding(.horizontal)
    }
}

struct FloorDetailView: View {
    let floor: Floor
    @Environment(\.dismiss) var dismiss
    @State private var isPlaying = false
    
    var body: some View {
        VStack(spacing: 20) {
            // Floor-Header
            VStack(spacing: 16) {
                Image(floor.image)
                    .resizable()
                    .aspectRatio(contentMode: .fill)
                    .frame(height: 300)
                    .clipped()
                    .cornerRadius(12)
                
                    Text(floor.name)
                        .font(.title)
                    .bold()
                    
                    Text(floor.description)
                        .font(.body)
                        .foregroundColor(.secondary)
                    .multilineTextAlignment(.center)
                    .padding(.horizontal)
            }
            
            // Statistiken
            VStack(spacing: 16) {
                StatRow(
                    title: "Schwierigkeit",
                    value: "Level \(floor.difficulty)",
                    icon: "chart.bar.fill",
                    color: .blue
                )
                
                StatRow(
                    title: "Benötigtes Level",
                    value: "Level \(floor.requiredLevel)",
                    icon: "star.fill",
                    color: .yellow
                )
                
                StatRow(
                    title: "Belohnung",
                    value: "\(floor.reward) Coins",
                    icon: "dollarsign.circle.fill",
                    color: .green
                )
                    }
            .padding()
            .background(Color(.windowBackgroundColor))
            .cornerRadius(12)
            .padding(.horizontal)
            
            // Musik-Player
                    Button(action: {
                        isPlaying.toggle()
                    }) {
                        HStack {
                            Image(systemName: isPlaying ? "pause.fill" : "play.fill")
                    Text(isPlaying ? "Pause" : "Abspielen")
                        }
                .frame(width: 200)
            }
            .buttonStyle(.borderedProminent)
            
            Spacer()
                }
                .padding()
        .frame(width: 600, height: 700)
        .toolbar {
            ToolbarItem(placement: .automatic) {
                Button("Schließen") {
                    dismiss()
                }
            }
        }
    }
}

struct StatRow: View {
    let title: String
    let value: String
    let icon: String
    let color: Color
    
    var body: some View {
        HStack {
            Image(systemName: icon)
                .foregroundColor(color)
            Text(title)
                .foregroundColor(.secondary)
            Spacer()
            Text(value)
                .bold()
        }
    }
}

struct MusicGenreListView_Previews: PreviewProvider {
    static var previews: some View {
        MusicGenreListView()
            .environmentObject(AppState())
    }
} 