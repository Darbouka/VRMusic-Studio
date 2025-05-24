package com.stompcoin.app.security

import android.content.Context
import androidx.biometric.BiometricManager
import androidx.biometric.BiometricPrompt
import androidx.core.content.ContextCompat
import androidx.fragment.app.FragmentActivity
import dagger.hilt.android.qualifiers.ApplicationContext
import javax.inject.Inject
import javax.inject.Singleton

@Singleton
class BiometricManager @Inject constructor(
    @ApplicationContext private val context: Context
) {
    private val biometricManager = BiometricManager.from(context)

    fun canAuthenticate(): Boolean {
        return biometricManager.canAuthenticate(BiometricManager.Authenticators.BIOMETRIC_STRONG) ==
                BiometricManager.BIOMETRIC_SUCCESS
    }

    fun showBiometricPrompt(
        activity: FragmentActivity,
        onSuccess: () -> Unit,
        onError: (String) -> Unit
    ) {
        val executor = ContextCompat.getMainExecutor(context)
        val callback = object : BiometricPrompt.AuthenticationCallback() {
            override fun onAuthenticationSucceeded(result: BiometricPrompt.AuthenticationResult) {
                onSuccess()
            }

            override fun onAuthenticationError(errorCode: Int, errString: CharSequence) {
                onError(errString.toString())
            }

            override fun onAuthenticationFailed() {
                onError("Biometrische Authentifizierung fehlgeschlagen")
            }
        }

        val promptInfo = BiometricPrompt.PromptInfo.Builder()
            .setTitle("Biometrische Authentifizierung")
            .setSubtitle("Bitte authentifizieren Sie sich mit Ihrem biometrischen Merkmal")
            .setNegativeButtonText("Abbrechen")
            .build()

        BiometricPrompt(activity, executor, callback).authenticate(promptInfo)
    }

    fun isBiometricAvailable(): Boolean {
        return biometricManager.canAuthenticate(BiometricManager.Authenticators.BIOMETRIC_STRONG) ==
                BiometricManager.BIOMETRIC_SUCCESS
    }

    fun getBiometricStrength(): Int {
        return when (biometricManager.canAuthenticate(BiometricManager.Authenticators.BIOMETRIC_STRONG)) {
            BiometricManager.BIOMETRIC_SUCCESS -> BiometricManager.Authenticators.BIOMETRIC_STRONG
            BiometricManager.BIOMETRIC_ERROR_NO_HARDWARE -> 0
            BiometricManager.BIOMETRIC_ERROR_HW_UNAVAILABLE -> 0
            BiometricManager.BIOMETRIC_ERROR_NONE_ENROLLED -> 0
            else -> 0
        }
    }
} 