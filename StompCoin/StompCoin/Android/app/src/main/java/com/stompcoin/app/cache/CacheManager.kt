package com.stompcoin.app.cache

import android.content.Context
import androidx.room.Room
import com.stompcoin.app.data.local.StompCoinDatabase
import dagger.hilt.android.qualifiers.ApplicationContext
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.map
import javax.inject.Inject
import javax.inject.Singleton

@Singleton
class CacheManager @Inject constructor(
    @ApplicationContext private val context: Context
) {
    private val database = Room.databaseBuilder(
        context,
        StompCoinDatabase::class.java,
        "stompcoin_cache"
    ).build()

    // Cache-Konfiguration
    private val cacheConfig = CacheConfig(
        maxSize = 50 * 1024 * 1024, // 50MB
        maxAge = 24 * 60 * 60 * 1000 // 24 Stunden
    )

    // Cache-Statistiken
    private val cacheStats = CacheStats()

    fun <T> cache(
        key: String,
        data: T,
        maxAge: Long = cacheConfig.maxAge
    ) {
        val cacheEntry = CacheEntry(
            key = key,
            data = data,
            timestamp = System.currentTimeMillis(),
            maxAge = maxAge
        )
        database.cacheDao().insert(cacheEntry)
        cacheStats.recordWrite()
    }

    fun <T> getCached(key: String): T? {
        val entry = database.cacheDao().get(key)
        return if (entry != null && !entry.isExpired()) {
            cacheStats.recordHit()
            entry.data as T
        } else {
            cacheStats.recordMiss()
            null
        }
    }

    fun <T> observeCached(key: String): Flow<T?> {
        return database.cacheDao().observe(key).map { entry ->
            if (entry != null && !entry.isExpired()) {
                cacheStats.recordHit()
                entry.data as T
            } else {
                cacheStats.recordMiss()
                null
            }
        }
    }

    fun clearCache() {
        database.cacheDao().clearAll()
        cacheStats.reset()
    }

    fun clearExpiredCache() {
        database.cacheDao().clearExpired(System.currentTimeMillis())
    }

    fun getCacheStats(): CacheStats {
        return cacheStats.copy()
    }

    fun isCacheValid(key: String): Boolean {
        val entry = database.cacheDao().get(key)
        return entry != null && !entry.isExpired()
    }

    fun getCacheSize(): Long {
        return database.cacheDao().getCacheSize()
    }

    fun isCacheFull(): Boolean {
        return getCacheSize() >= cacheConfig.maxSize
    }

    fun removeOldestEntries() {
        if (isCacheFull()) {
            val entriesToRemove = (getCacheSize() - cacheConfig.maxSize) / 1024
            database.cacheDao().removeOldestEntries(entriesToRemove)
        }
    }
}

data class CacheConfig(
    val maxSize: Long,
    val maxAge: Long
)

data class CacheEntry(
    val key: String,
    val data: Any,
    val timestamp: Long,
    val maxAge: Long
) {
    fun isExpired(): Boolean {
        return System.currentTimeMillis() - timestamp > maxAge
    }
}

data class CacheStats(
    var hits: Long = 0,
    var misses: Long = 0,
    var writes: Long = 0
) {
    fun recordHit() {
        hits++
    }

    fun recordMiss() {
        misses++
    }

    fun recordWrite() {
        writes++
    }

    fun reset() {
        hits = 0
        misses = 0
        writes = 0
    }

    fun hitRate(): Double {
        val total = hits + misses
        return if (total > 0) hits.toDouble() / total else 0.0
    }
} 