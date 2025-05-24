package com.stompcoin.app.security

import android.content.Context
import android.security.keystore.KeyGenParameterSpec
import android.security.keystore.KeyProperties
import androidx.security.crypto.EncryptedSharedPreferences
import androidx.security.crypto.MasterKey
import dagger.hilt.android.qualifiers.ApplicationContext
import javax.inject.Inject
import javax.inject.Singleton

@Singleton
class SecureKeyStore @Inject constructor(
    @ApplicationContext private val context: Context
) {
    private val masterKey = MasterKey.Builder(context)
        .setKeyScheme(MasterKey.KeyScheme.AES256_GCM)
        .setKeyGenParameterSpec(
            KeyGenParameterSpec.Builder(
                "_stompcoin_master_key_",
                KeyProperties.PURPOSE_ENCRYPT or KeyProperties.PURPOSE_DECRYPT
            )
            .setBlockModes(KeyProperties.BLOCK_MODE_GCM)
            .setEncryptionPaddings(KeyProperties.ENCRYPTION_PADDING_NONE)
            .setKeySize(256)
            .build()
        )
        .build()

    private val securePrefs = EncryptedSharedPreferences.create(
        context,
        "secure_prefs",
        masterKey,
        EncryptedSharedPreferences.PrefKeyEncryptionScheme.AES256_SIV,
        EncryptedSharedPreferences.PrefValueEncryptionScheme.AES256_GCM
    )

    fun saveApiKey(key: String, value: String) {
        securePrefs.edit().putString(key, value).apply()
    }

    fun getApiKey(key: String): String? {
        return securePrefs.getString(key, null)
    }

    fun removeApiKey(key: String) {
        securePrefs.edit().remove(key).apply()
    }

    fun clearAllKeys() {
        securePrefs.edit().clear().apply()
    }

    fun hasApiKey(key: String): Boolean {
        return securePrefs.contains(key)
    }

    fun getAllApiKeys(): Map<String, String> {
        return securePrefs.all.mapValues { it.value.toString() }
    }

    companion object {
        const val KEY_API_BASE_URL = "api_base_url"
        const val KEY_API_KEY = "api_key"
        const val KEY_API_SECRET = "api_secret"
        const val KEY_FIREBASE_CONFIG = "firebase_config"
        const val KEY_ANALYTICS_KEY = "analytics_key"
    }
} 