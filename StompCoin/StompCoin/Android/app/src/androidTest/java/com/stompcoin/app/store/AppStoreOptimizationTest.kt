package com.stompcoin.app.store

import android.content.Context
import androidx.test.core.app.ApplicationProvider
import dagger.hilt.android.testing.HiltAndroidRule
import dagger.hilt.android.testing.HiltAndroidTest
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
class AppStoreOptimizationTest {
    @get:Rule
    var hiltRule = HiltAndroidRule(this)

    @Inject
    lateinit var appStoreOptimizer: AppStoreOptimizer

    private lateinit var context: Context

    @Before
    fun setup() {
        hiltRule.inject()
        context = ApplicationProvider.getApplicationContext()
    }

    @Test
    fun `should validate app metadata`() {
        // Arrange
        val metadata = AppMetadata(
            title = "StompCoin",
            shortDescription = "Verdiene Münzen durch Bewegung",
            fullDescription = "StompCoin ist eine innovative App, die dich für deine tägliche Bewegung belohnt.",
            keywords = listOf("Fitness", "Gamification", "Belohnungen", "Gesundheit"),
            category = "Gesundheit & Fitness"
        )

        // Act
        val validationResult = appStoreOptimizer.validateMetadata(metadata)

        // Assert
        assert(validationResult.isValid)
        assert(validationResult.errors.isEmpty())
    }

    @Test
    fun `should optimize app screenshots`() {
        // Arrange
        val screenshots = listOf(
            AppScreenshot(
                path = "screenshots/main.png",
                title = "Hauptbildschirm",
                description = "Übersicht deiner täglichen Aktivitäten"
            ),
            AppScreenshot(
                path = "screenshots/rewards.png",
                title = "Belohnungen",
                description = "Sammle Münzen und löse sie ein"
            )
        )

        // Act
        val optimizedScreenshots = appStoreOptimizer.optimizeScreenshots(screenshots)

        // Assert
        assert(optimizedScreenshots.all { it.width == 1080 })
        assert(optimizedScreenshots.all { it.height == 1920 })
    }

    @Test
    fun `should validate app icon`() {
        // Arrange
        val icon = AppIcon(
            path = "icons/app_icon.png",
            size = 512
        )

        // Act
        val validationResult = appStoreOptimizer.validateIcon(icon)

        // Assert
        assert(validationResult.isValid)
        assert(validationResult.errors.isEmpty())
    }

    @Test
    fun `should optimize app description`() {
        // Arrange
        val description = """
            StompCoin ist eine innovative App, die dich für deine tägliche Bewegung belohnt.
            Verdiene Münzen durch Schritte und löse sie für tolle Belohnungen ein.
            Teile deine Erfolge mit Freunden und motiviere dich gegenseitig.
        """.trimIndent()

        // Act
        val optimizedDescription = appStoreOptimizer.optimizeDescription(description)

        // Assert
        assert(optimizedDescription.length <= 4000)
        assert(optimizedDescription.contains("StompCoin"))
        assert(optimizedDescription.contains("Belohnungen"))
    }

    @Test
    fun `should validate app keywords`() {
        // Arrange
        val keywords = listOf(
            "Fitness",
            "Gamification",
            "Belohnungen",
            "Gesundheit",
            "Schritte",
            "Münzen",
            "Motivation"
        )

        // Act
        val validationResult = appStoreOptimizer.validateKeywords(keywords)

        // Assert
        assert(validationResult.isValid)
        assert(validationResult.errors.isEmpty())
    }

    @Test
    fun `should optimize app rating`() {
        // Arrange
        val ratings = listOf(
            AppRating(5, "Tolle App!"),
            AppRating(4, "Sehr gut"),
            AppRating(3, "Okay"),
            AppRating(2, "Könnte besser sein"),
            AppRating(1, "Nicht zu empfehlen")
        )

        // Act
        val averageRating = appStoreOptimizer.calculateAverageRating(ratings)

        // Assert
        assert(averageRating >= 4.0)
    }

    @Test
    fun `should validate app permissions`() {
        // Arrange
        val permissions = listOf(
            "android.permission.ACTIVITY_RECOGNITION",
            "android.permission.ACCESS_FINE_LOCATION",
            "android.permission.INTERNET"
        )

        // Act
        val validationResult = appStoreOptimizer.validatePermissions(permissions)

        // Assert
        assert(validationResult.isValid)
        assert(validationResult.errors.isEmpty())
    }

    @Test
    fun `should optimize app size`() {
        // Arrange
        val appSize = 50 * 1024 * 1024 // 50MB

        // Act
        val optimizationResult = appStoreOptimizer.optimizeAppSize(appSize)

        // Assert
        assert(optimizationResult.optimizedSize < appSize)
        assert(optimizationResult.recommendations.isNotEmpty())
    }

    @Test
    fun `should validate app content rating`() {
        // Arrange
        val contentRating = AppContentRating(
            violence = false,
            sexualContent = false,
            language = false,
            gambling = false
        )

        // Act
        val validationResult = appStoreOptimizer.validateContentRating(contentRating)

        // Assert
        assert(validationResult.isValid)
        assert(validationResult.rating == "Everyone")
    }

    @Test
    fun `should optimize app updates`() {
        // Arrange
        val updateHistory = listOf(
            AppUpdate("1.0.0", "Erste Version"),
            AppUpdate("1.1.0", "Bugfixes"),
            AppUpdate("1.2.0", "Neue Features")
        )

        // Act
        val optimizationResult = appStoreOptimizer.optimizeUpdateStrategy(updateHistory)

        // Assert
        assert(optimizationResult.recommendedFrequency == "2 Wochen")
        assert(optimizationResult.recommendations.isNotEmpty())
    }

    @Test
    fun `should validate app privacy policy`() {
        // Arrange
        val privacyPolicy = """
            Diese App sammelt nur die für die Funktionalität notwendigen Daten.
            Alle Daten werden sicher gespeichert und nicht an Dritte weitergegeben.
        """.trimIndent()

        // Act
        val validationResult = appStoreOptimizer.validatePrivacyPolicy(privacyPolicy)

        // Assert
        assert(validationResult.isValid)
        assert(validationResult.errors.isEmpty())
    }
} 