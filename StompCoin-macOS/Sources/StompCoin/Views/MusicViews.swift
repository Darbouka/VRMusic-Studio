import SwiftUI

struct MusicGenreListView: View {
    @EnvironmentObject var appState: AppState
    @State private var selectedGenre: MusicGenre?
    @State private var showFloorDetails = false
    
    var body: some View {
        NavigationView {
            ScrollView {
                LazyVGrid(columns: [GridItem(.adaptive(minimum: 300))], spacing: 20) {
                    ForEach(appState.musicGenres) { genre in
                        NavigationLink(destination: MusicGenreDetailView(genre: genre)) {
                            MusicGenreCard(genre: genre)
                        }
                    }
                }
                .padding()
            }
            .navigationTitle("Musikgenres")
        }
    }
}

struct MusicGenreCard: View {
    let genre: MusicGenre
    
    var body: some View {
        VStack(alignment: .leading, spacing: 10) {
            Image(genre.image)
                .resizable()
                .aspectRatio(contentMode: .fill)
                .frame(height: 200)
                .clipped()
                .cornerRadius(10)
            
            VStack(alignment: .leading, spacing: 5) {
                Text(genre.name)
                    .font(.title2)
                    .fontWeight(.bold)
                
                Text(genre.description)
                    .font(.subheadline)
                    .foregroundColor(.secondary)
                
                HStack {
                    Image(systemName: genre.icon)
                        .foregroundColor(.blue)
                    Text("\(genre.floors.count) Floors")
                        .font(.caption)
                        .foregroundColor(.secondary)
                }
            }
            .padding()
        }
        .background(Color(.systemBackground))
        .cornerRadius(15)
        .shadow(radius: 5)
    }
}

struct MusicGenreDetailView: View {
    let genre: MusicGenre
    @Environment(\.dismiss) private var dismiss
    @State private var selectedFloor: Floor?
    @State private var showFloorDetails = false
    
    var body: some View {
        ScrollView {
            VStack(alignment: .leading, spacing: 20) {
                Image(genre.image)
                    .resizable()
                    .aspectRatio(contentMode: .fill)
                    .frame(height: 250)
                    .clipped()
                
                VStack(alignment: .leading, spacing: 15) {
                    Text(genre.name)
                        .font(.title)
                        .fontWeight(.bold)
                    
                    Text(genre.description)
                        .font(.body)
                        .foregroundColor(.secondary)
                    
                    Divider()
                    
                    Text("Verfügbare Floors")
                        .font(.headline)
                        .padding(.top)
                    
                    ForEach(genre.floors) { floor in
                        Button(action: {
                            selectedFloor = floor
                            showFloorDetails = true
                        }) {
                            FloorCard(floor: floor)
                        }
                    }
                }
                .padding()
            }
        }
        .navigationBarTitleDisplayMode(.inline)
        .sheet(isPresented: $showFloorDetails) {
            if let floor = selectedFloor {
                FloorDetailView(floor: floor)
            }
        }
    }
}

struct FloorCard: View {
    let floor: Floor
    @State private var showDetails = false
    
    var body: some View {
        VStack(alignment: .leading, spacing: 10) {
            Image(floor.image)
                .resizable()
                .aspectRatio(contentMode: .fill)
                .frame(height: 150)
                .clipped()
                .cornerRadius(10)
            
            VStack(alignment: .leading, spacing: 5) {
                Text(floor.name)
                    .font(.headline)
                
                HStack {
                    Text("Schwierigkeit: \(floor.difficulty)")
                        .font(.subheadline)
                        .foregroundColor(.blue)
                    
                    Spacer()
                    
                    Text("Level \(floor.requiredLevel)")
                        .font(.caption)
                        .padding(.horizontal, 8)
                        .padding(.vertical, 4)
                        .background(Color.blue.opacity(0.1))
                        .cornerRadius(8)
                }
            }
            .padding()
        }
        .background(Color(.systemBackground))
        .cornerRadius(15)
        .shadow(radius: 5)
    }
}

struct FloorDetailView: View {
    let floor: Floor
    @Environment(\.dismiss) private var dismiss
    @State private var isPlaying = false
    
    var body: some View {
        ScrollView {
            VStack(alignment: .leading, spacing: 20) {
                Image(floor.image)
                    .resizable()
                    .aspectRatio(contentMode: .fill)
                    .frame(height: 250)
                    .clipped()
                
                VStack(alignment: .leading, spacing: 15) {
                    Text(floor.name)
                        .font(.title)
                        .fontWeight(.bold)
                    
                    Text(floor.description)
                        .font(.body)
                        .foregroundColor(.secondary)
                    
                    Divider()
                    
                    VStack(alignment: .leading, spacing: 15) {
                        StatRow(icon: "chart.bar", title: "Schwierigkeit", value: "\(floor.difficulty)")
                        StatRow(icon: "person.fill", title: "Benötigtes Level", value: "\(floor.requiredLevel)")
                        StatRow(icon: "dollarsign.circle.fill", title: "Belohnung", value: "\(floor.reward) Coins")
                    }
                    
                    Button(action: {
                        isPlaying.toggle()
                    }) {
                        HStack {
                            Image(systemName: isPlaying ? "pause.fill" : "play.fill")
                            Text(isPlaying ? "Pause" : "Start")
                        }
                        .frame(maxWidth: .infinity)
                        .padding()
                        .background(Color.blue)
                        .foregroundColor(.white)
                        .cornerRadius(10)
                    }
                    .padding(.top)
                }
                .padding()
            }
        }
        .navigationBarTitleDisplayMode(.inline)
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
    let icon: String
    let title: String
    let value: String
    
    var body: some View {
        HStack {
            Image(systemName: icon)
                .foregroundColor(.blue)
                .frame(width: 30)
            
            Text(title)
                .foregroundColor(.secondary)
            
            Spacer()
            
            Text(value)
                .fontWeight(.medium)
        }
    }
} 