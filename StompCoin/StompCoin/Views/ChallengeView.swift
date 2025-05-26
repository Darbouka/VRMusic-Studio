import SwiftUI

struct ChallengeView: View {
    @StateObject private var challengeService = ChallengeService.shared
    @State private var selectedType: ChallengeService.ChallengeType?
    @State private var showingJoinConfirmation = false
    @State private var selectedChallenge: ChallengeService.Challenge?
    @State private var showingError = false
    @State private var errorMessage = ""
    
    var filteredChallenges: [ChallengeService.Challenge] {
        challengeService.getChallenges(for: selectedType)
    }
    
    var body: some View {
        List {
            Section {
                Picker("Filter", selection: $selectedType) {
                    Text("Alle").tag(Optional<ChallengeService.ChallengeType>.none)
                    ForEach([ChallengeService.ChallengeType.event,
                            .video,
                            .social], id: \.self) { type in
                        Text(type.rawValue).tag(Optional(type))
                    }
                }
                .pickerStyle(.segmented)
            }
            
            if !challengeService.getUpcomingChallenges().isEmpty {
                Section(header: Text("Bevorstehende Challenges")) {
                    ForEach(challengeService.getUpcomingChallenges()) { challenge in
                        ChallengeRow(challenge: challenge)
                            .onTapGesture {
                                selectedChallenge = challenge
                                showingJoinConfirmation = true
                            }
                    }
                }
            }
            
            if !challengeService.getActiveChallenges().isEmpty {
                Section(header: Text("Aktive Challenges")) {
                    ForEach(challengeService.getActiveChallenges()) { challenge in
                        ChallengeRow(challenge: challenge)
                    }
                }
            }
            
            if !challengeService.getUserChallenges().isEmpty {
                Section(header: Text("Meine Challenges")) {
                    ForEach(challengeService.getUserChallenges()) { challenge in
                        ChallengeRow(challenge: challenge)
                    }
                }
            }
        }
        .navigationTitle("Challenges")
        .alert("Challenge beitreten", isPresented: $showingJoinConfirmation) {
            Button("Abbrechen", role: .cancel) { }
            Button("Beitreten") {
                if let challenge = selectedChallenge {
                    Task {
                        do {
                            try await challengeService.joinChallenge(challenge)
                        } catch {
                            errorMessage = error.localizedDescription
                            showingError = true
                        }
                    }
                }
            }
        } message: {
            if let challenge = selectedChallenge {
                VStack {
                    Text(challenge.title)
                        .font(.headline)
                    Text(challenge.description)
                        .font(.subheadline)
                    Text("Benötigte Token: \(challenge.requiredTokens)")
                    Text("Benötigte StompCoins: \(challenge.requiredStompCoins)")
                }
            }
        }
        .alert("Fehler", isPresented: $showingError) {
            Button("OK", role: .cancel) { }
        } message: {
            Text(errorMessage)
        }
    }
}

struct ChallengeRow: View {
    let challenge: ChallengeService.Challenge
    
    var body: some View {
        VStack(alignment: .leading, spacing: 5) {
            HStack {
                Text(challenge.title)
                    .font(.headline)
                Spacer()
                Text(challenge.type.rawValue)
                    .font(.caption)
                    .foregroundColor(.secondary)
            }
            
            Text(challenge.description)
                .font(.subheadline)
                .foregroundColor(.secondary)
            
            HStack {
                Text("Start: \(challenge.startDate.formatted())")
                    .font(.caption)
                    .foregroundColor(.secondary)
                
                Spacer()
                
                Text(challenge.status.rawValue)
                    .font(.caption)
                    .foregroundColor(statusColor)
            }
            
            HStack {
                Text("Token: \(challenge.requiredTokens)")
                    .font(.caption)
                    .foregroundColor(.orange)
                
                Spacer()
                
                Text("StompCoins: \(challenge.requiredStompCoins)")
                    .font(.caption)
                    .foregroundColor(.green)
            }
        }
        .padding(.vertical, 5)
    }
    
    var statusColor: Color {
        switch challenge.status {
        case .upcoming:
            return .blue
        case .active:
            return .green
        case .completed:
            return .gray
        case .cancelled:
            return .red
        }
    }
}

#Preview {
    NavigationView {
        ChallengeView()
    }
} 