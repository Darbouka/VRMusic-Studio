import Foundation
import SwiftData

@Model
final class Transaction {
    var id: UUID
    var user: User
    var type: TransactionType
    var amount: Double
    var currency: Currency
    var status: TransactionStatus
    var date: Date
    var description: String
    
    init(user: User, type: TransactionType, amount: Double, currency: Currency, description: String) {
        self.id = UUID()
        self.user = user
        self.type = type
        self.amount = amount
        self.currency = currency
        self.status = .pending
        self.date = Date()
        self.description = description
    }
}

enum TransactionType: String, Codable {
    case purchase
    case conversion
    case reward
    case subscription
    case refund
}

enum Currency: String, Codable {
    case stompCoin
    case omCoin
    case eur
    case usd
}

enum TransactionStatus: String, Codable {
    case pending
    case completed
    case failed
    case refunded
}

@Model
final class Subscription {
    var id: UUID
    var user: User
    var type: SubscriptionType
    var startDate: Date
    var endDate: Date
    var paymentMethod: PaymentMethod
    var status: SubscriptionStatus
    var price: Double
    var currency: Currency
    
    init(user: User, type: SubscriptionType, startDate: Date, endDate: Date, paymentMethod: PaymentMethod, price: Double, currency: Currency) {
        self.id = UUID()
        self.user = user
        self.type = type
        self.startDate = startDate
        self.endDate = endDate
        self.paymentMethod = paymentMethod
        self.status = .active
        self.price = price
        self.currency = currency
    }
}

enum PaymentMethod: String, Codable {
    case creditCard
    case visa
    case mastercard
    case giro
    case paypal
    case klarna
    case bankTransfer
}

enum SubscriptionStatus: String, Codable {
    case active
    case expired
    case cancelled
    case pending
} 