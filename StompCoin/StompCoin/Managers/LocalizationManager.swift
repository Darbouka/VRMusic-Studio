import Foundation

class LocalizationManager {
    static let shared = LocalizationManager()
    
    private var currentLocale: Locale
    private var supportedLocales: [Locale]
    private var currencyFormatter: NumberFormatter
    private var dateFormatter: DateFormatter
    
    // MARK: - Initialisierung
    
    init() {
        // Standardmäßig System-Locale verwenden
        currentLocale = Locale.current
        supportedLocales = [
            Locale(identifier: "de_DE"),
            Locale(identifier: "en_US"),
            Locale(identifier: "fr_FR"),
            Locale(identifier: "es_ES"),
            Locale(identifier: "it_IT")
        ]
        
        // Währungsformatierung konfigurieren
        currencyFormatter = NumberFormatter()
        currencyFormatter.numberStyle = .currency
        currencyFormatter.locale = currentLocale
        
        // Datumsformatierung konfigurieren
        dateFormatter = DateFormatter()
        dateFormatter.locale = currentLocale
        dateFormatter.dateStyle = .medium
        dateFormatter.timeStyle = .short
    }
    
    // MARK: - Locale Management
    
    func setLocale(_ locale: Locale) {
        guard supportedLocales.contains(locale) else { return }
        currentLocale = locale
        updateFormatters()
    }
    
    func getCurrentLocale() -> Locale {
        return currentLocale
    }
    
    func getSupportedLocales() -> [Locale] {
        return supportedLocales
    }
    
    // MARK: - Übersetzungen
    
    func localizedString(_ key: String) -> String {
        return NSLocalizedString(key, comment: "")
    }
    
    func localizedString(_ key: String, arguments: CVarArg...) -> String {
        let format = NSLocalizedString(key, comment: "")
        return String(format: format, arguments: arguments)
    }
    
    // MARK: - Währungsumrechnung
    
    func formatCurrency(_ amount: Double) -> String {
        return currencyFormatter.string(from: NSNumber(value: amount)) ?? "\(amount)"
    }
    
    func convertCurrency(_ amount: Double, from sourceCurrency: String, to targetCurrency: String) async throws -> Double {
        // Hier würde die Integration mit einem Währungs-API erfolgen
        // Beispiel: Fixer.io, ExchangeRate-API, etc.
        return amount // Platzhalter
    }
    
    // MARK: - Datumsformatierung
    
    func formatDate(_ date: Date) -> String {
        return dateFormatter.string(from: date)
    }
    
    func formatDate(_ date: Date, style: DateFormatter.Style) -> String {
        dateFormatter.dateStyle = style
        return dateFormatter.string(from: date)
    }
    
    // MARK: - Regionale Anpassungen
    
    func getRegionalSettings() -> RegionalSettings {
        return RegionalSettings(
            timeZone: TimeZone.current,
            calendar: Calendar.current,
            measurementSystem: getMeasurementSystem(),
            dateFormat: getDateFormat(),
            timeFormat: getTimeFormat()
        )
    }
    
    // MARK: - Hilfsmethoden
    
    private func updateFormatters() {
        currencyFormatter.locale = currentLocale
        dateFormatter.locale = currentLocale
    }
    
    private func getMeasurementSystem() -> MeasurementSystem {
        let locale = currentLocale
        if locale.usesMetricSystem {
            return .metric
        } else {
            return .imperial
        }
    }
    
    private func getDateFormat() -> String {
        return dateFormatter.dateFormat ?? "dd.MM.yyyy"
    }
    
    private func getTimeFormat() -> String {
        return dateFormatter.timeFormat ?? "HH:mm"
    }
}

// MARK: - Modelle

struct RegionalSettings {
    let timeZone: TimeZone
    let calendar: Calendar
    let measurementSystem: MeasurementSystem
    let dateFormat: String
    let timeFormat: String
}

enum MeasurementSystem {
    case metric
    case imperial
}

// MARK: - Lokalisierte Strings

extension LocalizationManager {
    struct Strings {
        // Allgemein
        static let appName = "StompCoin"
        static let ok = "OK"
        static let cancel = "Abbrechen"
        static let error = "Fehler"
        static let success = "Erfolg"
        
        // Fitness
        static let steps = "Schritte"
        static let calories = "Kalorien"
        static let distance = "Distanz"
        static let activeMinutes = "Aktive Minuten"
        
        // Wallet
        static let balance = "Guthaben"
        static let transactions = "Transaktionen"
        static let send = "Senden"
        static let receive = "Empfangen"
        
        // Social
        static let friends = "Freunde"
        static let groups = "Gruppen"
        static let messages = "Nachrichten"
        static let notifications = "Benachrichtigungen"
        
        // Profile
        static let settings = "Einstellungen"
        static let language = "Sprache"
        static let currency = "Währung"
        static let theme = "Design"
    }
} 