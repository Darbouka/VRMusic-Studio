import Foundation

class FiatConversionManager {
    static let shared = FiatConversionManager()
    
    private let conversionFee: Double = 0.02 // 2% Konvertierungsgebühr
    private let minimumWithdrawal: Double = 1000 // 1000 STOMP Minimum
    
    private var exchangeRates: [String: Double] = [
        "EUR": 0.001, // 1 STOMP = 0.001 EUR
        "USD": 0.0011,
        "GBP": 0.00085
    ]
    
    private init() {}
    
    func convertToFiat(stompAmount: Double, targetCurrency: String) -> Double? {
        guard stompAmount >= minimumWithdrawal,
              let rate = exchangeRates[targetCurrency] else {
            return nil
        }
        
        let fee = stompAmount * conversionFee
        let netAmount = stompAmount - fee
        return netAmount * rate
    }
    
    func convertToStomp(fiatAmount: Double, sourceCurrency: String) -> Double? {
        guard let rate = exchangeRates[sourceCurrency] else {
            return nil
        }
        
        return fiatAmount / rate
    }
    
    func updateExchangeRate(for currency: String, newRate: Double) {
        exchangeRates[currency] = newRate
    }
    
    func getAvailableCurrencies() -> [String] {
        return Array(exchangeRates.keys)
    }
    
    func getConversionFee() -> Double {
        return conversionFee
    }
    
    func getMinimumWithdrawal() -> Double {
        return minimumWithdrawal
    }
} 