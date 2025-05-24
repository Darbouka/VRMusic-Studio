import SwiftUI
import SwiftData

struct DashboardView: View {
    @Environment(\.modelContext) private var modelContext
    @Query private var users: [User]
    @State private var showingStepSettings = false
    @State private var showingVideoBoost = false
    
    var currentUser: User? {
        users.first
    }
    
    var body: some View {
        NavigationView {
            ScrollView {
                VStack(spacing: 20) {
                    // Step Counter Card
                    StepCounterCard(user: currentUser)
                    
                    // Coin Balance Card
                    CoinBalanceCard(user: currentUser)
                    
                    // Active Events Card
                    ActiveEventsCard()
                    
                    // Daily Challenges Card
                    DailyChallengesCard()
                    
                    // Video Boost Card
                    VideoBoostCard(user: currentUser, showingVideoBoost: $showingVideoBoost)
                }
                .padding()
            }
            .navigationTitle("Dashboard")
            .toolbar {
                ToolbarItem(placement: .navigationBarTrailing) {
                    Button(action: { showingStepSettings = true }) {
                        Image(systemName: "gear")
                    }
                }
            }
            .sheet(isPresented: $showingStepSettings) {
                StepSettingsView(user: currentUser)
            }
            .sheet(isPresented: $showingVideoBoost) {
                VideoBoostView(user: currentUser)
            }
        }
    }
}

struct StepCounterCard: View {
    let user: User?
    
    var body: some View {
        VStack {
            Text("Today's Steps")
                .font(.headline)
            
            Text("\(user?.currentSteps ?? 0)")
                .font(.system(size: 48, weight: .bold))
            
            Text("of \(user?.dailyStepLimit ?? 1000)")
                .font(.subheadline)
                .foregroundColor(.secondary)
            
            ProgressView(value: Double(user?.currentSteps ?? 0), total: Double(user?.dailyStepLimit ?? 1000))
                .padding(.horizontal)
        }
        .padding()
        .background(Color(.systemBackground))
        .cornerRadius(12)
        .shadow(radius: 2)
    }
}

struct CoinBalanceCard: View {
    let user: User?
    
    var body: some View {
        HStack(spacing: 20) {
            VStack {
                Text("StompCoins")
                    .font(.headline)
                Text("\(Int(user?.stompCoins ?? 0))")
                    .font(.title)
                    .bold()
            }
            
            Divider()
            
            VStack {
                Text("OM Coins")
                    .font(.headline)
                Text("\(Int(user?.omCoins ?? 0))")
                    .font(.title)
                    .bold()
            }
        }
        .padding()
        .background(Color(.systemBackground))
        .cornerRadius(12)
        .shadow(radius: 2)
    }
}

struct ActiveEventsCard: View {
    @Query private var events: [Event]
    
    var activeEvents: [Event] {
        events.filter { $0.isActive }
    }
    
    var body: some View {
        VStack(alignment: .leading) {
            Text("Active Events")
                .font(.headline)
                .padding(.bottom, 8)
            
            if activeEvents.isEmpty {
                Text("No active events")
                    .foregroundColor(.secondary)
            } else {
                ForEach(activeEvents) { event in
                    EventRow(event: event)
                }
            }
        }
        .padding()
        .background(Color(.systemBackground))
        .cornerRadius(12)
        .shadow(radius: 2)
    }
}

struct EventRow: View {
    let event: Event
    
    var body: some View {
        VStack(alignment: .leading) {
            Text(event.name)
                .font(.subheadline)
                .bold()
            Text(event.location)
                .font(.caption)
                .foregroundColor(.secondary)
        }
        .padding(.vertical, 4)
    }
}

struct DailyChallengesCard: View {
    @Query private var challenges: [Challenge]
    
    var dailyChallenges: [Challenge] {
        challenges.filter { Calendar.current.isDateInToday($0.startDate) }
    }
    
    var body: some View {
        VStack(alignment: .leading) {
            Text("Daily Challenges")
                .font(.headline)
                .padding(.bottom, 8)
            
            if dailyChallenges.isEmpty {
                Text("No challenges today")
                    .foregroundColor(.secondary)
            } else {
                ForEach(dailyChallenges) { challenge in
                    ChallengeRow(challenge: challenge)
                }
            }
        }
        .padding()
        .background(Color(.systemBackground))
        .cornerRadius(12)
        .shadow(radius: 2)
    }
}

struct ChallengeRow: View {
    let challenge: Challenge
    
    var body: some View {
        VStack(alignment: .leading) {
            Text(challenge.name)
                .font(.subheadline)
                .bold()
            Text("Reward: \(Int(challenge.reward)) StompCoins")
                .font(.caption)
                .foregroundColor(.secondary)
        }
        .padding(.vertical, 4)
    }
}

struct VideoBoostCard: View {
    let user: User?
    @Binding var showingVideoBoost: Bool
    
    var body: some View {
        VStack {
            Text("Video Boost")
                .font(.headline)
            
            Text("\(user?.videoBoostsRemaining ?? 0) remaining today")
                .font(.subheadline)
                .foregroundColor(.secondary)
            
            Button(action: { showingVideoBoost = true }) {
                Text("Watch Video")
                    .bold()
                    .foregroundColor(.white)
                    .padding()
                    .frame(maxWidth: .infinity)
                    .background(Color.blue)
                    .cornerRadius(8)
            }
            .disabled(user?.videoBoostsRemaining == 0)
        }
        .padding()
        .background(Color(.systemBackground))
        .cornerRadius(12)
        .shadow(radius: 2)
    }
}

#Preview {
    DashboardView()
        .modelContainer(for: [User.self, Event.self, Challenge.self], inMemory: true)
} 