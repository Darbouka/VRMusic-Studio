import SwiftUI
import SwiftData

struct WalletView: View {
    @Environment(\.modelContext) private var modelContext
    @Query private var users: [User]
    @Query private var transactions: [Transaction]
    @State private var showingPurchaseSheet = false
    @State private var showingConversionSheet = false
    @State private var selectedTab = 0
    
    var currentUser: User? {
        users.first
    }
    
    var filteredTransactions: [Transaction] {
        transactions.filter { $0.user.id == currentUser?.id }
    }
    
    var body: some View {
        NavigationView {
            VStack {
                // Coin Balance Cards
                HStack(spacing: 20) {
                    CoinBalanceCard(
                        title: "StompCoins",
                        amount: currentUser?.stompCoins ?? 0,
                        color: .blue
                    )
                    
                    CoinBalanceCard(
                        title: "OM Coins",
                        amount: currentUser?.omCoins ?? 0,
                        color: .purple
                    )
                }
                .padding()
                
                // Action Buttons
                HStack(spacing: 20) {
                    ActionButton(
                        title: "Buy Coins",
                        systemImage: "plus.circle.fill",
                        action: { showingPurchaseSheet = true }
                    )
                    
                    ActionButton(
                        title: "Convert",
                        systemImage: "arrow.triangle.2.circlepath",
                        action: { showingConversionSheet = true }
                    )
                }
                .padding(.horizontal)
                
                // Transaction History
                VStack(alignment: .leading) {
                    Text("Transaction History")
                        .font(.headline)
                        .padding(.horizontal)
                    
                    List {
                        ForEach(filteredTransactions) { transaction in
                            TransactionRow(transaction: transaction)
                        }
                    }
                }
            }
            .navigationTitle("Wallet")
            .sheet(isPresented: $showingPurchaseSheet) {
                PurchaseView(user: currentUser)
            }
            .sheet(isPresented: $showingConversionSheet) {
                ConversionView(user: currentUser)
            }
        }
    }
}

struct CoinBalanceCard: View {
    let title: String
    let amount: Double
    let color: Color
    
    var body: some View {
        VStack {
            Text(title)
                .font(.headline)
            
            Text("\(Int(amount))")
                .font(.system(size: 32, weight: .bold))
            
            Text("Available")
                .font(.caption)
                .foregroundColor(.secondary)
        }
        .frame(maxWidth: .infinity)
        .padding()
        .background(color.opacity(0.1))
        .cornerRadius(12)
    }
}

struct ActionButton: View {
    let title: String
    let systemImage: String
    let action: () -> Void
    
    var body: some View {
        Button(action: action) {
            VStack {
                Image(systemName: systemImage)
                    .font(.title2)
                Text(title)
                    .font(.caption)
            }
            .frame(maxWidth: .infinity)
            .padding()
            .background(Color.blue.opacity(0.1))
            .cornerRadius(12)
        }
    }
}

struct TransactionRow: View {
    let transaction: Transaction
    
    var body: some View {
        HStack {
            VStack(alignment: .leading) {
                Text(transaction.type.rawValue.capitalized)
                    .font(.headline)
                
                Text(transaction.date, style: .date)
                    .font(.caption)
                    .foregroundColor(.secondary)
            }
            
            Spacer()
            
            VStack(alignment: .trailing) {
                Text("\(transaction.amount, specifier: "%.2f") \(transaction.currency.rawValue)")
                    .font(.subheadline)
                    .bold()
                
                Text(transaction.status.rawValue.capitalized)
                    .font(.caption)
                    .foregroundColor(statusColor)
            }
        }
        .padding(.vertical, 4)
    }
    
    var statusColor: Color {
        switch transaction.status {
        case .completed:
            return .green
        case .pending:
            return .orange
        case .failed:
            return .red
        case .refunded:
            return .blue
        }
    }
}

struct PurchaseView: View {
    @Environment(\.dismiss) private var dismiss
    let user: User?
    @State private var selectedAmount = 100.0
    @State private var selectedPaymentMethod = PaymentMethod.creditCard
    
    let availableAmounts = [100.0, 500.0, 1000.0, 5000.0]
    
    var body: some View {
        NavigationView {
            Form {
                Section("Amount") {
                    Picker("Select Amount", selection: $selectedAmount) {
                        ForEach(availableAmounts, id: \.self) { amount in
                            Text("\(Int(amount)) StompCoins")
                        }
                    }
                }
                
                Section("Payment Method") {
                    Picker("Select Payment Method", selection: $selectedPaymentMethod) {
                        ForEach(PaymentMethod.allCases, id: \.self) { method in
                            Text(method.rawValue.capitalized)
                        }
                    }
                }
                
                Section {
                    Button("Purchase") {
                        // TODO: Implement purchase
                        dismiss()
                    }
                }
            }
            .navigationTitle("Buy StompCoins")
            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                ToolbarItem(placement: .navigationBarTrailing) {
                    Button("Cancel") {
                        dismiss()
                    }
                }
            }
        }
    }
}

struct ConversionView: View {
    @Environment(\.dismiss) private var dismiss
    let user: User?
    @State private var amount = ""
    @State private var isConvertingToOM = true
    
    var body: some View {
        NavigationView {
            Form {
                Section {
                    TextField("Amount", text: $amount)
                        .keyboardType(.decimalPad)
                    
                    Picker("Conversion Direction", selection: $isConvertingToOM) {
                        Text("StompCoins to OM Coins").tag(true)
                        Text("OM Coins to StompCoins").tag(false)
                    }
                }
                
                Section {
                    Button("Convert") {
                        // TODO: Implement conversion
                        dismiss()
                    }
                }
            }
            .navigationTitle("Convert Coins")
            .navigationBarTitleDisplayMode(.inline)
            .toolbar {
                ToolbarItem(placement: .navigationBarTrailing) {
                    Button("Cancel") {
                        dismiss()
                    }
                }
            }
        }
    }
}

#Preview {
    WalletView()
        .modelContainer(for: [User.self, Transaction.self], inMemory: true)
} 