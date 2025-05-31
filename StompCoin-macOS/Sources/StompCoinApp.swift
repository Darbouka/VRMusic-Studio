import SwiftUI
import Alamofire

@main
struct StompCoinApp: App {
    @StateObject private var appState = AppState()
    
    var body: some Scene {
        WindowGroup {
            ContentView()
                .environmentObject(appState)
        }
        .windowStyle(HiddenTitleBarWindowStyle())
        .commands {
            CommandGroup(replacing: .newItem) { }
        }
        .windowToolbarStyle(.unified)
        .windowResizability(.contentSize)
    }
}

@MainActor
class AppState: ObservableObject {
    @Published var isLoggedIn = false
    @Published var currentUser: User?
    @Published var walletBalance: Double = 0.0
    @Published var fitnessData: FitnessData?
    @Published var isLoading = false
    @Published var errorMessage: String?
    @Published var isDemoMode = false
    
    private let baseURL = "http://localhost:3000/api"
    
    func login(email: String, password: String) async throws {
        isLoading = true
        errorMessage = nil
        
        if isDemoMode {
            currentUser = User(id: "demo1", email: "demo@stompcoin.com", username: "DemoUser", walletBalance: 1234.56)
            walletBalance = 1234.56
            isLoggedIn = true
            fitnessData = FitnessData(steps: 9876, calories: 456.7, distance: 7.89, timestamp: Date())
            isLoading = false
            return
        }
        
        do {
            let parameters: [String: Any] = [
                "email": email,
                "password": password
            ]
            
            let response = try await AF.request("\(baseURL)/auth/login",
                                              method: .post,
                                              parameters: parameters,
                                              encoding: JSONEncoding.default)
                .serializingDecodable(LoginResponse.self)
                .value
            
            currentUser = response.user
            walletBalance = response.user.walletBalance
            isLoggedIn = true
            await loadFitnessData()
        } catch {
            errorMessage = "Login fehlgeschlagen: \(error.localizedDescription)"
            throw error
        }
        
        isLoading = false
    }
    
    func loadFitnessData() async {
        if isDemoMode {
            fitnessData = FitnessData(steps: 9876, calories: 456.7, distance: 7.89, timestamp: Date())
            return
        }
        guard let userId = currentUser?.id else { return }
        
        do {
            let response = try await AF.request("\(baseURL)/fitness/\(userId)",
                                              method: .get)
                .serializingDecodable(FitnessData.self)
                .value
            
            fitnessData = response
        } catch {
            print("Fehler beim Laden der Fitness-Daten: \(error)")
        }
    }
    
    func logout() {
        isLoggedIn = false
        currentUser = nil
        fitnessData = nil
        walletBalance = 0.0
    }
}

struct User: Codable {
    let id: String
    let email: String
    let username: String
    var walletBalance: Double
}

struct FitnessData: Codable {
    let steps: Int
    let calories: Double
    let distance: Double
    let timestamp: Date
}

struct LoginResponse: Codable {
    let user: User
    let token: String
} 