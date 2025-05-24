import Foundation
import Combine

enum NetworkError: Error {
    case invalidURL
    case noData
    case decodingError
    case serverError(String)
    case unauthorized
    case networkError(Error)
}

class NetworkManager {
    static let shared = NetworkManager()
    private let baseURL = "https://api.stompcoin.com/v1"
    private var authToken: String?
    
    private init() {}
    
    // MARK: - Authentication
    func login(email: String, password: String) async throws -> User {
        let endpoint = "\(baseURL)/auth/login"
        let body = ["email": email, "password": password]
        
        let (data, response) = try await makeRequest(endpoint: endpoint, method: "POST", body: body)
        
        guard let httpResponse = response as? HTTPURLResponse else {
            throw NetworkError.serverError("Invalid response")
        }
        
        if httpResponse.statusCode == 401 {
            throw NetworkError.unauthorized
        }
        
        let user = try JSONDecoder().decode(User.self, from: data)
        self.authToken = user.token
        return user
    }
    
    func register(username: String, email: String, password: String) async throws -> User {
        let endpoint = "\(baseURL)/auth/register"
        let body = [
            "username": username,
            "email": email,
            "password": password
        ]
        
        let (data, _) = try await makeRequest(endpoint: endpoint, method: "POST", body: body)
        return try JSONDecoder().decode(User.self, from: data)
    }
    
    // MARK: - Steps & Coins
    func syncSteps(_ steps: Int) async throws {
        let endpoint = "\(baseURL)/steps/sync"
        let body = ["count": steps]
        _ = try await makeRequest(endpoint: endpoint, method: "POST", body: body)
    }
    
    func getCoinBalance() async throws -> Double {
        let endpoint = "\(baseURL)/wallet/balance"
        let (data, _) = try await makeRequest(endpoint: endpoint, method: "GET")
        let response = try JSONDecoder().decode(WalletResponse.self, from: data)
        return response.balance
    }
    
    // MARK: - Events & Challenges
    func getNearbyEvents() async throws -> [Event] {
        let endpoint = "\(baseURL)/events/nearby"
        let (data, _) = try await makeRequest(endpoint: endpoint, method: "GET")
        return try JSONDecoder().decode([Event].self, from: data)
    }
    
    func joinEvent(_ eventId: UUID) async throws {
        let endpoint = "\(baseURL)/events/\(eventId)/join"
        _ = try await makeRequest(endpoint: endpoint, method: "POST")
    }
    
    // MARK: - Leaderboard
    func getLeaderboard(timeFrame: TimeFrame) async throws -> [LeaderboardEntry] {
        let endpoint = "\(baseURL)/leaderboard?timeFrame=\(timeFrame.rawValue)"
        let (data, _) = try await makeRequest(endpoint: endpoint, method: "GET")
        return try JSONDecoder().decode([LeaderboardEntry].self, from: data)
    }
    
    // MARK: - Helper Methods
    private func makeRequest(endpoint: String, method: String, body: [String: Any]? = nil) async throws -> (Data, URLResponse) {
        guard let url = URL(string: endpoint) else {
            throw NetworkError.invalidURL
        }
        
        var request = URLRequest(url: url)
        request.httpMethod = method
        request.setValue("application/json", forHTTPHeaderField: "Content-Type")
        
        if let token = authToken {
            request.setValue("Bearer \(token)", forHTTPHeaderField: "Authorization")
        }
        
        if let body = body {
            request.httpBody = try JSONSerialization.data(withJSONObject: body)
        }
        
        do {
            return try await URLSession.shared.data(for: request)
        } catch {
            throw NetworkError.networkError(error)
        }
    }
}

// MARK: - Supporting Types
enum TimeFrame: String {
    case daily
    case weekly
    case monthly
    case allTime
}

struct WalletResponse: Codable {
    let balance: Double
}

struct LeaderboardEntry: Codable {
    let userId: UUID
    let username: String
    let score: Int
    let rank: Int
} 