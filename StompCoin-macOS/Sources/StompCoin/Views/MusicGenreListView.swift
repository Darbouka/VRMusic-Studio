import SwiftUI
import Foundation

struct MusicGenreListView: View {
    @EnvironmentObject var appState: AppState
    
    var body: some View {
        ScrollView {
            LazyVGrid(columns: [
                GridItem(.flexible()),
                GridItem(.flexible())
            ], spacing: 20) {
                ForEach(appState.musicGenres) { genre in
                    MusicGenreCard(genre: genre)
                }
            }
            .padding()
        }
    }
}

struct MusicGenreCard: View {
    let genre: MusicGenre
    
    var body: some View {
        VStack(alignment: .leading, spacing: 10) {
            HStack {
                Image(systemName: genre.icon)
                    .font(.title)
                    .foregroundColor(.blue)
                
                Text(genre.name)
                    .font(.headline)
            }
            
            Text(genre.description)
                .font(.subheadline)
                .foregroundColor(.secondary)
                .lineLimit(2)
            
            Text("\(genre.floors.count) Floors")
                .font(.caption)
                .foregroundColor(.blue)
        }
        .padding()
        .frame(maxWidth: .infinity, alignment: .leading)
        .background(Color(NSColor.windowBackgroundColor))
        .cornerRadius(12)
        .shadow(radius: 2)
    }
} 