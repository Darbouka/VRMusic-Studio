package com.stompcoin.app.localization

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
class LocalizationTest {
    @get:Rule
    var hiltRule = HiltAndroidRule(this)

    @Inject
    lateinit var localizationManager: LocalizationManager

    private lateinit var context: Context

    @Before
    fun setup() {
        hiltRule.inject()
        context = ApplicationProvider.getApplicationContext()
    }

    @Test
    fun `should load German translations`() {
        // Arrange
        localizationManager.setLocale("de")

        // Act
        val translations = localizationManager.getTranslations()

        // Assert
        assert(translations["welcome"] == "Willkommen bei StompCoin")
        assert(translations["steps"] == "Schritte")
        assert(translations["coins"] == "Münzen")
    }

    @Test
    fun `should load English translations`() {
        // Arrange
        localizationManager.setLocale("en")

        // Act
        val translations = localizationManager.getTranslations()

        // Assert
        assert(translations["welcome"] == "Welcome to StompCoin")
        assert(translations["steps"] == "Steps")
        assert(translations["coins"] == "Coins")
    }

    @Test
    fun `should handle missing translations`() {
        // Arrange
        localizationManager.setLocale("fr")

        // Act
        val translations = localizationManager.getTranslations()

        // Assert
        assert(translations["welcome"] == "Welcome to StompCoin") // Fallback to English
    }

    @Test
    fun `should format numbers according to locale`() {
        // Arrange
        localizationManager.setLocale("de")
        val number = 1234.56

        // Act
        val formattedNumber = localizationManager.formatNumber(number)

        // Assert
        assert(formattedNumber == "1.234,56")
    }

    @Test
    fun `should format currency according to locale`() {
        // Arrange
        localizationManager.setLocale("de")
        val amount = 1234.56

        // Act
        val formattedAmount = localizationManager.formatCurrency(amount)

        // Assert
        assert(formattedAmount == "1.234,56 €")
    }

    @Test
    fun `should format date according to locale`() {
        // Arrange
        localizationManager.setLocale("de")
        val date = java.util.Date(1646092800000) // 2022-03-01

        // Act
        val formattedDate = localizationManager.formatDate(date)

        // Assert
        assert(formattedDate == "01.03.2022")
    }

    @Test
    fun `should handle plural forms`() {
        // Arrange
        localizationManager.setLocale("de")

        // Act
        val singular = localizationManager.getPlural("step", 1)
        val plural = localizationManager.getPlural("step", 5)

        // Assert
        assert(singular == "Schritt")
        assert(plural == "Schritte")
    }

    @Test
    fun `should handle dynamic content`() {
        // Arrange
        localizationManager.setLocale("de")
        val params = mapOf(
            "name" to "Max",
            "steps" to "1000"
        )

        // Act
        val message = localizationManager.formatString("welcome_user", params)

        // Assert
        assert(message == "Willkommen, Max! Du hast 1000 Schritte gemacht.")
    }

    @Test
    fun `should handle locale changes`() {
        // Arrange
        localizationManager.setLocale("de")
        val initialTranslations = localizationManager.getTranslations()

        // Act
        localizationManager.setLocale("en")
        val newTranslations = localizationManager.getTranslations()

        // Assert
        assert(initialTranslations["welcome"] != newTranslations["welcome"])
    }

    @Test
    fun `should handle RTL languages`() {
        // Arrange
        localizationManager.setLocale("ar")

        // Act
        val isRTL = localizationManager.isRTL()

        // Assert
        assert(isRTL)
    }

    @Test
    fun `should handle locale preferences`() {
        // Arrange
        val preferredLocales = listOf("de", "en", "fr")

        // Act
        localizationManager.setPreferredLocales(preferredLocales)
        val currentLocale = localizationManager.getCurrentLocale()

        // Assert
        assert(preferredLocales.contains(currentLocale))
    }

    @Test
    fun `should handle locale fallbacks`() {
        // Arrange
        localizationManager.setLocale("es")

        // Act
        val translations = localizationManager.getTranslations()

        // Assert
        assert(translations["welcome"] == "Welcome to StompCoin") // Fallback to English
    }
} 