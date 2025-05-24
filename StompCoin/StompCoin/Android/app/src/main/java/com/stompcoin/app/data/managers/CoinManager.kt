package com.stompcoin.app.data.managers

import com.stompcoin.app.data.model.Currency
import com.stompcoin.app.data.model.PaymentMethod
import com.stompcoin.app.data.model.Transaction
import com.stompcoin.app.data.model.User
import com.stompcoin.app.data.repository.TransactionRepository
import com.stompcoin.app.data.repository.UserRepository
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import javax.inject.Inject
import javax.inject.Singleton

@Singleton
class CoinManager @Inject constructor(
    private val userRepository: UserRepository,
    private val transactionRepository: TransactionRepository
) {
    private val _error = MutableStateFlow<String?>(null)
    val error: StateFlow<String?> = _error

    suspend fun convertCoins(
        amount: Double,
        from: Currency,
        to: Currency,
        user: User
    ): Boolean {
        if (amount <= 0) return false

        val conversionRate = when (from to to) {
            Currency.STOMP_COIN to Currency.OM_COIN -> 0.1 // 10 StompCoins = 1 OM Coin
            Currency.OM_COIN to Currency.STOMP_COIN -> 10.0 // 1 OM Coin = 10 StompCoins
            else -> return false
        }

        val hasEnoughCoins = when (from) {
            Currency.STOMP_COIN -> user.stompCoins >= amount
            Currency.OM_COIN -> user.omCoins >= amount
        }

        if (!hasEnoughCoins) return false

        // Coins umrechnen
        when (from) {
            Currency.STOMP_COIN -> {
                user.stompCoins -= amount
                user.omCoins += amount * conversionRate
            }
            Currency.OM_COIN -> {
                user.omCoins -= amount
                user.stompCoins += amount * conversionRate
            }
        }

        // Transaktion erstellen
        val transaction = Transaction(
            userId = user.id,
            type = Transaction.Type.CONVERSION,
            amount = amount,
            currency = from,
            description = "Konvertierung von ${from.name} zu ${to.name}"
        )

        try {
            userRepository.updateUser(user)
            transactionRepository.createTransaction(transaction)
            return true
        } catch (e: Exception) {
            _error.value = e.message
            return false
        }
    }

    suspend fun purchaseCoins(
        amount: Double,
        paymentMethod: PaymentMethod,
        user: User
    ): Boolean {
        if (amount <= 0) return false

        try {
            // Hier würde die Zahlungsabwicklung implementiert werden
            // Für jetzt simulieren wir einen erfolgreichen Kauf
            user.stompCoins += amount

            val transaction = Transaction(
                userId = user.id,
                type = Transaction.Type.PURCHASE,
                amount = amount,
                currency = Currency.STOMP_COIN,
                description = "Kauf von ${amount.toInt()} StompCoins"
            )

            userRepository.updateUser(user)
            transactionRepository.createTransaction(transaction)
            return true
        } catch (e: Exception) {
            _error.value = e.message
            return false
        }
    }
} 