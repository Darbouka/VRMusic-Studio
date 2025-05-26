import SwiftUI

struct MainTabView: View {
    @StateObject private var appManager = AppManager.shared
    
    var body: some View {
        TabView {
            HomeView()
                .tabItem {
                    Label("Home", systemImage: "house.fill")
                }
            
            ChallengesView()
                .tabItem {
                    Label("Challenges", systemImage: "trophy.fill")
                }
            
            StreamingView()
                .tabItem {
                    Label("Stream", systemImage: "video.fill")
                }
            
            RewardsView()
                .tabItem {
                    Label("Rewards", systemImage: "star.fill")
                }
            
            ProfileView()
                .tabItem {
                    Label("Profil", systemImage: "person.fill")
                }
        }
        .accentColor(.blue)
    }
}

#Preview {
    MainTabView()
} 