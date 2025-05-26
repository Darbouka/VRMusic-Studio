import Foundation

class LocalizationService {
    static let shared = LocalizationService()
    
    private let defaults = UserDefaults.standard
    private let languageKey = "selectedLanguage"
    
    var currentLanguage: String {
        get {
            return defaults.string(forKey: languageKey) ?? Locale.current.languageCode ?? "en"
        }
        set {
            defaults.set(newValue, forKey: languageKey)
            NotificationCenter.default.post(name: .languageDidChange, object: nil)
        }
    }
    
    func localizedString(_ key: String, _ args: CVarArg...) -> String {
        let format = NSLocalizedString(key, comment: "")
        return String(format: format, arguments: args)
    }
    
    func availableLanguages() -> [(code: String, name: String)] {
        return [
            ("de", "Deutsch"),
            ("en", "English"),
            ("es", "Español"),
            ("fr", "Français"),
            ("it", "Italiano"),
            ("pt", "Português"),
            ("ru", "Русский"),
            ("tr", "Türkçe"),
            ("ar", "العربية"),
            ("zh", "中文"),
            ("ja", "日本語"),
            ("ko", "한국어")
        ]
    }
    
    func setLanguage(_ languageCode: String) {
        currentLanguage = languageCode
        UserDefaults.standard.set([languageCode], forKey: "AppleLanguages")
        UserDefaults.standard.synchronize()
    }
}

extension Notification.Name {
    static let languageDidChange = Notification.Name("languageDidChange")
}

// String Extension für einfachere Lokalisierung
extension String {
    var localized: String {
        return NSLocalizedString(self, comment: "")
    }
    
    func localizedFormat(_ args: CVarArg...) -> String {
        return String(format: localized, arguments: args)
    }
} 