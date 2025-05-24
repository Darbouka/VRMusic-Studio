package com.stompcoin.app.security

import android.content.Context
import com.google.firebase.auth.FirebaseAuth
import com.google.firebase.auth.PhoneAuthCredential
import com.google.firebase.auth.PhoneAuthOptions
import com.google.firebase.auth.PhoneAuthProvider
import dagger.hilt.android.qualifiers.ApplicationContext
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import java.util.concurrent.TimeUnit
import javax.inject.Inject
import javax.inject.Singleton

@Singleton
class TwoFactorAuth @Inject constructor(
    @ApplicationContext private val context: Context,
    private val auth: FirebaseAuth
) {
    private val _verificationState = MutableStateFlow<VerificationState>(VerificationState.Idle)
    val verificationState: StateFlow<VerificationState> = _verificationState

    private var verificationId: String? = null
    private var resendToken: PhoneAuthProvider.ForceResendingToken? = null

    fun startPhoneVerification(phoneNumber: String) {
        _verificationState.value = VerificationState.Loading

        val options = PhoneAuthOptions.newBuilder(auth)
            .setPhoneNumber(phoneNumber)
            .setTimeout(60L, TimeUnit.SECONDS)
            .setActivity(context as android.app.Activity)
            .setCallbacks(callbacks)
            .build()

        PhoneAuthProvider.verifyPhoneNumber(options)
    }

    fun verifyCode(code: String) {
        val credential = verificationId?.let {
            PhoneAuthProvider.getCredential(it, code)
        } ?: run {
            _verificationState.value = VerificationState.Error("Ungültiger Verifizierungscode")
            return
        }

        verifyPhoneAuthCredential(credential)
    }

    private fun verifyPhoneAuthCredential(credential: PhoneAuthCredential) {
        auth.signInWithCredential(credential)
            .addOnCompleteListener { task ->
                if (task.isSuccessful) {
                    _verificationState.value = VerificationState.Success
                } else {
                    _verificationState.value = VerificationState.Error(
                        task.exception?.message ?: "Verifizierung fehlgeschlagen"
                    )
                }
            }
    }

    private val callbacks = object : PhoneAuthProvider.OnVerificationStateChangedCallbacks() {
        override fun onVerificationCompleted(credential: PhoneAuthCredential) {
            verifyPhoneAuthCredential(credential)
        }

        override fun onVerificationFailed(e: Exception) {
            _verificationState.value = VerificationState.Error(
                e.message ?: "Verifizierung fehlgeschlagen"
            )
        }

        override fun onCodeSent(
            verificationId: String,
            token: PhoneAuthProvider.ForceResendingToken
        ) {
            this@TwoFactorAuth.verificationId = verificationId
            this@TwoFactorAuth.resendToken = token
            _verificationState.value = VerificationState.CodeSent
        }
    }

    fun resetState() {
        _verificationState.value = VerificationState.Idle
        verificationId = null
        resendToken = null
    }
}

sealed class VerificationState {
    object Idle : VerificationState()
    object Loading : VerificationState()
    object CodeSent : VerificationState()
    object Success : VerificationState()
    data class Error(val message: String) : VerificationState()
} 