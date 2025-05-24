package com.stompcoin.app.security

import android.content.Context
import androidx.test.core.app.ApplicationProvider
import com.stompcoin.app.network.SecureApiClient
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
class SecurityTest {
    @get:Rule
    var hiltRule = HiltAndroidRule(this)

    @Inject
    lateinit var encryptionManager: EncryptionManager

    @Inject
    lateinit var twoFactorAuth: TwoFactorAuth

    @Inject
    lateinit var secureApiClient: SecureApiClient

    private lateinit var context: Context

    @Before
    fun setup() {
        hiltRule.inject()
        context = ApplicationProvider.getApplicationContext()
    }

    @Test
    fun `should encrypt and decrypt sensitive data`() {
        // Arrange
        val sensitiveData = mapOf(
            "password" to "test123",
            "token" to "secret-token"
        )

        // Act
        val encryptedData = encryptionManager.encryptSensitiveData(sensitiveData)
        val decryptedData = encryptionManager.decryptSensitiveData(encryptedData)

        // Assert
        assert(decryptedData == sensitiveData)
    }

    @Test
    fun `should handle encryption key rotation`() {
        // Arrange
        val data = "sensitive data"
        val (encrypted1, iv1) = encryptionManager.encrypt(data)

        // Act
        encryptionManager.clearKey()
        encryptionManager.createKey()
        val (encrypted2, iv2) = encryptionManager.encrypt(data)

        // Assert
        assert(encrypted1 != encrypted2)
        assert(iv1 != iv2)
    }

    @Test
    fun `should verify two factor authentication flow`() = runBlocking {
        // Arrange
        val phoneNumber = "+49123456789"

        // Act
        twoFactorAuth.startPhoneVerification(phoneNumber)

        // Assert
        assert(twoFactorAuth.verificationState.value is VerificationState.CodeSent)
    }

    @Test
    fun `should handle invalid verification code`() = runBlocking {
        // Arrange
        val invalidCode = "000000"

        // Act
        twoFactorAuth.verifyCode(invalidCode)

        // Assert
        assert(twoFactorAuth.verificationState.value is VerificationState.Error)
    }

    @Test
    fun `should secure API communication`() {
        // Arrange
        val request = okhttp3.Request.Builder()
            .url("https://api.stompcoin.com/test")
            .post(okhttp3.RequestBody.create(
                okhttp3.MediaType.parse("application/json"),
                "{\"sensitive\":\"data\"}"
            ))
            .build()

        // Act
        val encryptedRequest = secureApiClient.encryptRequest(request)

        // Assert
        assert(encryptedRequest.header("X-Encrypted") == "true")
        assert(encryptedRequest.header("X-IV") != null)
    }

    @Test
    fun `should handle secure API response`() {
        // Arrange
        val encryptedData = "encrypted-data"
        val iv = "initialization-vector"
        val response = okhttp3.Response.Builder()
            .request(okhttp3.Request.Builder().url("https://api.stompcoin.com/test").build())
            .protocol(okhttp3.Protocol.HTTP_1_1)
            .code(200)
            .message("OK")
            .header("X-Encrypted", "true")
            .header("X-IV", iv)
            .body(okhttp3.ResponseBody.create(
                okhttp3.MediaType.parse("application/json"),
                encryptedData
            ))
            .build()

        // Act
        val decryptedResponse = secureApiClient.decryptResponse(response)

        // Assert
        assert(decryptedResponse.body()?.string()?.isNotEmpty() == true)
    }

    @Test
    fun `should handle API authentication`() {
        // Arrange
        val request = okhttp3.Request.Builder()
            .url("https://api.stompcoin.com/test")
            .build()

        // Act
        val authenticatedRequest = secureApiClient.authenticateRequest(request)

        // Assert
        assert(authenticatedRequest.header("Authorization")?.startsWith("Bearer ") == true)
        assert(authenticatedRequest.header("X-API-Key") != null)
    }

    @Test
    fun `should handle security exceptions`() {
        // Arrange
        val invalidData = ByteArray(0)
        val invalidIv = ByteArray(0)

        // Act & Assert
        try {
            encryptionManager.decrypt(invalidData, invalidIv)
            assert(false) // Sollte eine Exception werfen
        } catch (e: Exception) {
            assert(e is SecurityException)
        }
    }

    @Test
    fun `should handle concurrent security operations`() = runBlocking {
        // Arrange
        val data = "sensitive data"
        val operations = List(100) {
            kotlinx.coroutines.async {
                encryptionManager.encrypt(data)
            }
        }

        // Act
        val results = operations.map { it.await() }

        // Assert
        assert(results.all { it.first.isNotEmpty() })
        assert(results.all { it.second.isNotEmpty() })
    }
} 