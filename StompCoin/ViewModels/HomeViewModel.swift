import Foundation
import SwiftUI

@MainActor
class HomeViewModel: ObservableObject {
    @Published var activeChallenges: [Challenge] = []
    @Published var topStreamers: [User] = []
    @Published var userRewards: Rewards = Rewards(stompCoins: 0, omCoins: 0, level: 1, experience: 0)
    
    private let challengeService = ChallengeService.shared
    private let userManager = UserManager.shared
    private let rewardsService = RewardsService.shared
    
    init() {
        Task {
            await refreshData()
        }
    }
    
    func refreshData() async {
        do {
            // Aktive Challenges laden
            activeChallenges = try await challengeService.getActiveChallenges()
            
            // Top Streamer laden
            topStreamers = try await userManager.getTopStreamers()
            
            // Benutzer-Belohnungen laden
            if let currentUser = userManager.getCurrentUser() {
                userRewards = try await rewardsService.getUserRewards(userId: currentUser.id)
            }
        } catch {
            print("Fehler beim Laden der Daten: \(error)")
        }
    }
} 