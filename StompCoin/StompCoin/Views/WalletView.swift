import SwiftUI
import SwiftData

struct WalletView: View {
    @StateObject private var viewModel = WalletViewModel()
    @State private var showingSendSheet = false
    @State private var showingReceiveSheet = false
    @State private var showingSettings = false
    
    var body: some View {
        NavigationView {
            ScrollView {
                VStack(spacing: 20) {
                    // Gesamtbilanz
                    BalanceCard(
                        totalBalance: viewModel.totalBalance,
                        currency: viewModel.selectedCurrency
                    )
                    
                    // Krypto-Assets
                    AssetsCard(assets: viewModel.cryptoAssets)
                    
                    // Transaktionshistorie
                    TransactionsCard(transactions: viewModel.recentTransactions)
                }
                .padding()
            }
            .navigationTitle("Wallet")
            .toolbar {
                ToolbarItem(placement: .navigationBarTrailing) {
                    Button(action: { showingSettings = true }) {
                        Image(systemName: "gear")
                    }
                }
            }
            .toolbar {
                ToolbarItem(placement: .bottomBar) {
                    HStack {
                        Button(action: { showingSendSheet = true }) {
                            Label("Senden", systemImage: "arrow.up.circle.fill")
                        }
                        
                        Spacer()
                        
                        Button(action: { showingReceiveSheet = true }) {
                            Label("Empfangen", systemImage: "arrow.down.circle.fill")
                        }
                    }
                    .padding()
                }
            }
            .sheet(isPresented: $showingSendSheet) {
                SendView(viewModel: viewModel)
            }
            .sheet(isPresented: $showingReceiveSheet) {
                ReceiveView(viewModel: viewModel)
            }
            .sheet(isPresented: $showingSettings) {
                WalletSettingsView(viewModel: viewModel)
            }
            .refreshable {
                await viewModel.refresh()
            }
        }
    }
}

struct BalanceCard: View {
    let totalBalance: Double
    let currency: String
    
    var body: some View {
        VStack(spacing: 15) {
            Text("Gesamtbilanz")
                .font(.headline)
            
            Text(String(format: "%.2f %@", totalBalance, currency))
                .font(.system(size: 36, weight: .bold))
            
            HStack {
                Text("+2.5%")
                    .foregroundColor(.green)
                
                Text("24h")
                    .foregroundColor(.gray)
            }
            .font(.subheadline)
        }
        .padding()
        .frame(maxWidth: .infinity)
        .background(Color(.systemBackground))
        .cornerRadius(15)
        .shadow(radius: 5)
    }
}

struct AssetsCard: View {
    let assets: [CryptoAsset]
    
    var body: some View {
        VStack(alignment: .leading, spacing: 15) {
            Text("Assets")
                .font(.headline)
            
            ForEach(assets) { asset in
                AssetRow(asset: asset)
            }
        }
        .padding()
        .background(Color(.systemBackground))
        .cornerRadius(15)
        .shadow(radius: 5)
    }
}

struct AssetRow: View {
    let asset: CryptoAsset
    
    var body: some View {
        HStack {
            AsyncImage(url: URL(string: asset.iconURL)) { image in
                image.resizable()
            } placeholder: {
                Color.gray
            }
            .frame(width: 40, height: 40)
            .clipShape(Circle())
            
            VStack(alignment: .leading) {
                Text(asset.symbol)
                    .font(.headline)
                
                Text(asset.name)
                    .font(.caption)
                    .foregroundColor(.gray)
            }
            
            Spacer()
            
            VStack(alignment: .trailing) {
                Text(String(format: "%.2f %@", asset.balance, asset.symbol))
                    .font(.headline)
                
                Text(String(format: "%.2f %@", asset.value, asset.currency))
                    .font(.caption)
                    .foregroundColor(.gray)
            }
        }
        .padding(.vertical, 5)
    }
}

struct TransactionsCard: View {
    let transactions: [Transaction]
    
    var body: some View {
        VStack(alignment: .leading, spacing: 15) {
            Text("Transaktionen")
                .font(.headline)
            
            ForEach(transactions) { transaction in
                TransactionRow(transaction: transaction)
            }
        }
        .padding()
        .background(Color(.systemBackground))
        .cornerRadius(15)
        .shadow(radius: 5)
    }
}

struct TransactionRow: View {
    let transaction: Transaction
    
    var body: some View {
        HStack {
            Image(systemName: transaction.type.icon)
                .font(.title2)
                .foregroundColor(transaction.type.color)
            
            VStack(alignment: .leading) {
                Text(transaction.type.description)
                    .font(.headline)
                
                Text(transaction.date, style: .time)
                    .font(.caption)
                    .foregroundColor(.gray)
            }
            
            Spacer()
            
            VStack(alignment: .trailing) {
                Text(String(format: "%.2f %@", transaction.amount, transaction.symbol))
                    .font(.headline)
                
                Text(String(format: "%.2f %@", transaction.value, transaction.currency))
                    .font(.caption)
                    .foregroundColor(.gray)
            }
        }
        .padding(.vertical, 5)
    }
}

