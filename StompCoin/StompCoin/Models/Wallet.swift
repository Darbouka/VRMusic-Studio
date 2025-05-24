import Foundation
import SwiftData

@Model
final class Wallet {
    var id: UUID
    var balance: Double
    var transactions: [Transaction]
    var lastUpdated: Date
    var isVerified: Bool
    var kycStatus: KYCStatus
    
    init(id: UUID = UUID(), balance: Double = 0.0) {
        self.id = id
        self.balance = balance
        self.transactions = []
        self.lastUpdated = Date()
        self.isVerified = false
        self.kycStatus = .pending
    }
    
    func addTransaction(_ transaction: Transaction) {
        transactions.append(transaction)
        updateBalance()
    }
    
    private func updateBalance() {
        balance = transactions.reduce(0) { $0 + $1.amount }
        lastUpdated = Date()
    }
}

enum KYCStatus: String, Codable {
    case pending
    case inProgress
    case verified
    case rejected
} 