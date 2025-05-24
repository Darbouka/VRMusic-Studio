package com.stompcoin.app.registration

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
class RegistrationBonusTest {
    @get:Rule
    var hiltRule = HiltAndroidRule(this)

    @Inject
    lateinit var registrationManager: RegistrationManager

    @Inject
    lateinit var gamificationManager: GamificationManager

    private lateinit var context: Context

    @Before
    fun setup() {
        hiltRule.inject()
        context = ApplicationProvider.getApplicationContext()
    }

    @Test
    fun `should award initial bonus coins on registration`() = runBlocking {
        // Arrange
        val newUser = UserRegistration(
            username = "testuser",
            email = "test@example.com",
            password = "securePassword123"
        )

        // Act
        val registrationResult = registrationManager.registerUser(newUser)

        // Assert
        assert(registrationResult.isSuccess)
        assert(registrationResult.userId != null)

        // Überprüfe das initiale Spielguthaben
        val gameBalance = gamificationManager.getGameBalance(registrationResult.userId!!)
        assert(gameBalance.coins == 1000)
    }

    @Test
    fun `should not award bonus coins on subsequent registrations`() = runBlocking {
        // Arrange
        val existingUser = UserRegistration(
            username = "existinguser",
            email = "existing@example.com",
            password = "securePassword123"
        )

        // Act
        val firstRegistration = registrationManager.registerUser(existingUser)
        val secondRegistration = registrationManager.registerUser(existingUser)

        // Assert
        assert(firstRegistration.isSuccess)
        assert(!secondRegistration.isSuccess) // Zweite Registrierung sollte fehlschlagen
    }

    @Test
    fun `should verify bonus coins transaction`() = runBlocking {
        // Arrange
        val newUser = UserRegistration(
            username = "testuser2",
            email = "test2@example.com",
            password = "securePassword123"
        )

        // Act
        val registrationResult = registrationManager.registerUser(newUser)

        // Assert
        assert(registrationResult.isSuccess)
        
        // Überprüfe die Transaktionshistorie
        val transactions = gamificationManager.getTransactionHistory(registrationResult.userId!!)
        val bonusTransaction = transactions.find { it.type == "registration_bonus" }
        
        assert(bonusTransaction != null)
        assert(bonusTransaction?.amount == 1000)
        assert(bonusTransaction?.description == "Willkommensbonus bei Registrierung")
    }

    @Test
    fun `should handle bonus coins with different currencies`() = runBlocking {
        // Arrange
        val newUser = UserRegistration(
            username = "testuser3",
            email = "test3@example.com",
            password = "securePassword123",
            preferredCurrency = "EUR"
        )

        // Act
        val registrationResult = registrationManager.registerUser(newUser)

        // Assert
        assert(registrationResult.isSuccess)
        
        // Überprüfe das Spielguthaben in der bevorzugten Währung
        val gameBalance = gamificationManager.getGameBalance(registrationResult.userId!!)
        assert(gameBalance.coins == 1000)
        assert(gameBalance.currency == "EUR")
    }

    @Test
    fun `should handle bonus coins with special characters in username`() = runBlocking {
        // Arrange
        val newUser = UserRegistration(
            username = "test.user-123",
            email = "special@example.com",
            password = "securePassword123"
        )

        // Act
        val registrationResult = registrationManager.registerUser(newUser)

        // Assert
        assert(registrationResult.isSuccess)
        val gameBalance = gamificationManager.getGameBalance(registrationResult.userId!!)
        assert(gameBalance.coins == 1000)
    }

    @Test
    fun `should handle bonus coins with minimum password length`() = runBlocking {
        // Arrange
        val newUser = UserRegistration(
            username = "testuser4",
            email = "test4@example.com",
            password = "12345678" // Minimale Passwortlänge
        )

        // Act
        val registrationResult = registrationManager.registerUser(newUser)

        // Assert
        assert(registrationResult.isSuccess)
        val gameBalance = gamificationManager.getGameBalance(registrationResult.userId!!)
        assert(gameBalance.coins == 1000)
    }

    @Test
    fun `should handle bonus coins with maximum username length`() = runBlocking {
        // Arrange
        val newUser = UserRegistration(
            username = "a".repeat(50), // Maximale Benutzernamenlänge
            email = "long@example.com",
            password = "securePassword123"
        )

        // Act
        val registrationResult = registrationManager.registerUser(newUser)

        // Assert
        assert(registrationResult.isSuccess)
        val gameBalance = gamificationManager.getGameBalance(registrationResult.userId!!)
        assert(gameBalance.coins == 1000)
    }

    @Test
    fun `should handle bonus coins with different email domains`() = runBlocking {
        // Arrange
        val newUser = UserRegistration(
            username = "testuser5",
            email = "test5@custom-domain.de",
            password = "securePassword123"
        )

        // Act
        val registrationResult = registrationManager.registerUser(newUser)

        // Assert
        assert(registrationResult.isSuccess)
        val gameBalance = gamificationManager.getGameBalance(registrationResult.userId!!)
        assert(gameBalance.coins == 1000)
    }

    @Test
    fun `should handle bonus coins with concurrent registrations`() = runBlocking {
        // Arrange
        val users = List(10) { index ->
            UserRegistration(
                username = "concurrent_user_$index",
                email = "concurrent_$index@example.com",
                password = "securePassword123"
            )
        }

        // Act
        val results = users.map { user ->
            kotlinx.coroutines.async {
                registrationManager.registerUser(user)
            }
        }.map { it.await() }

        // Assert
        assert(results.all { it.isSuccess })
        results.forEach { result ->
            val gameBalance = gamificationManager.getGameBalance(result.userId!!)
            assert(gameBalance.coins == 1000)
        }
    }

    @Test
    fun `should handle bonus coins with registration retry`() = runBlocking {
        // Arrange
        val newUser = UserRegistration(
            username = "retryuser",
            email = "retry@example.com",
            password = "securePassword123"
        )

        // Act
        var registrationResult = registrationManager.registerUser(newUser)
        if (!registrationResult.isSuccess) {
            // Warte kurz und versuche es erneut
            kotlinx.coroutines.delay(1000)
            registrationResult = registrationManager.registerUser(newUser)
        }

        // Assert
        assert(registrationResult.isSuccess)
        val gameBalance = gamificationManager.getGameBalance(registrationResult.userId!!)
        assert(gameBalance.coins == 1000)
    }

    @Test
    fun `should handle bonus coins with registration cancellation`() = runBlocking {
        // Arrange
        val newUser = UserRegistration(
            username = "canceluser",
            email = "cancel@example.com",
            password = "securePassword123"
        )

        // Act
        val registrationResult = registrationManager.registerUser(newUser)
        registrationManager.cancelRegistration(registrationResult.userId!!)

        // Assert
        val gameBalance = gamificationManager.getGameBalance(registrationResult.userId!!)
        assert(gameBalance.coins == 0) // Bonus sollte zurückgezogen werden
    }
} 