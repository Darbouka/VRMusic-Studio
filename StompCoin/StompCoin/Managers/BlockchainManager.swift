import Foundation
import CryptoKit

class BlockchainManager {
    static let shared = BlockchainManager()
    
    private var chain: [Block] = []
    private var pendingTransactions: [Transaction] = []
    
    private init() {
        createGenesisBlock()
    }
    
    private func createGenesisBlock() {
        let genesisBlock = Block(
            timestamp: Date(),
            transactions: [],
            previousHash: "0"
        )
        chain.append(genesisBlock)
    }
    
    func addTransaction(_ transaction: Transaction) {
        pendingTransactions.append(transaction)
    }
    
    func minePendingTransactions() {
        let block = Block(
            timestamp: Date(),
            transactions: pendingTransactions,
            previousHash: chain.last?.hash ?? "0"
        )
        
        chain.append(block)
        pendingTransactions = []
    }
    
    func getBalance(for address: String) -> Double {
        var balance: Double = 0
        
        for block in chain {
            for transaction in block.transactions {
                if transaction.fromAddress == address {
                    balance -= transaction.amount
                }
                if transaction.toAddress == address {
                    balance += transaction.amount
                }
            }
        }
        
        return balance
    }
}

struct Block {
    let timestamp: Date
    let transactions: [Transaction]
    let previousHash: String
    var hash: String {
        let data = "\(timestamp)\(transactions)\(previousHash)".data(using: .utf8)!
        let hash = SHA256.hash(data: data)
        return hash.compactMap { String(format: "%02x", $0) }.joined()
    }
} 