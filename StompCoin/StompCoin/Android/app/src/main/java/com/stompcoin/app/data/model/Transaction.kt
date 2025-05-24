package com.stompcoin.app.data.model

import java.time.LocalDateTime

data class Transaction(
    val id: String = java.util.UUID.randomUUID().toString(),
    val userId: String,
    val type: Type,
    val amount: Double,
    val currency: Currency,
    val description: String,
    val timestamp: Long = System.currentTimeMillis()
) {
    enum class Type {
        PURCHASE,
        CONVERSION,
        REWARD,
        TRANSFER
    }
}

enum class Currency {
    STOMP_COIN,
    OM_COIN
}

enum class PaymentMethod {
    CREDIT_CARD,
    PAYPAL,
    APPLE_PAY,
    GOOGLE_PAY
} 