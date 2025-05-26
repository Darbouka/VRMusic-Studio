import SwiftUI

struct TransactionHistoryView: View {
    @StateObject private var payoutService = PayoutService.shared
    @State private var selectedType: PayoutService.TransactionType?
    @State private var showingFilter = false
    
    var filteredTransactions: [PayoutService.Transaction] {
        payoutService.getTransactionHistory(for: selectedType)
    }
    
    var body: some View {
        List {
            Section {
                Picker("Filter", selection: $selectedType) {
                    Text("Alle").tag(Optional<PayoutService.TransactionType>.none)
                    ForEach([PayoutService.TransactionType.developerPayout,
                            .userPayout,
                            .bonusPayout], id: \.self) { type in
                        Text(type.rawValue).tag(Optional(type))
                    }
                }
                .pickerStyle(.segmented)
            }
            
            Section(header: Text("Transaktionen")) {
                ForEach(filteredTransactions, id: \.id) { transaction in
                    TransactionRow(transaction: transaction)
                }
            }
            
            if !payoutService.getPendingTransactions().isEmpty {
                Section(header: Text("Ausstehende Transaktionen")) {
                    ForEach(payoutService.getPendingTransactions(), id: \.id) { transaction in
                        TransactionRow(transaction: transaction)
                    }
                }
            }
            
            if !payoutService.getFailedTransactions().isEmpty {
                Section(header: Text("Fehlgeschlagene Transaktionen")) {
                    ForEach(payoutService.getFailedTransactions(), id: \.id) { transaction in
                        TransactionRow(transaction: transaction)
                    }
                }
            }
        }
        .navigationTitle("Transaktionshistorie")
    }
}

struct TransactionRow: View {
    let transaction: PayoutService.Transaction
    
    var body: some View {
        VStack(alignment: .leading, spacing: 5) {
            HStack {
                Text(transaction.type.rawValue)
                    .font(.headline)
                Spacer()
                Text(transaction.amount, format: .currency(code: "EUR"))
                    .font(.headline)
                    .foregroundColor(.green)
            }
            
            Text(transaction.description)
                .font(.subheadline)
                .foregroundColor(.secondary)
            
            HStack {
                Text(transaction.date.formatted())
                    .font(.caption)
                    .foregroundColor(.secondary)
                
                Spacer()
                
                Text(transaction.status.rawValue)
                    .font(.caption)
                    .foregroundColor(statusColor)
            }
        }
        .padding(.vertical, 5)
    }
    
    var statusColor: Color {
        switch transaction.status {
        case .pending:
            return .orange
        case .completed:
            return .green
        case .failed:
            return .red
        }
    }
}

#Preview {
    NavigationView {
        TransactionHistoryView()
    }
} 