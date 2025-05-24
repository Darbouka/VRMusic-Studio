package com.stompcoin.app.network

import android.content.Context
import android.net.ConnectivityManager
import android.net.NetworkCapabilities
import com.stompcoin.app.cache.CacheManager
import dagger.hilt.android.qualifiers.ApplicationContext
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.flow
import okhttp3.Cache
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.Response
import java.io.File
import java.util.concurrent.TimeUnit
import javax.inject.Inject
import javax.inject.Singleton

@Singleton
class NetworkOptimizer @Inject constructor(
    @ApplicationContext private val context: Context,
    private val cacheManager: CacheManager
) {
    private val connectivityManager = context.getSystemService(Context.CONNECTIVITY_SERVICE) as ConnectivityManager

    private val cacheSize = 10 * 1024 * 1024L // 10MB
    private val cacheDir = File(context.cacheDir, "http_cache")
    private val cache = Cache(cacheDir, cacheSize)

    private val okHttpClient = OkHttpClient.Builder()
        .cache(cache)
        .addInterceptor { chain ->
            val request = chain.request()
            val response = chain.proceed(request)
            
            when {
                isNetworkAvailable() -> {
                    // Online: Cache für 1 Stunde
                    response.newBuilder()
                        .header("Cache-Control", "public, max-age=3600")
                        .build()
                }
                else -> {
                    // Offline: Cache für 7 Tage
                    response.newBuilder()
                        .header("Cache-Control", "public, max-stale=604800")
                        .build()
                }
            }
        }
        .connectTimeout(30, TimeUnit.SECONDS)
        .readTimeout(30, TimeUnit.SECONDS)
        .writeTimeout(30, TimeUnit.SECONDS)
        .build()

    fun isNetworkAvailable(): Boolean {
        val network = connectivityManager.activeNetwork ?: return false
        val capabilities = connectivityManager.getNetworkCapabilities(network) ?: return false

        return capabilities.hasCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET) &&
                capabilities.hasCapability(NetworkCapabilities.NET_CAPABILITY_VALIDATED)
    }

    fun getNetworkType(): NetworkType {
        val network = connectivityManager.activeNetwork ?: return NetworkType.NONE
        val capabilities = connectivityManager.getNetworkCapabilities(network) ?: return NetworkType.NONE

        return when {
            capabilities.hasTransport(NetworkCapabilities.TRANSPORT_WIFI) -> NetworkType.WIFI
            capabilities.hasTransport(NetworkCapabilities.TRANSPORT_CELLULAR) -> NetworkType.CELLULAR
            else -> NetworkType.OTHER
        }
    }

    fun observeNetworkStatus(): Flow<NetworkStatus> = flow {
        emit(NetworkStatus(isNetworkAvailable(), getNetworkType()))
    }

    fun optimizeRequest(request: Request): Request {
        return request.newBuilder()
            .apply {
                if (!isNetworkAvailable()) {
                    // Offline: Verwende Cache
                    cacheControl(okhttp3.CacheControl.FORCE_CACHE)
                } else {
                    // Online: Verwende Netzwerk mit Cache-Fallback
                    cacheControl(okhttp3.CacheControl.FORCE_NETWORK)
                }
            }
            .build()
    }

    fun optimizeResponse(response: Response): Response {
        return response.newBuilder()
            .apply {
                if (!isNetworkAvailable()) {
                    // Offline: Cache für 7 Tage
                    header("Cache-Control", "public, max-stale=604800")
                } else {
                    // Online: Cache für 1 Stunde
                    header("Cache-Control", "public, max-age=3600")
                }
            }
            .build()
    }

    fun clearCache() {
        cache.evictAll()
    }

    fun getCacheSize(): Long {
        return cache.size()
    }

    fun getCacheHitCount(): Long {
        return cache.hitCount()
    }

    fun getCacheMissCount(): Long {
        return cache.missCount()
    }

    fun getCacheRequestCount(): Long {
        return cache.requestCount()
    }
}

enum class NetworkType {
    NONE,
    WIFI,
    CELLULAR,
    OTHER
}

data class NetworkStatus(
    val isAvailable: Boolean,
    val type: NetworkType
) 