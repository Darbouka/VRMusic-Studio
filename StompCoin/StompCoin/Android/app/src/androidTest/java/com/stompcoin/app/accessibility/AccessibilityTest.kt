package com.stompcoin.app.accessibility

import android.content.Context
import androidx.compose.ui.test.*
import androidx.compose.ui.test.junit4.createComposeRule
import androidx.test.core.app.ApplicationProvider
import com.stompcoin.app.ui.components.AccessibleButton
import com.stompcoin.app.ui.components.AccessibleText
import com.stompcoin.app.ui.components.AccessibleImage
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
class AccessibilityTest {
    @get:Rule
    val composeTestRule = createComposeRule()

    @get:Rule
    var hiltRule = HiltAndroidRule(this)

    @Inject
    lateinit var accessibilityManager: AccessibilityManager

    private lateinit var context: Context

    @Before
    fun setup() {
        hiltRule.inject()
        context = ApplicationProvider.getApplicationContext()
    }

    @Test
    fun `should have proper content description for images`() {
        // Arrange
        composeTestRule.setContent {
            AccessibleImage(
                imageUrl = "https://example.com/image.jpg",
                contentDescription = "StompCoin Logo"
            )
        }

        // Assert
        composeTestRule.onNodeWithContentDescription("StompCoin Logo").assertExists()
    }

    @Test
    fun `should have proper text contrast ratio`() {
        // Arrange
        composeTestRule.setContent {
            AccessibleText(
                text = "Wichtiger Text",
                style = androidx.compose.ui.text.TextStyle(
                    color = androidx.compose.ui.graphics.Color.Black
                )
            )
        }

        // Act
        val contrastRatio = accessibilityManager.calculateContrastRatio(
            foreground = androidx.compose.ui.graphics.Color.Black,
            background = androidx.compose.ui.graphics.Color.White
        )

        // Assert
        assert(contrastRatio >= 4.5) // WCAG AA Standard
    }

    @Test
    fun `should have proper touch target size`() {
        // Arrange
        composeTestRule.setContent {
            AccessibleButton(
                onClick = {},
                contentDescription = "Großer Button"
            ) {
                AccessibleText("Klick mich")
            }
        }

        // Act
        val touchTarget = composeTestRule.onNodeWithContentDescription("Großer Button")
            .fetchSemanticsNode()

        // Assert
        assert(touchTarget.boundsInRoot.width >= 48)
        assert(touchTarget.boundsInRoot.height >= 48)
    }

    @Test
    fun `should announce important changes`() {
        // Arrange
        composeTestRule.setContent {
            AccessibleText(
                text = "Neue Nachricht",
                isAnnouncement = true
            )
        }

        // Assert
        composeTestRule.onNodeWithText("Neue Nachricht")
            .assertIsDisplayed()
    }

    @Test
    fun `should handle screen reader navigation`() {
        // Arrange
        composeTestRule.setContent {
            AccessibleButton(
                onClick = {},
                contentDescription = "Navigation Button",
                isFocusable = true
            ) {
                AccessibleText("Navigation")
            }
        }

        // Act
        composeTestRule.onNodeWithContentDescription("Navigation Button")
            .performTouchInput { click() }

        // Assert
        assert(accessibilityManager.isFocused("Navigation Button"))
    }

    @Test
    fun `should have proper heading hierarchy`() {
        // Arrange
        composeTestRule.setContent {
            AccessibleText(
                text = "Hauptüberschrift",
                style = androidx.compose.ui.text.TextStyle(
                    fontSize = androidx.compose.ui.unit.sp(24)
                ),
                isHeading = true
            )
        }

        // Assert
        composeTestRule.onNodeWithText("Hauptüberschrift")
            .assertIsDisplayed()
    }

    @Test
    fun `should handle dynamic text sizing`() {
        // Arrange
        val textSize = 24f
        composeTestRule.setContent {
            AccessibleText(
                text = "Anpassbarer Text",
                style = androidx.compose.ui.text.TextStyle(
                    fontSize = androidx.compose.ui.unit.sp(textSize)
                )
            )
        }

        // Act
        accessibilityManager.setFontSize(1.5f)

        // Assert
        val scaledSize = textSize * 1.5f
        composeTestRule.onNodeWithText("Anpassbarer Text")
            .assertExists()
    }

    @Test
    fun `should handle color blindness`() {
        // Arrange
        composeTestRule.setContent {
            AccessibleText(
                text = "Farbenblind-freundlicher Text",
                style = androidx.compose.ui.text.TextStyle(
                    color = androidx.compose.ui.graphics.Color.Blue
                )
            )
        }

        // Act
        accessibilityManager.setColorBlindMode(true)

        // Assert
        val isColorBlindFriendly = accessibilityManager.isColorBlindFriendly(
            foreground = androidx.compose.ui.graphics.Color.Blue,
            background = androidx.compose.ui.graphics.Color.White
        )
        assert(isColorBlindFriendly)
    }

    @Test
    fun `should handle reduced motion`() {
        // Arrange
        composeTestRule.setContent {
            AccessibleText(
                text = "Text ohne Animation",
                style = androidx.compose.ui.text.TextStyle(
                    color = androidx.compose.ui.graphics.Color.Black
                )
            )
        }

        // Act
        accessibilityManager.setReducedMotion(true)

        // Assert
        assert(!accessibilityManager.isAnimationEnabled())
    }

    @Test
    fun `should handle screen reader focus`() {
        // Arrange
        composeTestRule.setContent {
            AccessibleButton(
                onClick = {},
                contentDescription = "Fokussierbarer Button",
                isFocusable = true
            ) {
                AccessibleText("Fokus")
            }
        }

        // Act
        accessibilityManager.setScreenReaderFocus(true)

        // Assert
        assert(accessibilityManager.isScreenReaderFocused("Fokussierbarer Button"))
    }

    @Test
    fun `should handle accessibility shortcuts`() {
        // Arrange
        composeTestRule.setContent {
            AccessibleButton(
                onClick = {},
                contentDescription = "Shortcut Button",
                shortcut = "Alt+S"
            ) {
                AccessibleText("Shortcut")
            }
        }

        // Act
        val shortcut = accessibilityManager.getAccessibilityShortcut("Shortcut Button")

        // Assert
        assert(shortcut == "Alt+S")
    }
} 