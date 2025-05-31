import SwiftUI

struct ContentView: View {
    @EnvironmentObject var appState: AppState
    @State private var selectedTab = 0
    
    var body: some View {
        if appState.isLoggedIn {
            TabView(selection: $selectedTab) {
                DashboardView()
                    .tabItem {
                        Label("Dashboard", systemImage: "chart.bar.fill")
                    }
                    .tag(0)
                
                FitnessView()
                    .tabItem {
                        Label("Fitness", systemImage: "figure.walk")
                    }
                    .tag(1)
                
                WalletView()
                    .tabItem {
                        Label("Wallet", systemImage: "creditcard.fill")
                    }
                    .tag(2)
                
                SocialView()
                    .tabItem {
                        Label("Social", systemImage: "person.2.fill")
                    }
                    .tag(3)
                
                SettingsView()
                    .tabItem {
                        Label("Einstellungen", systemImage: "gear")
                    }
                    .tag(4)
            }
            .frame(minWidth: 800, minHeight: 600)
        } else {
            LoginView()
                .frame(width: 400, height: 500)
        }
    }
}

struct DashboardView: View {
    @EnvironmentObject var appState: AppState
    
    var body: some View {
        ScrollView {
            VStack(spacing: 20) {
                Text("Willkommen, \(appState.currentUser?.username ?? "")")
                    .font(.title)
                    .padding(.top)
                
                HStack(spacing: 20) {
                    StatCard(title: "Schritte", value: "\(appState.fitnessData?.steps ?? 0)", icon: "figure.walk")
                    StatCard(title: "Kalorien", value: "\(Int(appState.fitnessData?.calories ?? 0))", icon: "flame.fill")
                    StatCard(title: "Distanz", value: "\(String(format: "%.2f", appState.fitnessData?.distance ?? 0)) km", icon: "map.fill")
                }
                .padding(.horizontal)
                
                WalletCard(balance: appState.walletBalance)
                    .padding(.horizontal)
                
                RecentActivityView()
                    .padding(.horizontal)
            }
            .padding()
        }
    }
}

struct StatCard: View {
    let title: String
    let value: String
    let icon: String
    
    var body: some View {
        VStack(spacing: 10) {
            Image(systemName: icon)
                .font(.system(size: 30))
                .foregroundColor(.blue)
            
            Text(title)
                .font(.headline)
            
            Text(value)
                .font(.title2)
                .bold()
        }
        .frame(maxWidth: .infinity)
        .padding()
        .background(Color(.windowBackgroundColor))
        .cornerRadius(15)
        .shadow(radius: 5)
    }
}

struct WalletCard: View {
    let balance: Double
    
    var body: some View {
        VStack(spacing: 15) {
            Text("Wallet Balance")
                .font(.headline)
            
            Text("\(String(format: "%.2f", balance)) STOMP")
                .font(.system(size: 36, weight: .bold))
                .foregroundColor(.green)
            
            HStack(spacing: 20) {
                Button(action: {}) {
                    Label("Senden", systemImage: "arrow.up.right")
                }
                .buttonStyle(.bordered)
                
                Button(action: {}) {
                    Label("Empfangen", systemImage: "arrow.down.left")
                }
                .buttonStyle(.bordered)
            }
        }
        .frame(maxWidth: .infinity)
        .padding()
        .background(Color(.windowBackgroundColor))
        .cornerRadius(15)
        .shadow(radius: 5)
    }
}

struct RecentActivityView: View {
    var body: some View {
        VStack(alignment: .leading, spacing: 15) {
            Text("Letzte Aktivitäten")
                .font(.headline)
            
            ForEach(0..<3) { _ in
                HStack {
                    Image(systemName: "figure.walk")
                        .foregroundColor(.blue)
                    
                    VStack(alignment: .leading) {
                        Text("Tägliches Training")
                            .font(.subheadline)
                        Text("Vor 2 Stunden")
                            .font(.caption)
                            .foregroundColor(.gray)
                    }
                    
                    Spacer()
                    
                    Text("+50 STOMP")
                        .foregroundColor(.green)
                }
                .padding()
                .background(Color(.windowBackgroundColor))
                .cornerRadius(10)
            }
        }
    }
}

struct LoginView: View {
    @EnvironmentObject var appState: AppState
    @State private var email = ""
    @State private var password = ""
    
    var body: some View {
        VStack(spacing: 30) {
            Image(systemName: "figure.walk")
                .font(.system(size: 80))
                .foregroundColor(.blue)
            
            Text("Stomp Coin")
                .font(.system(size: 40, weight: .bold))
            
            VStack(spacing: 20) {
                TextField("E-Mail", text: $email)
                    .textFieldStyle(RoundedBorderTextFieldStyle())
                
                SecureField("Passwort", text: $password)
                    .textFieldStyle(RoundedBorderTextFieldStyle())
                
                Toggle("Demo-Modus aktivieren", isOn: $appState.isDemoMode)
                    .toggleStyle(.switch)
                
                if let error = appState.errorMessage {
                    Text(error)
                        .foregroundColor(.red)
                        .font(.caption)
                }
                
                Button(action: login) {
                    if appState.isLoading {
                        ProgressView()
                            .progressViewStyle(CircularProgressViewStyle())
                    } else {
                        Text(appState.isDemoMode ? "Demo starten" : "Anmelden")
                            .frame(maxWidth: .infinity)
                    }
                }
                .buttonStyle(.borderedProminent)
                .disabled(appState.isLoading)
            }
            .padding(.horizontal, 40)
        }
    }
    
    private func login() {
        Task {
            do {
                try await appState.login(email: email, password: password)
            } catch {
                // Fehler wird bereits in AppState behandelt
            }
        }
    }
}

// Platzhalter-Views für die anderen Tabs
struct FitnessView: View {
    var body: some View {
        Text("Fitness View")
    }
}

struct WalletView: View {
    var body: some View {
        Text("Wallet View")
    }
}

struct SocialView: View {
    var body: some View {
        Text("Social View")
    }
}

struct SettingsView: View {
    var body: some View {
        Text("Settings View")
    }
} 