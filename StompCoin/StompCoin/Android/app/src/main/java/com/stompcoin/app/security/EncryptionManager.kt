package com.stompcoin.app.security

import android.security.keystore.KeyGenParameterSpec
import android.security.keystore.KeyProperties
import java.security.KeyStore
import javax.crypto.Cipher
import javax.crypto.KeyGenerator
import javax.crypto.SecretKey
import javax.crypto.spec.GCMParameterSpec
import javax.inject.Inject
import javax.inject.Singleton

@Singleton
class EncryptionManager @Inject constructor() {
    private val keyStore = KeyStore.getInstance("AndroidKeyStore").apply { load(null) }
    private val keyAlias = "StompCoinKey"

    init {
        if (!keyStore.containsAlias(keyAlias)) {
            createKey()
        }
    }

    private fun createKey() {
        val keyGenerator = KeyGenerator.getInstance(
            KeyProperties.KEY_ALGORITHM_AES,
            "AndroidKeyStore"
        )

        val keyGenParameterSpec = KeyGenParameterSpec.Builder(
            keyAlias,
            KeyProperties.PURPOSE_ENCRYPT or KeyProperties.PURPOSE_DECRYPT
        )
            .setBlockModes(KeyProperties.BLOCK_MODE_GCM)
            .setEncryptionPaddings(KeyProperties.ENCRYPTION_PADDING_NONE)
            .setKeySize(256)
            .build()

        keyGenerator.init(keyGenParameterSpec)
        keyGenerator.generateKey()
    }

    fun encrypt(data: String): Pair<ByteArray, ByteArray> {
        val cipher = Cipher.getInstance("AES/GCM/NoPadding")
        val secretKey = keyStore.getKey(keyAlias, null) as SecretKey
        cipher.init(Cipher.ENCRYPT_MODE, secretKey)
        
        val encryptedBytes = cipher.doFinal(data.toByteArray(Charsets.UTF_8))
        val iv = cipher.iv

        return Pair(encryptedBytes, iv)
    }

    fun decrypt(encryptedData: ByteArray, iv: ByteArray): String {
        val cipher = Cipher.getInstance("AES/GCM/NoPadding")
        val secretKey = keyStore.getKey(keyAlias, null) as SecretKey
        val spec = GCMParameterSpec(128, iv)
        cipher.init(Cipher.DECRYPT_MODE, secretKey, spec)
        
        val decryptedBytes = cipher.doFinal(encryptedData)
        return String(decryptedBytes, Charsets.UTF_8)
    }

    fun encryptSensitiveData(data: Map<String, String>): Map<String, Pair<ByteArray, ByteArray>> {
        return data.mapValues { (_, value) -> encrypt(value) }
    }

    fun decryptSensitiveData(encryptedData: Map<String, Pair<ByteArray, ByteArray>>): Map<String, String> {
        return encryptedData.mapValues { (_, value) ->
            decrypt(value.first, value.second)
        }
    }

    fun clearKey() {
        if (keyStore.containsAlias(keyAlias)) {
            keyStore.deleteEntry(keyAlias)
        }
    }
} 