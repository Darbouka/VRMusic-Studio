import SwiftUI

struct HomeView: View {
    @EnvironmentObject var appState: AppState
    
    var body: some View {
        NavigationView {
            ScrollView {
                VStack(spacing: 20) {
                    // Begrüßung
                    VStack(alignment: .leading, spacing: 8) {
                        Text("Willkommen zurück,")
                            .font(.title2)
                            .foregroundColor(.secondary)
                        Text(appState.currentUser?.username ?? "Benutzer")
                            .font(.largeTitle)
                            .bold()
                    }
                    .frame(maxWidth: .infinity, alignment: .leading)
                    .padding(.horizontal)
                    
                    // Statistiken
                    HStack(spacing: 20) {
                        StatCard(
                            title: "Level",
                            value: "\(appState.currentUser?.level ?? 1)",
                            icon: "star.fill",
                            color: Color.yellow
                        )
                        
                        StatCard(
                            title: "Wallet",
                            value: String(format: "%.2f", appState.currentUser?.walletBalance ?? 0),
                            icon: "dollarsign.circle.fill",
                            color: Color.green
                        )
                        
                        StatCard(
                            title: "XP",
                            value: "\(appState.currentUser?.experience ?? 0)",
                            icon: "bolt.fill",
                            color: Color.blue
                        )
                    }
                    .padding(.horizontal)
                    
                    // Fitness-Daten
                    if let fitnessData = appState.fitnessData {
                        VStack(alignment: .leading, spacing: 16) {
                            Text("Fitness-Daten")
                                .font(.title2)
                                .bold()
                            
                            HStack(spacing: 20) {
                                FitnessCard(
                                    title: "Schritte",
                                    value: "\(fitnessData.steps)",
                                    icon: "figure.walk",
                                    color: .blue
                                )
                                
                                FitnessCard(
                                    title: "Kalorien",
                                    value: String(format: "%.1f", fitnessData.calories),
                                    icon: "flame.fill",
                                    color: .orange
                                )
                                
                                FitnessCard(
                                    title: "Distanz",
                                    value: String(format: "%.2f km", fitnessData.distance),
                                    icon: "map.fill",
                                    color: .green
                                )
                            }
                        }
                        .padding()
                        .background(Color(.windowBackgroundColor))
                        .cornerRadius(12)
                        .padding(.horizontal)
                    }
                    
                    // Letzte Aktivitäten
                    VStack(alignment: .leading, spacing: 16) {
                        Text("Letzte Aktivitäten")
                            .font(.title2)
                            .bold()
                        
                        ForEach(appState.currentUser?.activities ?? []) { activity in
                            ActivityRow(activity: activity)
                        }
                    }
                    .padding()
                    .background(Color(.windowBackgroundColor))
                    .cornerRadius(12)
                    .padding(.horizontal)
                }
                .padding(.vertical)
            }
            .navigationTitle("Home")
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
                    .foregroundColor(.secondary)
            }
            .font(.subheadline)
            
            Text(value)
                .font(.title)
                .bold()
        }
        .frame(maxWidth: .infinity, alignment: .leading)
        .padding()
        .background(Color(.windowBackgroundColor))
        .cornerRadius(12)
    }
}

struct FitnessCard: View {
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
                    .foregroundColor(.secondary)
            }
            .font(.subheadline)
            
            Text(value)
                .font(.title2)
                .bold()
        }
        .frame(maxWidth: .infinity, alignment: .leading)
        .padding()
        .background(Color(.controlBackgroundColor))
        .cornerRadius(8)
    }
}

struct ActivityRow: View {
    let activity: Activity
    
    var body: some View {
        HStack {
            Image(systemName: activity.icon)
                .foregroundColor(.blue)
                .frame(width: 32, height: 32)
                .background(Color.blue.opacity(0.1))
                .cornerRadius(8)
            
            VStack(alignment: .leading, spacing: 4) {
                Text(activity.title)
                    .font(.headline)
                Text(activity.description)
                    .font(.subheadline)
                    .foregroundColor(.secondary)
            }
            
            Spacer()
            
            Text(activity.timestamp, style: .relative)
                .font(.caption)
                .foregroundColor(.secondary)
        }
        .padding(.vertical, 8)
    }
}

struct HomeView_Previews: PreviewProvider {
    static var previews: some View {
        HomeView()
            .environmentObject(AppState())
    }
} 