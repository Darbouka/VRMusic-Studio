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
        let endpoint = "\(Config.exchangeRateAPI)/convert"
        var request = URLRequest(url: URL(string: endpoint)!)
        request.httpMethod = "GET"
        request.setValue(Config.exchangeRateAPIKey, forHTTPHeaderField: "X-API-Key")
        
        let queryItems = [
            URLQueryItem(name: "from", value: sourceCurrency),
            URLQueryItem(name: "to", value: targetCurrency),
            URLQueryItem(name: "amount", value: String(amount))
        ]
        
        request.url?.append(queryItems: queryItems)
        
        let (data, response) = try await URLSession.shared.data(for: request)
        
        guard let httpResponse = response as? HTTPURLResponse,
              httpResponse.statusCode == 200 else {
            throw LocalizationError.conversionFailed
        }
        
        let result = try JSONDecoder().decode(ExchangeRateResponse.self, from: data)
        return result.convertedAmount
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
        let formatter = DateFormatter()
        formatter.locale = currentLocale
        formatter.dateStyle = .medium
        return formatter.dateFormat
    }
    
    private func getTimeFormat() -> String {
        let formatter = DateFormatter()
        formatter.locale = currentLocale
        formatter.timeStyle = .medium
        return formatter.timeFormat
    }
    
    // Lokalisierte Zahlen
    func localizedNumber(_ number: Double) -> String {
        let formatter = NumberFormatter()
        formatter.locale = currentLocale
        formatter.numberStyle = .decimal
        return formatter.string(from: NSNumber(value: number)) ?? String(number)
    }
    
    // Lokalisierte Währungen
    func localizedCurrency(_ amount: Double, currencyCode: String) -> String {
        let formatter = NumberFormatter()
        formatter.locale = currentLocale
        formatter.numberStyle = .currency
        formatter.currencyCode = currencyCode
        return formatter.string(from: NSNumber(value: amount)) ?? String(amount)
    }
    
    // Lokalisierte Datumsangaben
    func localizedDate(_ date: Date, style: DateFormatter.Style) -> String {
        let formatter = DateFormatter()
        formatter.locale = currentLocale
        formatter.dateStyle = style
        return formatter.string(from: date)
    }
    
    // Lokalisierte Zeiten
    func localizedTime(_ date: Date, style: DateFormatter.Style) -> String {
        let formatter = DateFormatter()
        formatter.locale = currentLocale
        formatter.timeStyle = style
        return formatter.string(from: date)
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
        static let appName = "Stomp Coin"
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