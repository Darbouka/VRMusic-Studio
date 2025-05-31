import SwiftUI

struct MusicGenreListView: View {
    @EnvironmentObject var appState: AppState
    @State private var selectedGenre: MusicGenre?
    
    var body: some View {
        NavigationView {
            // Sidebar mit Genre-Liste
            List(appState.musicGenres, id: \.id) { genre in
                Button(action: {
                    selectedGenre = genre
                }) {
                    HStack {
                        Image(systemName: genre.icon)
                            .foregroundColor(.accentColor)
                        Text(genre.name)
                        Spacer()
                        Text("\(genre.floors.count) Floors")
                            .foregroundColor(.secondary)
                    }
                }
            }
            .listStyle(SidebarListStyle())
            .frame(minWidth: 250)
            
            // Detail-Ansicht
            if let selectedGenre = selectedGenre {
                ScrollView {
                    VStack(alignment: .leading, spacing: 20) {
                        // Genre-Header
                        HStack {
                            Image(systemName: selectedGenre.icon)
                                .font(.system(size: 40))
                                .foregroundColor(.accentColor)
                            VStack(alignment: .leading) {
                                Text(selectedGenre.name)
                                    .font(.title)
                                    .bold()
                                Text(selectedGenre.description)
                                    .foregroundColor(.secondary)
                            }
                        }
                        .padding()
                        
                        // Floors
                        Text("Floors")
                            .font(.title2)
                            .bold()
                            .padding(.horizontal)
                        
                        LazyVGrid(columns: [
                            GridItem(.adaptive(minimum: 300, maximum: 400), spacing: 20)
                        ], spacing: 20) {
                            ForEach(selectedGenre.floors) { floor in
                                FloorCard(floor: floor)
                            }
                        }
                        .padding(.horizontal)
                    }
                }
            } else {
                Text("Wähle ein Genre aus")
                    .font(.title2)
                    .foregroundColor(.secondary)
            }
        }
        .navigationViewStyle(DoubleColumnNavigationViewStyle())
    }
}

struct FloorCard: View {
    let floor: Floor
    
    var body: some View {
        VStack(alignment: .leading, spacing: 10) {
            Text(floor.name)
                .font(.headline)
            
            Text(floor.description)
                .font(.subheadline)
                .foregroundColor(.secondary)
            
            HStack {
                Label("\(floor.reward) Coins", systemImage: "dollarsign.circle.fill")
                    .foregroundColor(.green)
                Spacer()
                Label("\(floor.difficulty)/10", systemImage: "chart.bar.fill")
                    .foregroundColor(.blue)
            }
            .font(.caption)
        }
        .padding()
        .background(Color(.systemBackground))
        .cornerRadius(10)
        .shadow(radius: 2)
    }
} 