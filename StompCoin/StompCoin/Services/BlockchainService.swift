import Foundation
import web3

class BlockchainService {
    static let shared = BlockchainService()
    
    private let web3: web3
    private let contractAddress: String
    private let contractABI: String
    private let gasLimit: BigUInt = 21000
    private let gasPrice: BigUInt = 20000000000 // 20 Gwei
    
    // MARK: - Initialisierung
    
    init() {
        // Ethereum Mainnet
        let endpoint = "https://mainnet.infura.io/v3/YOUR_INFURA_KEY"
        self.web3 = web3(rpcURL: endpoint)
        self.contractAddress = "YOUR_CONTRACT_ADDRESS"
        self.contractABI = """
        [
            {
                "constant": true,
                "inputs": [],
                "name": "name",
                "outputs": [{"name": "", "type": "string"}],
                "type": "function"
            },
            {
                "constant": true,
                "inputs": [],
                "name": "symbol",
                "outputs": [{"name": "", "type": "string"}],
                "type": "function"
            },
            {
                "constant": true,
                "inputs": [],
                "name": "decimals",
                "outputs": [{"name": "", "type": "uint8"}],
                "type": "function"
            },
            {
                "constant": true,
                "inputs": [{"name": "_owner", "type": "address"}],
                "name": "balanceOf",
                "outputs": [{"name": "balance", "type": "uint256"}],
                "type": "function"
            },
            {
                "constant": false,
                "inputs": [
                    {"name": "_to", "type": "address"},
                    {"name": "_value", "type": "uint256"}
                ],
                "name": "transfer",
                "outputs": [{"name": "", "type": "bool"}],
                "type": "function"
            }
        ]
        """
    }
    
    // MARK: - Wallet Operationen
    
    func getBalance(address: String) async throws -> BigUInt {
        let contract = web3.contract(contractABI, at: contractAddress)!
        let parameters: [Any] = [address]
        let result = try await contract.read("balanceOf", parameters: parameters)!
        return result["balance"] as! BigUInt
    }
    
    func transfer(to address: String, amount: BigUInt, from wallet: EthereumAccount) async throws -> String {
        let contract = web3.contract(contractABI, at: contractAddress)!
        let parameters: [Any] = [address, amount]
        
        let transaction = try await contract.write(
            "transfer",
            parameters: parameters,
            account: wallet,
            gasLimit: gasLimit,
            gasPrice: gasPrice
        )
        
        return transaction.hash
    }
    
    // MARK: - Mining Operationen
    
    func startMining(address: String) async throws -> String {
        let contract = web3.contract(contractABI, at: contractAddress)!
        let parameters: [Any] = [address]
        
        let transaction = try await contract.write(
            "startMining",
            parameters: parameters,
            account: EthereumAccount(keyStorage: KeyStorage()),
            gasLimit: gasLimit,
            gasPrice: gasPrice
        )
        
        return transaction.hash
    }
    
    func stopMining(address: String) async throws -> String {
        let contract = web3.contract(contractABI, at: contractAddress)!
        let parameters: [Any] = [address]
        
        let transaction = try await contract.write(
            "stopMining",
            parameters: parameters,
            account: EthereumAccount(keyStorage: KeyStorage()),
            gasLimit: gasLimit,
            gasPrice: gasPrice
        )
        
        return transaction.hash
    }
    
    // MARK: - Smart Contract Operationen
    
    func getTokenInfo() async throws -> TokenInfo {
        let contract = web3.contract(contractABI, at: contractAddress)!
        
        let name = try await contract.read("name")!["0"] as! String
        let symbol = try await contract.read("symbol")!["0"] as! String
        let decimals = try await contract.read("decimals")!["0"] as! BigUInt
        
        return TokenInfo(
            name: name,
            symbol: symbol,
            decimals: Int(decimals)
        )
    }
    
    // MARK: - Transaktionsüberwachung
    
    func getTransactionStatus(hash: String) async throws -> TransactionStatus {
        let receipt = try await web3.eth.getTransactionReceipt(hash)
        
        if receipt.status == 1 {
            return .success
        } else if receipt.status == 0 {
            return .failed
        } else {
            return .pending
        }
    }
    
    func getTransactionHistory(address: String) async throws -> [Transaction] {
        let events = try await web3.eth.getLogs(
            address: contractAddress,
            topics: [
                "0xddf252ad1be2c89b69c2b068fc378daa952ba7f163c4a11628f55a4df523b3ef" // Transfer event
            ]
        )
        
        return events.compactMap { event in
            guard let from = event.topics[1],
                  let to = event.topics[2],
                  let data = event.data else {
                return nil
            }
            
            return Transaction(
                hash: event.transactionHash,
                from: from,
                to: to,
                value: BigUInt(data),
                blockNumber: event.blockNumber,
                timestamp: Date()
            )
        }
    }
}

// MARK: - Modelle

struct TokenInfo {
    let name: String
    let symbol: String
    let decimals: Int
}

struct Transaction {
    let hash: String
    let from: String
    let to: String
    let value: BigUInt
    let blockNumber: BigUInt
    let timestamp: Date
}

enum TransactionStatus {
    case pending
    case success
    case failed
}

// MARK: - Fehler

enum BlockchainError: Error {
    case invalidAddress
    case insufficientFunds
    case transactionFailed
    case networkError
    case contractError
} 