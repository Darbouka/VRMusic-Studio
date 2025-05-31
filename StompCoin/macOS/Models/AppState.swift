import Foundation
import SwiftUI
import Alamofire

@MainActor
class AppState: ObservableObject {
    @Published var currentUser: User?
    @Published var isAuthenticated = false
    @Published var isLoading = false
    @Published var errorMessage: String?
    @Published var walletBalance: Double = 0.0
    @Published var fitnessData: FitnessData?
    @Published var isDemoMode = false
    @Published var isRegistering = false
    @Published var musicGenres: [MusicGenre] = []
    @Published var leaderboardEntries: [LeaderboardEntry] = []
    @Published var recentActivities: [Activity] = []
    @Published var settings = UserSettings()
    
    private let baseURL = "https://api.stompcoin.com"
    
    init() {
        loadDemoData()
    }
    
    func loadDemoData() {
        musicGenres = [
            MusicGenre(
                id: "1",
                name: "Hip Hop",
                description: "Die Grundlagen des Hip Hop Tanzes",
                icon: "music.note",
                floors: [
                    Floor(
                        id: "1",
                        name: "Basic Steps",
                        image: "hiphop_basic",
                        difficulty: 1,
                        requiredLevel: 1,
                        reward: 100,
                        musicGenre: "Hip Hop",
                        description: "Lerne die grundlegenden Schritte des Hip Hop"
                    )
                ]
            )
        ]
        
        leaderboardEntries = [
            LeaderboardEntry(
                id: "1",
                userId: "1",
                username: "DanceKing",
                score: 1000,
                rank: 1,
                profileImage: "profile_default"
            ),
            LeaderboardEntry(
                id: "2",
                userId: "2",
                username: "RhythmQueen",
                score: 950,
                rank: 2,
                profileImage: "profile_default"
            ),
            LeaderboardEntry(
                id: "3",
                userId: "3",
                username: "BeatMaster",
                score: 900,
                rank: 3,
                profileImage: "profile_default"
            )
        ]
    }
    
    func login(email: String, password: String) async throws {
        isLoading = true
        defer { isLoading = false }
        
        do {
            let response = try await AF.request(
                "\(baseURL)/auth/login",
                method: .post,
                parameters: ["email": email, "password": password],
                encoder: JSONParameterEncoder.default
            ).serializingDecodable(LoginResponse.self).value
            
            currentUser = response.user
            isAuthenticated = true
            errorMessage = nil
            await loadFitnessData()
        } catch {
            errorMessage = "Anmeldung fehlgeschlagen: \(error.localizedDescription)"
            throw error
        }
    }
    
    func register(username: String, email: String, password: String) async throws {
        isLoading = true
        defer { isLoading = false }
        
        do {
            let response = try await AF.request(
                "\(baseURL)/auth/register",
                method: .post,
                parameters: [
                    "username": username,
                    "email": email,
                    "password": password
                ],
                encoder: JSONParameterEncoder.default
            ).serializingDecodable(LoginResponse.self).value
            
            currentUser = response.user
            isAuthenticated = true
            errorMessage = nil
            await loadFitnessData()
        } catch {
            errorMessage = "Registrierung fehlgeschlagen: \(error.localizedDescription)"
            throw error
        }
    }
    
    func logout() {
        currentUser = nil
        isAuthenticated = false
        fitnessData = nil
    }
    
    func deleteAccount() {
        currentUser = nil
        isAuthenticated = false
    }
    
    private func loadFitnessData() async {
        guard let user = currentUser else { return }
        
        do {
            let response = try await AF.request(
                "\(baseURL)/fitness/\(user.id)",
                method: .get
            ).serializingDecodable(FitnessData.self).value
            
            // Update user's fitness data
            currentUser?.fitnessData = response
        } catch {
            errorMessage = "Fehler beim Laden der Fitness-Daten: \(error.localizedDescription)"
        }
    }
}

struct FitnessData: Codable {
    var steps: Int
    var calories: Double
    var distance: Double
    var activeMinutes: Int
    var lastUpdated: Date
}

struct LoginResponse: Codable {
    let user: User
    let token: String
} 