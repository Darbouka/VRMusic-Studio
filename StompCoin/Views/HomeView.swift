import SwiftUI

struct HomeView: View {
    @StateObject private var viewModel = HomeViewModel()
    @EnvironmentObject private var appManager: AppManager
    
    var body: some View {
        NavigationView {
            ScrollView {
                VStack(spacing: 20) {
                    // Benutzer-Statistiken
                    UserStatsView(user: appManager.currentUser)
                    
                    // Aktive Challenges
                    ActiveChallengesView(challenges: viewModel.activeChallenges)
                    
                    // Top Streamer
                    TopStreamersView(streamers: viewModel.topStreamers)
                    
                    // Belohnungen
                    RewardsOverviewView(rewards: viewModel.userRewards)
                }
                .padding()
            }
            .navigationTitle("StompCoin")
            .refreshable {
                await viewModel.refreshData()
            }
        }
    }
}

struct UserStatsView: View {
    let user: User
    
    var body: some View {
        VStack(spacing: 10) {
            HStack {
                VStack(alignment: .leading) {
                    Text(user.username)
                        .font(.title2)
                        .bold()
                    Text("Level \(user.level)")
                        .font(.subheadline)
                }
                Spacer()
                if user.isVIPPremium {
                    Image(systemName: "crown.fill")
                        .foregroundColor(.yellow)
                }
            }
            
            HStack(spacing: 20) {
                StatView(title: "StompCoins", value: "\(user.stompCoins)")
                StatView(title: "OMCoins", value: "\(user.omCoins)")
                StatView(title: "Tokens", value: "\(user.tokens)")
            }
        }
        .padding()
        .background(Color(.systemBackground))
        .cornerRadius(12)
        .shadow(radius: 2)
    }
}

struct StatView: View {
    let title: String
    let value: String
    
    var body: some View {
        VStack {
            Text(title)
                .font(.caption)
                .foregroundColor(.secondary)
            Text(value)
                .font(.headline)
        }
    }
}

struct ActiveChallengesView: View {
    let challenges: [Challenge]
    
    var body: some View {
        VStack(alignment: .leading, spacing: 10) {
            Text("Aktive Challenges")
                .font(.headline)
            
            ScrollView(.horizontal, showsIndicators: false) {
                HStack(spacing: 15) {
                    ForEach(challenges) { challenge in
                        ChallengeCard(challenge: challenge)
                    }
                }
            }
        }
    }
}

struct ChallengeCard: View {
    let challenge: Challenge
    
    var body: some View {
        VStack(alignment: .leading, spacing: 8) {
            Text(challenge.title)
                .font(.headline)
            
            Text(challenge.description)
                .font(.caption)
                .foregroundColor(.secondary)
            
            ProgressView(value: challenge.progress)
                .tint(.blue)
            
            HStack {
                Text("\(challenge.participants.count) Teilnehmer")
                    .font(.caption2)
                Spacer()
                Text("\(challenge.timeRemaining) verbleibend")
                    .font(.caption2)
            }
        }
        .padding()
        .frame(width: 200)
        .background(Color(.systemBackground))
        .cornerRadius(12)
        .shadow(radius: 2)
    }
}

struct TopStreamersView: View {
    let streamers: [User]
    
    var body: some View {
        VStack(alignment: .leading, spacing: 10) {
            Text("Top Streamer")
                .font(.headline)
            
            ForEach(streamers) { streamer in
                HStack {
                    Text("\(streamer.username)")
                    Spacer()
                    Text("\(streamer.totalStomps) Stomps")
                        .foregroundColor(.secondary)
                }
                .padding(.vertical, 5)
            }
        }
        .padding()
        .background(Color(.systemBackground))
        .cornerRadius(12)
        .shadow(radius: 2)
    }
}

struct RewardsOverviewView: View {
    let rewards: Rewards
    
    var body: some View {
        VStack(alignment: .leading, spacing: 10) {
            Text("Deine Belohnungen")
                .font(.headline)
            
            HStack(spacing: 20) {
                RewardView(title: "StompCoins", value: rewards.stompCoins, icon: "coin.fill")
                RewardView(title: "OMCoins", value: rewards.omCoins, icon: "star.fill")
                RewardView(title: "Level", value: rewards.level, icon: "trophy.fill")
            }
        }
        .padding()
        .background(Color(.systemBackground))
        .cornerRadius(12)
        .shadow(radius: 2)
    }
}

struct RewardView: View {
    let title: String
    let value: Int
    let icon: String
    
    var body: some View {
        VStack {
            Image(systemName: icon)
                .font(.title2)
            Text("\(value)")
                .font(.headline)
            Text(title)
                .font(.caption)
                .foregroundColor(.secondary)
        }
    }
}

#Preview {
    HomeView()
        .environmentObject(AppManager.shared)
} 