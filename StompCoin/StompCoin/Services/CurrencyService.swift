import Foundation

class CurrencyService {
    static let shared = CurrencyService()
    
    private let apiKey = "YOUR_FIXER_API_KEY"
    private let baseURL = "http://data.fixer.io/api"
    private let cache = NSCache<NSString, NSData>()
    private let cacheTimeout: TimeInterval = 3600 // 1 Stunde
    
    // MARK: - Währungsumrechnung
    
    func convertCurrency(_ amount: Double, from sourceCurrency: String, to targetCurrency: String) async throws -> Double {
        let cacheKey = "\(sourceCurrency)_\(targetCurrency)" as NSString
        
        if let cachedData = cache.object(forKey: cacheKey),
           let exchangeRate = try? JSONDecoder().decode(ExchangeRate.self, from: cachedData as Data) {
            return amount * exchangeRate.rate
        }
        
        let url = URL(string: "\(baseURL)/latest?access_key=\(apiKey)&base=\(sourceCurrency)&symbols=\(targetCurrency)")!
        
        let (data, response) = try await URLSession.shared.data(from: url)
        
        guard let httpResponse = response as? HTTPURLResponse,
              httpResponse.statusCode == 200 else {
            throw CurrencyError.invalidResponse
        }
        
        let exchangeRate = try JSONDecoder().decode(ExchangeRate.self, from: data)
        cache.setObject(data as NSData, forKey: cacheKey)
        
        return amount * exchangeRate.rate
    }
    
    func getSupportedCurrencies() async throws -> [Currency] {
        let url = URL(string: "\(baseURL)/symbols?access_key=\(apiKey)")!
        
        let (data, response) = try await URLSession.shared.data(from: url)
        
        guard let httpResponse = response as? HTTPURLResponse,
              httpResponse.statusCode == 200 else {
            throw CurrencyError.invalidResponse
        }
        
        let symbols = try JSONDecoder().decode(CurrencySymbols.self, from: data)
        return symbols.symbols.map { Currency(code: $0.key, name: $0.value) }
    }
    
    func getHistoricalRates(for currency: String, date: Date) async throws -> [String: Double] {
        let dateFormatter = DateFormatter()
        dateFormatter.dateFormat = "yyyy-MM-dd"
        let dateString = dateFormatter.string(from: date)
        
        let url = URL(string: "\(baseURL)/\(dateString)?access_key=\(apiKey)&base=\(currency)")!
        
        let (data, response) = try await URLSession.shared.data(from: url)
        
        guard let httpResponse = response as? HTTPURLResponse,
              httpResponse.statusCode == 200 else {
            throw CurrencyError.invalidResponse
        }
        
        let historicalRates = try JSONDecoder().decode(HistoricalRates.self, from: data)
        return historicalRates.rates
    }
}

// MARK: - Modelle

struct ExchangeRate: Codable {
    let success: Bool
    let timestamp: Int
    let base: String
    let date: String
    let rates: [String: Double]
    
    var rate: Double {
        rates.values.first ?? 1.0
    }
}

struct CurrencySymbols: Codable {
    let success: Bool
    let symbols: [String: String]
}

struct HistoricalRates: Codable {
    let success: Bool
    let timestamp: Int
    let base: String
    let date: String
    let rates: [String: Double]
}

struct Currency: Codable {
    let code: String
    let name: String
}

// MARK: - Fehler

enum CurrencyError: Error {
    case invalidResponse
    case invalidData
    case networkError
    case unsupportedCurrency
} 