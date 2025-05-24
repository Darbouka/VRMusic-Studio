package com.stompcoin.app.network

import android.content.Context
import com.stompcoin.app.security.EncryptionManager
import com.stompcoin.app.security.SecureKeyStore
import dagger.hilt.android.qualifiers.ApplicationContext
import okhttp3.Interceptor
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.Response
import okhttp3.logging.HttpLoggingInterceptor
import retrofit2.Retrofit
import retrofit2.converter.gson.GsonConverterFactory
import java.security.cert.X509Certificate
import java.util.concurrent.TimeUnit
import javax.inject.Inject
import javax.inject.Singleton
import javax.net.ssl.SSLContext
import javax.net.ssl.TrustManager
import javax.net.ssl.X509TrustManager

@Singleton
class SecureApiClient @Inject constructor(
    @ApplicationContext private val context: Context,
    private val secureKeyStore: SecureKeyStore,
    private val encryptionManager: EncryptionManager
) {
    private val okHttpClient: OkHttpClient by lazy {
        OkHttpClient.Builder()
            .addInterceptor(AuthInterceptor(secureKeyStore))
            .addInterceptor(EncryptionInterceptor(encryptionManager))
            .addInterceptor(HttpLoggingInterceptor().apply {
                level = HttpLoggingInterceptor.Level.BODY
            })
            .connectTimeout(30, TimeUnit.SECONDS)
            .readTimeout(30, TimeUnit.SECONDS)
            .writeTimeout(30, TimeUnit.SECONDS)
            .build()
    }

    private val retrofit: Retrofit by lazy {
        Retrofit.Builder()
            .baseUrl(secureKeyStore.getApiBaseUrl())
            .client(okHttpClient)
            .addConverterFactory(GsonConverterFactory.create())
            .build()
    }

    fun <T> create(serviceClass: Class<T>): T {
        return retrofit.create(serviceClass)
    }
}

class AuthInterceptor(
    private val secureKeyStore: SecureKeyStore
) : Interceptor {
    override fun intercept(chain: Interceptor.Chain): Response {
        val originalRequest = chain.request()
        val newRequest = originalRequest.newBuilder()
            .header("Authorization", "Bearer ${secureKeyStore.getApiKey()}")
            .header("X-API-Key", secureKeyStore.getApiKey())
            .build()
        return chain.proceed(newRequest)
    }
}

class EncryptionInterceptor(
    private val encryptionManager: EncryptionManager
) : Interceptor {
    override fun intercept(chain: Interceptor.Chain): Response {
        val originalRequest = chain.request()
        val encryptedRequest = encryptRequest(originalRequest)
        val response = chain.proceed(encryptedRequest)
        return decryptResponse(response)
    }

    private fun encryptRequest(request: Request): Request {
        // Verschlüssele sensitive Daten in der Anfrage
        val encryptedBody = request.body?.let { body ->
            val bodyString = body.toString()
            val (encryptedData, iv) = encryptionManager.encrypt(bodyString)
            // Erstelle neuen Request mit verschlüsselten Daten
            request.newBuilder()
                .header("X-Encrypted", "true")
                .header("X-IV", iv.toString(Charsets.UTF_8))
                .build()
        } ?: request

        return encryptedBody
    }

    private fun decryptResponse(response: Response): Response {
        // Entschlüssele sensitive Daten in der Antwort
        if (response.header("X-Encrypted") == "true") {
            val iv = response.header("X-IV")?.toByteArray(Charsets.UTF_8)
            val encryptedBody = response.body?.string()
            
            if (iv != null && encryptedBody != null) {
                val decryptedBody = encryptionManager.decrypt(
                    encryptedBody.toByteArray(Charsets.UTF_8),
                    iv
                )
                // Erstelle neue Response mit entschlüsselten Daten
                return response.newBuilder()
                    .body(decryptedBody.toResponseBody(response.body?.contentType()))
                    .build()
            }
        }
        return response
    }
}

class TrustAllCerts : X509TrustManager {
    override fun checkClientTrusted(chain: Array<X509Certificate>, authType: String) {}
    override fun checkServerTrusted(chain: Array<X509Certificate>, authType: String) {}
    override fun getAcceptedIssuers(): Array<X509Certificate> = arrayOf()
} 