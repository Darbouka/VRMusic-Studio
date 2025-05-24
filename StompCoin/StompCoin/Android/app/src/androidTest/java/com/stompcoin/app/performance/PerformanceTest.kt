package com.stompcoin.app.performance

import android.content.Context
import androidx.benchmark.junit4.BenchmarkRule
import androidx.benchmark.junit4.measureRepeated
import androidx.test.core.app.ApplicationProvider
import com.stompcoin.app.cache.CacheManager
import com.stompcoin.app.network.NetworkOptimizer
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
class PerformanceTest {
    @get:Rule
    val benchmarkRule = BenchmarkRule()

    @get:Rule
    var hiltRule = HiltAndroidRule(this)

    @Inject
    lateinit var cacheManager: CacheManager

    @Inject
    lateinit var networkOptimizer: NetworkOptimizer

    private lateinit var context: Context

    @Before
    fun setup() {
        hiltRule.inject()
        context = ApplicationProvider.getApplicationContext()
    }

    @Test
    fun benchmarkCacheOperations() {
        benchmarkRule.measureRepeated {
            // Arrange
            val key = "test_key_${runWithTimingDisabled { System.currentTimeMillis() }}"
            val data = "test_data".repeat(1000)

            // Act & Assert
            runWithTimingDisabled {
                cacheManager.put(key, data)
            }

            val cachedData = cacheManager.get<String>(key)
            assert(cachedData == data)
        }
    }

    @Test
    fun benchmarkNetworkOptimization() {
        benchmarkRule.measureRepeated {
            // Arrange
            val request = okhttp3.Request.Builder()
                .url("https://api.stompcoin.com/test")
                .build()

            // Act
            val optimizedRequest = networkOptimizer.optimizeRequest(request)

            // Assert
            assert(optimizedRequest.cacheControl() != null)
        }
    }

    @Test
    fun benchmarkConcurrentCacheAccess() = runBlocking {
        benchmarkRule.measureRepeated {
            // Arrange
            val operations = List(100) { index ->
                kotlinx.coroutines.async {
                    val key = "test_key_$index"
                    val data = "test_data_$index"
                    cacheManager.put(key, data)
                    cacheManager.get<String>(key)
                }
            }

            // Act
            val results = operations.map { it.await() }

            // Assert
            assert(results.all { it != null })
        }
    }

    @Test
    fun benchmarkLargeDataSetCaching() {
        benchmarkRule.measureRepeated {
            // Arrange
            val largeDataSet = List(1000) { index ->
                "data_$index".repeat(100)
            }

            // Act
            runWithTimingDisabled {
                largeDataSet.forEachIndexed { index, data ->
                    cacheManager.put("key_$index", data)
                }
            }

            // Assert
            val retrievedData = largeDataSet.mapIndexed { index, _ ->
                cacheManager.get<String>("key_$index")
            }
            assert(retrievedData.all { it != null })
        }
    }

    @Test
    fun benchmarkCacheEviction() {
        benchmarkRule.measureRepeated {
            // Arrange
            val cacheSize = 1000
            val data = "test_data".repeat(100)

            // Act
            runWithTimingDisabled {
                repeat(cacheSize + 100) { index ->
                    cacheManager.put("key_$index", data)
                }
            }

            // Assert
            val cacheStats = cacheManager.getCacheStats()
            assert(cacheStats.size <= cacheSize)
        }
    }

    @Test
    fun benchmarkNetworkTypeDetection() {
        benchmarkRule.measureRepeated {
            // Act
            val networkType = networkOptimizer.getNetworkType()

            // Assert
            assert(networkType != null)
        }
    }

    @Test
    fun benchmarkConcurrentNetworkOperations() = runBlocking {
        benchmarkRule.measureRepeated {
            // Arrange
            val requests = List(100) { index ->
                okhttp3.Request.Builder()
                    .url("https://api.stompcoin.com/test/$index")
                    .build()
            }

            // Act
            val optimizedRequests = requests.map { request ->
                networkOptimizer.optimizeRequest(request)
            }

            // Assert
            assert(optimizedRequests.all { it.cacheControl() != null })
        }
    }

    @Test
    fun benchmarkCacheHitRate() {
        benchmarkRule.measureRepeated {
            // Arrange
            val key = "test_key"
            val data = "test_data"

            // Act
            runWithTimingDisabled {
                cacheManager.put(key, data)
                repeat(100) {
                    cacheManager.get<String>(key)
                }
            }

            // Assert
            val stats = cacheManager.getCacheStats()
            assert(stats.hitCount > 0)
        }
    }

    @Test
    fun benchmarkMemoryUsage() {
        benchmarkRule.measureRepeated {
            // Arrange
            val largeData = ByteArray(1024 * 1024) // 1MB

            // Act
            runWithTimingDisabled {
                repeat(10) { index ->
                    cacheManager.put("large_key_$index", largeData)
                }
            }

            // Assert
            val stats = cacheManager.getCacheStats()
            assert(stats.size > 0)
        }
    }
} 