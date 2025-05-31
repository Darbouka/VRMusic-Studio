import SwiftUI
import Foundation

struct HomeView: View {
    @EnvironmentObject var appState: AppState
    
    var body: some View {
        ScrollView {
            VStack(spacing: 20) {
                // Statistik-Karten
                HStack(spacing: 15) {
                    StatCard(
                        title: "Level",
                        value: "12",
                        icon: "star.fill",
                        color: Color.yellow
                    )
                    
                    StatCard(
                        title: "Wallet",
                        value: "1.234,56",
                        icon: "dollarsign.circle.fill",
                        color: Color.green
                    )
                    
                    StatCard(
                        title: "XP",
                        value: "4.567",
                        icon: "bolt.fill",
                        color: Color.blue
                    )
                }
                .padding(.horizontal)
                
                // Fitness-Karte
                FitnessCard()
                    .padding(.horizontal)
                
                // Aktuelle Aktivität
                VStack(alignment: .leading, spacing: 10) {
                    Text("Aktuelle Aktivität")
                        .font(.headline)
                        .padding(.horizontal)
                    
                    ScrollView(.horizontal, showsIndicators: false) {
                        HStack(spacing: 15) {
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

struct StatCard: View {
    let title: String
    let value: String
    let icon: String
    let color: Color
    
    var body: some View {
        VStack(alignment: .leading, spacing: 8) {
            HStack {
                Image(systemName: icon)
                    .foregroundColor(color)
                Text(title)
                    .font(.subheadline)
                    .foregroundColor(.secondary)
            }
            
            Text(value)
                .font(.title2)
                .fontWeight(.bold)
        }
        .frame(maxWidth: .infinity, alignment: .leading)
        .padding()
        .background(Color(NSColor.windowBackgroundColor))
        .cornerRadius(12)
        .shadow(radius: 2)
    }
}

struct FitnessCard: View {
    var body: some View {
        VStack(alignment: .leading, spacing: 15) {
            Text("Fitness")
                .font(.headline)
            
            HStack(spacing: 20) {
                StatRow(title: "Schritte", value: "9.876", icon: "figure.walk", color: .blue)
                StatRow(title: "Kalorien", value: "456", icon: "flame.fill", color: .orange)
                StatRow(title: "Distanz", value: "7,89 km", icon: "map.fill", color: .green)
            }
        }
        .padding()
        .background(Color(NSColor.windowBackgroundColor))
        .cornerRadius(12)
        .shadow(radius: 2)
    }
}

struct StatRow: View {
    let title: String
    let value: String
    let icon: String
    let color: Color
    
    var body: some View {
        VStack(spacing: 8) {
            Image(systemName: icon)
                .font(.title2)
                .foregroundColor(color)
            
            Text(value)
                .font(.headline)
            
            Text(title)
                .font(.caption)
                .foregroundColor(.secondary)
        }
        .frame(maxWidth: .infinity)
    }
} 