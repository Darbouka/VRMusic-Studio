import Foundation
import StoreKit

class PaymentManager: ObservableObject {
    static let shared = PaymentManager()
    
    @Published var products: [Product] = []
    @Published var purchasedSubscriptions: [Product] = []
    
    private var updateListenerTask: Task<Void, Error>?
    
    private init() {
        updateListenerTask = listenForTransactions()
        
        Task {
            await loadProducts()
        }
    }
    
    deinit {
        updateListenerTask?.cancel()
    }
    
    // MARK: - Product Loading
    @MainActor
    func loadProducts() async {
        do {
            let storeProducts = try await Product.products(for: [
                "com.stompcoin.subscription.3months",
                "com.stompcoin.subscription.6months",
                "com.stompcoin.subscription.12months",
                "com.stompcoin.weekendboost",
                "com.stompcoin.weekboost"
            ])
            
            products = storeProducts.sorted { $0.price < $1.price }
        } catch {
            print("Failed to load products: \(error)")
        }
    }
    
    // MARK: - Purchase Handling
    func purchase(_ product: Product) async throws -> Transaction? {
        let result = try await product.purchase()
        
        switch result {
        case .success(let verification):
            let transaction = try checkVerified(verification)
            await transaction.finish()
            await updatePurchasedProducts()
            return transaction
            
        case .userCancelled:
            return nil
            
        case .pending:
            return nil
            
        @unknown default:
            return nil
        }
    }
    
    func checkVerified<T>(_ result: VerificationResult<T>) throws -> T {
        switch result {
        case .unverified:
            throw StoreError.failedVerification
        case .verified(let safe):
            return safe
        }
    }
    
    // MARK: - Transaction Updates
    private func listenForTransactions() -> Task<Void, Error> {
        return Task.detached {
            for await result in Transaction.updates {
                do {
                    let transaction = try self.checkVerified(result)
                    await self.updatePurchasedProducts()
                    await transaction.finish()
                } catch {
                    print("Transaction failed verification")
                }
            }
        }
    }
    
    @MainActor
    private func updatePurchasedProducts() async {
        var purchasedSubscriptions: [Product] = []
        
        for await result in Transaction.currentEntitlements {
            do {
                let transaction = try checkVerified(result)
                
                if let subscription = products.first(where: { $0.id == transaction.productID }) {
                    purchasedSubscriptions.append(subscription)
                }
            } catch {
                print("Failed to verify transaction")
            }
        }
        
        self.purchasedSubscriptions = purchasedSubscriptions
    }
    
    // MARK: - Subscription Status
    func isSubscribed(to productId: String) -> Bool {
        return purchasedSubscriptions.contains { $0.id == productId }
    }
    
    func getSubscriptionStatus() -> SubscriptionStatus {
        if purchasedSubscriptions.isEmpty {
            return .none
        }
        
        let sortedSubscriptions = purchasedSubscriptions.sorted { $0.price > $1.price }
        if let highestTier = sortedSubscriptions.first {
            switch highestTier.id {
            case "com.stompcoin.subscription.12months":
                return .yearly
            case "com.stompcoin.subscription.6months":
                return .sixMonths
            case "com.stompcoin.subscription.3months":
                return .threeMonths
            default:
                return .none
            }
        }
        
        return .none
    }
}

// MARK: - Supporting Types
enum StoreError: Error {
    case failedVerification
}

enum SubscriptionStatus {
    case none
    case threeMonths
    case sixMonths
    case yearly
} 