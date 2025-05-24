import Foundation
import StoreKit

class MonetizationManager: NSObject {
    static let shared = MonetizationManager()
    
    private var products: [Product] = []
    private var purchasedProducts: Set<String> = []
    private var adManager: AdManager?
    private var partnerManager: PartnerManager?
    
    // MARK: - In-App-Käufe
    
    func loadProducts() async throws {
        let productIdentifiers = [
            "com.stompcoin.premium.monthly",
            "com.stompcoin.premium.yearly",
            "com.stompcoin.boost.weekly",
            "com.stompcoin.boost.monthly",
            "com.stompcoin.special.theme",
            "com.stompcoin.special.avatar"
        ]
        
        products = try await Product.products(for: productIdentifiers)
    }
    
    func purchaseProduct(_ product: Product) async throws -> Transaction? {
        let result = try await product.purchase()
        
        switch result {
        case .success(let verification):
            let transaction = try checkVerified(verification)
            await transaction.finish()
            purchasedProducts.insert(product.id)
            return transaction
            
        case .userCancelled:
            return nil
            
        case .pending:
            return nil
            
        @unknown default:
            return nil
        }
    }
    
    func restorePurchases() async throws {
        try await AppStore.sync()
    }
    
    func isProductPurchased(_ productId: String) -> Bool {
        return purchasedProducts.contains(productId)
    }
    
    // MARK: - Premium Features
    
    func unlockPremiumFeatures(for user: User) {
        user.isPremium = true
        user.premiumFeatures = [
            .unlimitedTrades,
            .prioritySupport,
            .advancedAnalytics,
            .customThemes,
            .adFree
        ]
    }
    
    func checkPremiumStatus(for user: User) -> Bool {
        return user.isPremium
    }
    
    // MARK: - Werbung
    
    func initializeAds() {
        adManager = AdManager()
        adManager?.configure()
    }
    
    func showBannerAd() {
        adManager?.showBannerAd()
    }
    
    func showInterstitialAd() {
        adManager?.showInterstitialAd()
    }
    
    func showRewardedAd(completion: @escaping (Bool) -> Void) {
        adManager?.showRewardedAd(completion: completion)
    }
    
    // MARK: - Partnerprogramme
    
    func initializePartnerProgram() {
        partnerManager = PartnerManager()
        partnerManager?.configure()
    }
    
    func trackPartnerReferral(user: User, partnerCode: String) {
        partnerManager?.trackReferral(user: user, partnerCode: partnerCode)
    }
    
    func getPartnerEarnings(for user: User) -> Double {
        return partnerManager?.getEarnings(for: user) ?? 0.0
    }
    
    // MARK: - Hilfsmethoden
    
    private func checkVerified<T>(_ result: VerificationResult<T>) throws -> T {
        switch result {
        case .unverified:
            throw MonetizationError.verificationFailed
        case .verified(let safe):
            return safe
        }
    }
}

// MARK: - Ad Manager

class AdManager {
    private var bannerAd: BannerAd?
    private var interstitialAd: InterstitialAd?
    private var rewardedAd: RewardedAd?
    
    func configure() {
        // Werbenetzwerk-Konfiguration
        bannerAd = BannerAd()
        interstitialAd = InterstitialAd()
        rewardedAd = RewardedAd()
    }
    
    func showBannerAd() {
        bannerAd?.show()
    }
    
    func showInterstitialAd() {
        interstitialAd?.show()
    }
    
    func showRewardedAd(completion: @escaping (Bool) -> Void) {
        rewardedAd?.show(completion: completion)
    }
}

// MARK: - Partner Manager

class PartnerManager {
    private var partners: [Partner] = []
    private var referrals: [Referral] = []
    
    func configure() {
        // Partnerprogramm-Konfiguration
        loadPartners()
    }
    
    private func loadPartners() {
        // Partner aus der Datenbank laden
    }
    
    func trackReferral(user: User, partnerCode: String) {
        let referral = Referral(
            userId: user.id,
            partnerCode: partnerCode,
            date: Date(),
            status: .pending
        )
        referrals.append(referral)
    }
    
    func getEarnings(for user: User) -> Double {
        return referrals
            .filter { $0.userId == user.id && $0.status == .completed }
            .reduce(0) { $0 + $1.earnings }
    }
}

// MARK: - Modelle

struct Partner: Codable {
    let id: UUID
    let name: String
    let code: String
    let commission: Double
    let minimumPayout: Double
}

struct Referral: Codable {
    let id: UUID
    let userId: UUID
    let partnerCode: String
    let date: Date
    var status: ReferralStatus
    var earnings: Double
}

enum ReferralStatus: String, Codable {
    case pending
    case completed
    case rejected
}

enum PremiumFeature: String, Codable {
    case unlimitedTrades
    case prioritySupport
    case advancedAnalytics
    case customThemes
    case adFree
}

enum MonetizationError: Error {
    case verificationFailed
    case purchaseFailed
    case productNotFound
    case adNotAvailable
    case partnerNotFound
} 