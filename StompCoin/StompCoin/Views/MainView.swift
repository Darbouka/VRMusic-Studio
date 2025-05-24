import SwiftUI

struct MainView: View {
    @StateObject private var viewModel = MainViewModel()
    @Environment(\.colorScheme) var colorScheme
    
    var body: some View {
        TabView {
            DashboardView()
                .tabItem {
                    Label("Dashboard", systemImage: "chart.bar.fill")
                }
            
            FitnessView()
                .tabItem {
                    Label("Fitness", systemImage: "figure.walk")
                }
            
            WalletView()
                .tabItem {
                    Label("Wallet", systemImage: "wallet.pass.fill")
                }
            
            SocialView()
                .tabItem {
                    Label("Social", systemImage: "person.2.fill")
                }
            
            ProfileView()
                .tabItem {
                    Label("Profil", systemImage: "person.circle.fill")
                }
        }
        .onAppear {
            setupAppearance()
        }
    }
    
    private func setupAppearance() {
        let appearance = UITabBarAppearance()
        
        if colorScheme == .dark {
            appearance.backgroundColor = UIColor(Color(.systemBackground))
            appearance.stackedLayoutAppearance.normal.iconColor = .gray
            appearance.stackedLayoutAppearance.selected.iconColor = .white
        } else {
            appearance.backgroundColor = UIColor(Color(.systemBackground))
            appearance.stackedLayoutAppearance.normal.iconColor = .gray
            appearance.stackedLayoutAppearance.selected.iconColor = .blue
        }
        
        UITabBar.appearance().standardAppearance = appearance
        UITabBar.appearance().scrollEdgeAppearance = appearance
    }
}

struct DashboardView: View {
    @StateObject private var viewModel = DashboardViewModel()
    
    var body: some View {
        NavigationView {
            ScrollView {
                VStack(spacing: 20) {
                    StatsCard(stats: viewModel.fitnessStats)
                    ActivityChart(data: viewModel.activityData)
                    AchievementsList(achievements: viewModel.recentAchievements)
                    FriendsActivityList(activities: viewModel.friendsActivities)
                }
                .padding()
            }
            .navigationTitle("Dashboard")
            .refreshable {
                await viewModel.refresh()
            }
        }
    }
}

struct StatsCard: View {
    let stats: FitnessStats
    
    var body: some View {
        VStack(spacing: 15) {
            HStack {
                StatItem(title: "Schritte", value: "\(stats.steps)", icon: "figure.walk")
                StatItem(title: "Kalorien", value: "\(Int(stats.calories))", icon: "flame.fill")
                StatItem(title: "Distanz", value: String(format: "%.1f km", stats.distance), icon: "map.fill")
            }
        }
        .padding()
        .background(Color(.systemBackground))
        .cornerRadius(15)
        .shadow(radius: 5)
    }
}

struct StatItem: View {
    let title: String
    let value: String
    let icon: String
    
    var body: some View {
        VStack {
            Image(systemName: icon)
                .font(.title2)
                .foregroundColor(.blue)
            Text(value)
                .font(.headline)
            Text(title)
                .font(.caption)
                .foregroundColor(.gray)
        }
        .frame(maxWidth: .infinity)
    }
}

struct ActivityChart: View {
    let data: [ActivityData]
    
    var body: some View {
        VStack(alignment: .leading) {
            Text("Aktivität")
                .font(.headline)
            
            HStack(alignment: .bottom, spacing: 8) {
                ForEach(data) { item in
                    VStack {
                        RoundedRectangle(cornerRadius: 5)
                            .fill(Color.blue)
                            .frame(height: CGFloat(item.value) * 100)
                        Text(item.label)
                            .font(.caption)
                    }
                }
            }
            .frame(height: 150)
        }
        .padding()
        .background(Color(.systemBackground))
        .cornerRadius(15)
        .shadow(radius: 5)
    }
}

struct AchievementsList: View {
    let achievements: [Achievement]
    
    var body: some View {
        VStack(alignment: .leading) {
            Text("Erfolge")
                .font(.headline)
            
            ForEach(achievements) { achievement in
                AchievementRow(achievement: achievement)
            }
        }
        .padding()
        .background(Color(.systemBackground))
        .cornerRadius(15)
        .shadow(radius: 5)
    }
}

struct AchievementRow: View {
    let achievement: Achievement
    
    var body: some View {
        HStack {
            Image(systemName: achievement.icon)
                .font(.title2)
                .foregroundColor(.yellow)
            
            VStack(alignment: .leading) {
                Text(achievement.title)
                    .font(.headline)
                Text(achievement.description)
                    .font(.caption)
                    .foregroundColor(.gray)
            }
            
            Spacer()
            
            Text("+\(achievement.reward) XP")
                .font(.caption)
                .foregroundColor(.green)
        }
        .padding(.vertical, 5)
    }
}

struct FriendsActivityList: View {
    let activities: [FriendActivity]
    
    var body: some View {
        VStack(alignment: .leading) {
            Text("Freunde")
                .font(.headline)
            
            ForEach(activities) { activity in
                ActivityRow(activity: activity)
            }
        }
        .padding()
        .background(Color(.systemBackground))
        .cornerRadius(15)
        .shadow(radius: 5)
    }
}

struct ActivityRow: View {
    let activity: FriendActivity
    
    var body: some View {
        HStack {
            AsyncImage(url: URL(string: activity.friendAvatar)) { image in
                image.resizable()
            } placeholder: {
                Color.gray
            }
            .frame(width: 40, height: 40)
            .clipShape(Circle())
            
            VStack(alignment: .leading) {
                Text(activity.friendName)
                    .font(.headline)
                Text(activity.description)
                    .font(.caption)
                    .foregroundColor(.gray)
            }
            
            Spacer()
            
            Text(activity.timeAgo)
                .font(.caption)
                .foregroundColor(.gray)
        }
        .padding(.vertical, 5)
    }
}

// MARK: - ViewModels

class MainViewModel: ObservableObject {
    @Published var selectedTab = 0
}

class DashboardViewModel: ObservableObject {
    @Published var fitnessStats = FitnessStats()
    @Published var activityData: [ActivityData] = []
    @Published var recentAchievements: [Achievement] = []
    @Published var friendsActivities: [FriendActivity] = []
    
    func refresh() async {
        // Implementierung der Datenaktualisierung
    }
}

// MARK: - Modelle

struct FitnessStats {
    var steps: Int = 0
    var calories: Double = 0
    var distance: Double = 0
}

struct ActivityData: Identifiable {
    let id = UUID()
    let label: String
    let value: Double
}

struct Achievement: Identifiable {
    let id = UUID()
    let title: String
    let description: String
    let icon: String
    let reward: Int
}

struct FriendActivity: Identifiable {
    let id = UUID()
    let friendName: String
    let friendAvatar: String
    let description: String
    let timeAgo: String
} 