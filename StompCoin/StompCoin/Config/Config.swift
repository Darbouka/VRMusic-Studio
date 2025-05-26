import Foundation

enum Environment {
    case development
    case staging
    case production
    
    var baseURL: String {
        switch self {
        case .development:
            return "https://dev-api.stompcoin.com"
        case .staging:
            return "https://staging-api.stompcoin.com"
        case .production:
            return "https://api.stompcoin.com"
        }
    }
}

struct Config {
    static let environment: Environment = .production
    
    // Blockchain
    static let infuraAPIKey = "YOUR_INFURA_API_KEY"
    static let etherscanAPIKey = "YOUR_ETHERSCAN_API_KEY"
    
    // Currency Conversion
    static let fixerAPIKey = "YOUR_FIXER_API_KEY"
    
    // Analytics
    static let mixpanelToken = "YOUR_MIXPANEL_TOKEN"
    static let amplitudeAPIKey = "YOUR_AMPLITUDE_API_KEY"
    
    // Push Notifications
    static let firebaseConfig: [String: Any] = [
        "apiKey": "YOUR_FIREBASE_API_KEY",
        "authDomain": "stompcoin.firebaseapp.com",
        "projectId": "stompcoin",
        "storageBucket": "stompcoin.appspot.com",
        "messagingSenderId": "YOUR_SENDER_ID",
        "appId": "YOUR_APP_ID"
    ]
    
    // Social
    static let twitterAPIKey = "YOUR_TWITTER_API_KEY"
    static let twitterAPISecret = "YOUR_TWITTER_API_SECRET"
    
    // Security
    static let encryptionKey = "YOUR_ENCRYPTION_KEY"
    
    // App Store
    static let appStoreID = "YOUR_APP_STORE_ID"
    static let appStoreURL = "https://apps.apple.com/app/stompcoin/id\(appStoreID)"
    
    // Support
    static let supportEmail = "support@stompcoin.com"
    static let supportWebsite = "https://support.stompcoin.com"
    
    // Legal
    static let privacyPolicyURL = "https://stompcoin.com/privacy"
    static let termsOfServiceURL = "https://stompcoin.com/terms"
    
    // Feature Flags
    static let isMiningEnabled = true
    static let isSocialEnabled = true
    static let isTradingEnabled = true
    static let isOfflineModeEnabled = true
    
    // Cache Settings
    static let cacheExpirationTime: TimeInterval = 3600 // 1 hour
    static let maxCacheSize: Int = 100 * 1024 * 1024 // 100 MB
    
    // Network Settings
    static let requestTimeout: TimeInterval = 30
    static let maxRetryAttempts = 3
    
    // UI Settings
    static let defaultAnimationDuration: TimeInterval = 0.3
    static let defaultCornerRadius: CGFloat = 12
    static let defaultPadding: CGFloat = 16
    
    // Localization
    static let defaultLocale = Locale(identifier: "de_DE")
    static let supportedLocales = ["de_DE", "en_US", "fr_FR", "es_ES"]
    
    // Version
    static let appVersion = Bundle.main.infoDictionary?["CFBundleShortVersionString"] as? String ?? "1.0.0"
    static let buildNumber = Bundle.main.infoDictionary?["CFBundleVersion"] as? String ?? "1"
} 