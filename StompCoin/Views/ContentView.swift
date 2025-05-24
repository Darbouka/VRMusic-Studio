import SwiftUI
import SwiftData

struct ContentView: View {
    @Environment(\.modelContext) private var modelContext
    @Query private var users: [User]
    @State private var selectedTab = 0
    
    var body: some View {
        TabView(selection: $selectedTab) {
            DashboardView()
                .tabItem {
                    Label("Dashboard", systemImage: "house.fill")
                }
                .tag(0)
            
            EventsView()
                .tabItem {
                    Label("Events", systemImage: "calendar")
                }
                .tag(1)
            
            ChallengesView()
                .tabItem {
                    Label("Challenges", systemImage: "trophy.fill")
                }
                .tag(2)
            
            WalletView()
                .tabItem {
                    Label("Wallet", systemImage: "wallet.pass.fill")
                }
                .tag(3)
            
            ProfileView()
                .tabItem {
                    Label("Profile", systemImage: "person.fill")
                }
                .tag(4)
        }
    }
}

#Preview {
    ContentView()
        .modelContainer(for: [User.self, Event.self, EventParticipation.self, Challenge.self, Transaction.self, Subscription.self], inMemory: true)
} 