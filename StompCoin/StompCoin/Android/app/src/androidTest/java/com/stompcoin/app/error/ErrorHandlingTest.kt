package com.stompcoin.app.error

import android.content.Context
import androidx.test.core.app.ApplicationProvider
import com.stompcoin.app.network.NetworkError
import com.stompcoin.app.network.SecureApiClient
import com.stompcoin.app.security.SecurityException
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
class ErrorHandlingTest {
    @get:Rule
    var hiltRule = HiltAndroidRule(this)

    @Inject
    lateinit var errorHandler: ErrorHandler

    @Inject
    lateinit var secureApiClient: SecureApiClient

    private lateinit var context: Context

    @Before
    fun setup() {
        hiltRule.inject()
        context = ApplicationProvider.getApplicationContext()
    }

    @Test
    fun `should handle network errors`() = runBlocking {
        // Arrange
        val networkError = NetworkError("Keine Internetverbindung")

        // Act
        val handledError = errorHandler.handleError(networkError)

        // Assert
        assert(handledError.userMessage == "Bitte überprüfen Sie Ihre Internetverbindung")
        assert(handledError.shouldRetry)
    }

    @Test
    fun `should handle security errors`() = runBlocking {
        // Arrange
        val securityError = SecurityException("Ungültiger API-Schlüssel")

        // Act
        val handledError = errorHandler.handleError(securityError)

        // Assert
        assert(handledError.userMessage == "Sicherheitsfehler: Bitte melden Sie sich erneut an")
        assert(!handledError.shouldRetry)
    }

    @Test
    fun `should handle API errors`() = runBlocking {
        // Arrange
        val apiError = ApiError(404, "Ressource nicht gefunden")

        // Act
        val handledError = errorHandler.handleError(apiError)

        // Assert
        assert(handledError.userMessage == "Die angeforderte Ressource wurde nicht gefunden")
        assert(!handledError.shouldRetry)
    }

    @Test
    fun `should handle validation errors`() = runBlocking {
        // Arrange
        val validationError = ValidationError("Ungültige Eingabe")

        // Act
        val handledError = errorHandler.handleError(validationError)

        // Assert
        assert(handledError.userMessage == "Bitte überprüfen Sie Ihre Eingabe")
        assert(!handledError.shouldRetry)
    }

    @Test
    fun `should handle unknown errors`() = runBlocking {
        // Arrange
        val unknownError = Exception("Unbekannter Fehler")

        // Act
        val handledError = errorHandler.handleError(unknownError)

        // Assert
        assert(handledError.userMessage == "Ein unerwarteter Fehler ist aufgetreten")
        assert(!handledError.shouldRetry)
    }

    @Test
    fun `should handle concurrent errors`() = runBlocking {
        // Arrange
        val errors = listOf(
            NetworkError("Keine Internetverbindung"),
            SecurityException("Ungültiger API-Schlüssel"),
            ApiError(404, "Ressource nicht gefunden")
        )

        // Act
        val handledErrors = errors.map { error ->
            errorHandler.handleError(error)
        }

        // Assert
        assert(handledErrors.all { it.userMessage.isNotEmpty() })
    }

    @Test
    fun `should handle error recovery`() = runBlocking {
        // Arrange
        val networkError = NetworkError("Keine Internetverbindung")
        var retryCount = 0

        // Act
        val handledError = errorHandler.handleError(networkError) {
            retryCount++
            if (retryCount < 3) {
                throw networkError
            }
        }

        // Assert
        assert(retryCount == 3)
        assert(handledError.userMessage == "Bitte überprüfen Sie Ihre Internetverbindung")
    }

    @Test
    fun `should handle error logging`() = runBlocking {
        // Arrange
        val error = Exception("Testfehler")

        // Act
        errorHandler.handleError(error)

        // Assert
        val logs = errorHandler.getErrorLogs()
        assert(logs.any { it.message == "Testfehler" })
    }

    @Test
    fun `should handle error analytics`() = runBlocking {
        // Arrange
        val error = ApiError(500, "Interner Serverfehler")

        // Act
        errorHandler.handleError(error)

        // Assert
        val analytics = errorHandler.getErrorAnalytics()
        assert(analytics.any { it.type == "API_ERROR" && it.code == 500 })
    }

    @Test
    fun `should handle error notifications`() = runBlocking {
        // Arrange
        val criticalError = SecurityException("Kritischer Sicherheitsfehler")

        // Act
        errorHandler.handleError(criticalError)

        // Assert
        val notifications = errorHandler.getErrorNotifications()
        assert(notifications.any { it.isCritical })
    }
} 