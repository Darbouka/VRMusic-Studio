import SwiftUI
import SwiftData

struct ChallengesView: View {
    @Environment(\.modelContext) private var modelContext
    @Query private var challenges: [Challenge]
    @State private var selectedTab = 0
    @State private var showingChallengeDetails = false
    @State private var selectedChallenge: Challenge?
    @State private var showingSuccess = false
    @State private var showingError = false
    @State private var errorMessage = ""
    
    var activeChallenges: [Challenge] {
        challenges.filter { $0.isLive }
    }
    
    var upcomingChallenges: [Challenge] {
        challenges.filter { !$0.isLive && $0.startDate > Date() }
    }
    
    var completedChallenges: [Challenge] {
        challenges.filter { !$0.isLive && $0.endDate < Date() }
    }
    
    var body: some View {
        NavigationView {
            VStack {
                Picker("Challenge Type", selection: $selectedTab) {
                    Text("Active").tag(0)
                    Text("Upcoming").tag(1)
                    Text("Completed").tag(2)
                }
                .pickerStyle(.segmented)
                .padding()
                
                TabView(selection: $selectedTab) {
                    ChallengeListView(challenges: activeChallenges, showingDetails: $showingChallengeDetails, selectedChallenge: $selectedChallenge)
                        .tag(0)
                    
                    ChallengeListView(challenges: upcomingChallenges, showingDetails: $showingChallengeDetails, selectedChallenge: $selectedChallenge)
                        .tag(1)
                    
                    ChallengeListView(challenges: completedChallenges, showingDetails: $showingChallengeDetails, selectedChallenge: $selectedChallenge)
                        .tag(2)
                }
                .tabViewStyle(.page(indexDisplayMode: .never))
            }
            .navigationTitle("Challenges")
            .sheet(isPresented: $showingChallengeDetails) {
                if let challenge = selectedChallenge {
                    ChallengeDetailView(challenge: challenge)
                }
            }
        }
    }
    
    private func participateInChallenge() {
        guard let user = currentUser,
              let challenge = selectedChallenge else { return }
        
        let success = ChallengeManager.shared.joinChallenge(
            challenge: challenge,
            user: user,
            modelContext: modelContext
        )
        
        if success {
            showingSuccess = true
            // Challenge-spezifische Effekte anwenden
            challenge.participants.append(user)
            challenge.startDate = Date()
            
            // Benachrichtigung senden
            NotificationCenter.default.post(
                name: .challengeJoined,
                object: challenge
            )
        } else {
            showingError = true
            errorMessage = "Challenge konnte nicht beigetreten werden"
        }
    }
}

struct ChallengeListView: View {
    let challenges: [Challenge]
    @Binding var showingDetails: Bool
    @Binding var selectedChallenge: Challenge?
    
    var body: some View {
        List {
            if challenges.isEmpty {
                Text("No challenges available")
                    .foregroundColor(.secondary)
            } else {
                ForEach(challenges) { challenge in
                    ChallengeListItem(challenge: challenge)
                        .onTapGesture {
                            selectedChallenge = challenge
                            showingDetails = true
                        }
                }
            }
        }
    }
}

struct ChallengeListItem: View {
    let challenge: Challenge
    
    var body: some View {
        VStack(alignment: .leading, spacing: 8) {
            HStack {
                Text(challenge.name)
                    .font(.headline)
                Spacer()
                if challenge.isLive {
                    Text("LIVE")
                        .font(.caption)
                        .padding(.horizontal, 8)
                        .padding(.vertical, 4)
                        .background(Color.red)
                        .foregroundColor(.white)
                        .cornerRadius(4)
                }
            }
            
            Text(challenge.description)
                .font(.subheadline)
                .foregroundColor(.secondary)
                .lineLimit(2)
            
            HStack {
                Text("Reward:")
                Text("\(Int(challenge.reward)) StompCoins")
                    .bold()
            }
            .font(.caption)
            
            HStack {
                Text(challenge.startDate, style: .date)
                Text("-")
                Text(challenge.endDate, style: .date)
            }
            .font(.caption)
            .foregroundColor(.secondary)
            
            if challenge.isSocialMedia {
                Label("Social Media Challenge", systemImage: "globe")
                    .font(.caption)
                    .foregroundColor(.blue)
            }
        }
        .padding(.vertical, 8)
    }
}

struct ChallengeDetailView: View {
    @Environment(\.modelContext) private var modelContext
    @Environment(\.dismiss) private var dismiss
    let challenge: Challenge
    @State private var showingParticipationConfirmation = false
    
    var body: some View {
        NavigationView {
            ScrollView {
                VStack(alignment: .leading, spacing: 20) {
                    // Challenge Header
                    VStack(alignment: .leading, spacing: 8) {
                        Text(challenge.name)
                            .font(.title)
                            .bold()
                        
                        if challenge.isLive {
                            Text("LIVE")
                                .font(.headline)
                                .padding(.horizontal, 12)
                                .padding(.vertical, 6)
                                .background(Color.red)
                                .foregroundColor(.white)
                                .cornerRadius(8)
                        }
                    }
                    
                    // Description
                    Text(challenge.description)
                        .font(.body)
                    
                    // Reward
                    VStack(alignment: .leading, spacing: 8) {
                        Text("Reward")
                            .font(.headline)
                        
                        HStack {
                            Text("\(Int(challenge.reward)) StompCoins")
                                .font(.title2)
                                .bold()
                            
                            if challenge.isSocialMedia {
                                Image(systemName: "globe")
                                    .foregroundColor(.blue)
                            }
                        }
                    }
                    
                    // Time Information
                    VStack(alignment: .leading, spacing: 8) {
                        Text("Time Information")
                            .font(.headline)
                        
                        HStack {
                            VStack(alignment: .leading) {
                                Text("Starts")
                                    .font(.subheadline)
                                    .foregroundColor(.secondary)
                                Text(challenge.startDate, style: .date)
                                    .bold()
                                Text(challenge.startDate, style: .time)
                                    .font(.caption)
                            }
                            
                            Spacer()
                            
                            VStack(alignment: .trailing) {
                                Text("Ends")
                                    .font(.subheadline)
                                    .foregroundColor(.secondary)
                                Text(challenge.endDate, style: .date)
                                    .bold()
                                Text(challenge.endDate, style: .time)
                                    .font(.caption)
                            }
                        }
                    }
                    
                    // Event Information
                    if let event = challenge.event {
                        VStack(alignment: .leading, spacing: 8) {
                            Text("Event")
                                .font(.headline)
                            
                            Text(event.name)
                                .font(.subheadline)
                                .bold()
                            
                            Text(event.location)
                                .font(.caption)
                                .foregroundColor(.secondary)
                        }
                    }
                    
                    // Participation Button
                    if challenge.isLive {
                        Button(action: { showingParticipationConfirmation = true }) {
                            Text("Participate")
                                .bold()
                                .foregroundColor(.white)
                                .frame(maxWidth: .infinity)
                                .padding()
                                .background(Color.blue)
                                .cornerRadius(8)
                        }
                    }
                }
                .padding()
            }
            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                ToolbarItem(placement: .navigationBarTrailing) {
                    Button("Close") {
                        dismiss()
                    }
                }
            }
            .alert("Participate in Challenge", isPresented: $showingParticipationConfirmation) {
                Button("Cancel", role: .cancel) { }
                Button("Participate") {
                    participateInChallenge()
                }
            } message: {
                Text("Are you sure you want to participate in this challenge?")
            }
        }
    }
}

#Preview {
    ChallengesView()
        .modelContainer(for: [Challenge.self, Event.self], inMemory: true)
} 