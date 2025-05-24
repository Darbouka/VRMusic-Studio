package com.stompcoin.app.currency

import android.content.Context
import androidx.test.core.app.ApplicationProvider
import dagger.hilt.android.testing.HiltAndroidRule
import dagger.hilt.android.testing.HiltAndroidTest
import kotlinx.coroutines.runBlocking
import org.junit.Before
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith
import org.robolectric.RobolectricTestRunner
import org.robolectric.annotation.Config
import javax.inject.Inject

@HiltAndroidTest
@RunWith(RobolectricTestRunner::class)
@Config(application = HiltTestApplication::class)
class DigitalCurrencyTest {
    @get:Rule
    var hiltRule = HiltAndroidRule(this)

    @Inject
    lateinit var currencyManager: DigitalCurrencyManager

    @Inject
    lateinit var walletManager: DigitalWalletManager

    private lateinit var context: Context

    @Before
    fun setup() {
        hiltRule.inject()
        context = ApplicationProvider.getApplicationContext()
    }

    @Test
    fun `should only allow digital currency transactions`() = runBlocking {
        // Arrange
        val userId = "testuser"
        val transaction = Transaction(
            type = "digital",
            amount = 100,
            currency = "STOMP"
        )

        // Act
        val result = currencyManager.processTransaction(userId, transaction)

        // Assert
        assert(result.isSuccess)
        assert(result.currency == "STOMP")
    }

    @Test
    fun `should reject physical currency transactions`() = runBlocking {
        // Arrange
        val userId = "testuser"
        val transaction = Transaction(
            type = "physical",
            amount = 100,
            currency = "EUR"
        )

        // Act
        val result = currencyManager.processTransaction(userId, transaction)

        // Assert
        assert(!result.isSuccess)
        assert(result.error == "Nur digitale Währungen sind erlaubt")
    }

    @Test
    fun `should handle digital wallet creation`() = runBlocking {
        // Arrange
        val userId = "testuser"

        // Act
        val wallet = walletManager.createWallet(userId)

        // Assert
        assert(wallet.isSuccess)
        assert(wallet.currency == "STOMP")
        assert(wallet.balance == 0)
    }

    @Test
    fun `should handle digital currency conversion`() = runBlocking {
        // Arrange
        val userId = "testuser"
        val amount = 100
        val fromCurrency = "STOMP"
        val toCurrency = "STOMP" // Nur STOMP zu STOMP Konvertierung erlaubt

        // Act
        val result = currencyManager.convertCurrency(userId, amount, fromCurrency, toCurrency)

        // Assert
        assert(result.isSuccess)
        assert(result.amount == amount)
        assert(result.currency == "STOMP")
    }

    @Test
    fun `should reject conversion to physical currency`() = runBlocking {
        // Arrange
        val userId = "testuser"
        val amount = 100
        val fromCurrency = "STOMP"
        val toCurrency = "EUR"

        // Act
        val result = currencyManager.convertCurrency(userId, amount, fromCurrency, toCurrency)

        // Assert
        assert(!result.isSuccess)
        assert(result.error == "Konvertierung zu physischer Währung nicht erlaubt")
    }

    @Test
    fun `should handle digital currency transfer`() = runBlocking {
        // Arrange
        val senderId = "user1"
        val receiverId = "user2"
        val amount = 100

        // Act
        val result = currencyManager.transferCurrency(senderId, receiverId, amount)

        // Assert
        assert(result.isSuccess)
        assert(result.currency == "STOMP")
    }

    @Test
    fun `should handle digital currency balance`() = runBlocking {
        // Arrange
        val userId = "testuser"
        val initialBalance = 1000

        // Act
        walletManager.setBalance(userId, initialBalance)
        val balance = walletManager.getBalance(userId)

        // Assert
        assert(balance.amount == initialBalance)
        assert(balance.currency == "STOMP")
    }

    @Test
    fun `should handle digital currency transaction history`() = runBlocking {
        // Arrange
        val userId = "testuser"
        val transaction = Transaction(
            type = "digital",
            amount = 100,
            currency = "STOMP"
        )

        // Act
        currencyManager.processTransaction(userId, transaction)
        val history = walletManager.getTransactionHistory(userId)

        // Assert
        assert(history.isNotEmpty())
        assert(history.all { it.currency == "STOMP" })
    }

    @Test
    fun `should handle digital currency limits`() = runBlocking {
        // Arrange
        val userId = "testuser"
        val transaction = Transaction(
            type = "digital",
            amount = 1000000, // Sehr hoher Betrag
            currency = "STOMP"
        )

        // Act
        val result = currencyManager.processTransaction(userId, transaction)

        // Assert
        assert(!result.isSuccess)
        assert(result.error == "Transaktionslimit überschritten")
    }

    @Test
    fun `should handle digital currency security`() = runBlocking {
        // Arrange
        val userId = "testuser"
        val transaction = Transaction(
            type = "digital",
            amount = 100,
            currency = "STOMP"
        )

        // Act
        val result = currencyManager.processTransaction(userId, transaction)

        // Assert
        assert(result.isSuccess)
        assert(result.isSecure)
        assert(result.transactionId != null)
    }
} 