struct SendView: View {
    @ObservedObject var viewModel: WalletViewModel
    @Environment(\.dismiss) var dismiss
    @State private var amount = ""
    @State private var address = ""
    @State private var selectedAsset: CryptoAsset?
    
    var body: some View {
        NavigationView {
            Form {
                Section(header: Text("Asset")) {
                    Picker("Asset", selection: $selectedAsset) {
                        ForEach(viewModel.cryptoAssets) { asset in
                            Text(asset.symbol).tag(asset as CryptoAsset?)
                        }
                    }
                }
                
                Section(header: Text("Betrag")) {
                    TextField("0.00", text: $amount)
                        .keyboardType(.decimalPad)
                }
                
                Section(header: Text("Empfänger")) {
                    TextField("Adresse", text: $address)
                }
                
                Section {
                    Button("Senden") {
                        // Implementierung des Sendevorgangs
                        dismiss()
                    }
                    .frame(maxWidth: .infinity)
                    .foregroundColor(.blue)
                }
            }
            .navigationTitle("Senden")
            .navigationBarItems(trailing: Button("Abbrechen") {
                dismiss()
            })
        }
    }
}

struct ReceiveView: View {
    @ObservedObject var viewModel: WalletViewModel
    @Environment(\.dismiss) var dismiss
    
    var body: some View {
        NavigationView {
            VStack(spacing: 20) {
                // QR Code
                Image(systemName: "qrcode")
                    .font(.system(size: 200))
                    .foregroundColor(.blue)
                
                // Adresse
                Text(viewModel.walletAddress)
                    .font(.headline)
                    .padding()
                    .background(Color(.systemGray6))
                    .cornerRadius(10)
                
                // Kopieren Button
                Button(action: {
                    UIPasteboard.general.string = viewModel.walletAddress
                }) {
                    Label("Adresse kopieren", systemImage: "doc.on.doc")
                }
                .padding()
            }
            .padding()
            .navigationTitle("Empfangen")
            .navigationBarItems(trailing: Button("Fertig") {
                dismiss()
            })
        }
    }
}

struct WalletSettingsView: View {
    @ObservedObject var viewModel: WalletViewModel
    @Environment(\.dismiss) var dismiss
    
    var body: some View {
        NavigationView {
            Form {
                Section(header: Text("Währung")) {
                    Picker("Anzeigewährung", selection: $viewModel.selectedCurrency) {
                        ForEach(viewModel.availableCurrencies, id: \.self) { currency in
                            Text(currency).tag(currency)
                        }
                    }
                }
                
                Section(header: Text("Sicherheit")) {
                    Toggle("Biometrische Authentifizierung", isOn: $viewModel.biometricAuth)
                    Toggle("Transaktionsbestätigung", isOn: $viewModel.transactionConfirmation)
                }
                
                Section(header: Text("Benachrichtigungen")) {
                    Toggle("Transaktionsbenachrichtigungen", isOn: $viewModel.transactionNotifications)
                    Toggle("Preisbenachrichtigungen", isOn: $viewModel.priceNotifications)
                }
            }
            .navigationTitle("Einstellungen")
            .navigationBarItems(trailing: Button("Fertig") {
                dismiss()
            })
        }
    }
}

// MARK: - ViewModel

class WalletViewModel: ObservableObject {
    @Published var totalBalance: Double = 0.0
    @Published var selectedCurrency = "EUR"
    @Published var cryptoAssets: [CryptoAsset] = []
    @Published var recentTransactions: [Transaction] = []
    @Published var walletAddress = ""
    
    @Published var biometricAuth = true
    @Published var transactionConfirmation = true
    @Published var transactionNotifications = true
    @Published var priceNotifications = true
    
    let availableCurrencies = ["EUR", "USD", "GBP", "JPY"]
    
    init() {
        loadWalletData()
    }
    
    func refresh() async {
        // Implementierung der Datenaktualisierung
    }
    
    private func loadWalletData() {
        // Implementierung des Wallet-Datenladens
    }
}

// MARK: - Modelle

struct CryptoAsset: Identifiable {
    let id = UUID()
    let name: String
    let symbol: String
    let iconURL: String
    let balance: Double
    let value: Double
    let currency: String
}

struct Transaction: Identifiable {
    let id = UUID()
    let type: TransactionType
    let amount: Double
    let symbol: String
    let value: Double
    let currency: String
    let date: Date
}

enum TransactionType {
    case send
    case receive
    case mining
    
    var icon: String {
        switch self {
        case .send: return "arrow.up.circle.fill"
        case .receive: return "arrow.down.circle.fill"
        case .mining: return "hammer.fill"
        }
    }
    
    var description: String {
        switch self {
        case .send: return "Gesendet"
        case .receive: return "Empfangen"
        case .mining: return "Mining"
        }
    }
    
    var color: Color {
        switch self {
        case .send: return .red
        case .receive: return .green
        case .mining: return .blue
        }
    }
}

#Preview {
    WalletView()
        .modelContainer(for: [User.self, Transaction.self], inMemory: true)
} 
} 
