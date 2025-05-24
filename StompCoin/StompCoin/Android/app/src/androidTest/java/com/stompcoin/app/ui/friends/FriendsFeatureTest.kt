package com.stompcoin.app.ui.friends

import androidx.compose.ui.test.*
import androidx.compose.ui.test.junit4.createComposeRule
import androidx.test.ext.junit.runners.AndroidJUnit4
import com.stompcoin.app.data.model.Friend
import com.stompcoin.app.data.model.FriendActivity
import com.stompcoin.app.data.model.FriendRequest
import com.stompcoin.app.data.model.FriendStats
import dagger.hilt.android.testing.HiltAndroidRule
import dagger.hilt.android.testing.HiltAndroidTest
import org.junit.Before
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith

@HiltAndroidTest
@RunWith(AndroidJUnit4::class)
class FriendsFeatureTest {
    @get:Rule(order = 0)
    val hiltRule = HiltAndroidRule(this)

    @get:Rule(order = 1)
    val composeRule = createComposeRule()

    private lateinit var viewModel: FriendsViewModel

    @Before
    fun setup() {
        hiltRule.inject()
        viewModel = FriendsViewModel()
    }

    @Test
    fun testCompleteFriendRequestFlow() {
        // Arrange
        val testUser = "testUser"
        val testFriend = "testFriend"
        val testRequest = FriendRequest("1", testUser, testFriend, "PENDING", "Hallo!")

        // Act & Assert: Freundschaftsanfrage senden
        composeRule.setContent {
            FriendsScreen(viewModel = viewModel)
        }

        // Suche nach Benutzer
        composeRule.onNodeWithContentDescription("Suche").performClick()
        composeRule.onNodeWithTag("searchBar").performTextInput(testFriend)
        
        // Warte auf Suchergebnisse
        composeRule.waitUntil(timeoutMillis = 5000) {
            composeRule.onAllNodesWithTag("userSearchItem").fetchSemanticsNodes().isNotEmpty()
        }

        // Klicke auf "Freund hinzufügen"
        composeRule.onNodeWithText("Freund hinzufügen").performClick()

        // Bestätige Dialog
        composeRule.onNodeWithText("Bestätigen").performClick()

        // Überprüfe Erfolgsmeldung
        composeRule.onNodeWithText("Freundschaftsanfrage gesendet").assertExists()

        // Act & Assert: Freundschaftsanfrage akzeptieren
        viewModel.acceptFriendRequest(testRequest.id)

        // Überprüfe aktualisierte Freundesliste
        composeRule.waitUntil(timeoutMillis = 5000) {
            composeRule.onAllNodesWithTag("friendItem").fetchSemanticsNodes().isNotEmpty()
        }

        composeRule.onNodeWithText(testFriend).assertExists()
    }

    @Test
    fun testFriendActivityDisplay() {
        // Arrange
        val activities = listOf(
            FriendActivity("1", "user1", "STEPS_COMPLETED", "10000 Schritte erreicht!"),
            FriendActivity("2", "user1", "CHALLENGE_WON", "Challenge gewonnen!")
        )

        // Act
        composeRule.setContent {
            FriendsScreen(
                viewModel = viewModel.apply {
                    updateActivities(activities)
                }
            )
        }

        // Assert
        activities.forEach { activity ->
            composeRule.onNodeWithText(activity.description).assertExists()
        }
    }

    @Test
    fun testFriendStatsDisplay() {
        // Arrange
        val stats = FriendStats(
            totalFriends = 10,
            activeFriends = 5,
            pendingRequests = 2,
            recentActivities = emptyList()
        )

        // Act
        composeRule.setContent {
            FriendsScreen(
                viewModel = viewModel.apply {
                    updateStats(stats)
                }
            )
        }

        // Assert
        composeRule.onNodeWithText("10 Freunde").assertExists()
        composeRule.onNodeWithText("5 aktiv").assertExists()
        composeRule.onNodeWithText("2 Anfragen").assertExists()
    }

    @Test
    fun testFriendRemoval() {
        // Arrange
        val friend = Friend("1", "user1", "friend1", "ACCEPTED")
        
        // Act
        composeRule.setContent {
            FriendsScreen(
                viewModel = viewModel.apply {
                    updateFriends(listOf(friend))
                }
            )
        }

        // Klicke auf Freund-Menü
        composeRule.onNodeWithTag("friendItem").performTouchInput {
            longClick()
        }

        // Klicke auf "Freund entfernen"
        composeRule.onNodeWithText("Freund entfernen").performClick()

        // Bestätige Dialog
        composeRule.onNodeWithText("Bestätigen").performClick()

        // Assert
        composeRule.onNodeWithText("Freund entfernt").assertExists()
        composeRule.onNodeWithText("friend1").assertDoesNotExist()
    }

    @Test
    fun testSearchFunctionality() {
        // Arrange
        val friends = listOf(
            Friend("1", "user1", "test1", "ACCEPTED"),
            Friend("2", "user1", "test2", "ACCEPTED"),
            Friend("3", "user1", "other", "ACCEPTED")
        )

        // Act
        composeRule.setContent {
            FriendsScreen(
                viewModel = viewModel.apply {
                    updateFriends(friends)
                }
            )
        }

        // Suche nach "test"
        composeRule.onNodeWithContentDescription("Suche").performClick()
        composeRule.onNodeWithTag("searchBar").performTextInput("test")

        // Assert
        composeRule.onNodeWithText("test1").assertExists()
        composeRule.onNodeWithText("test2").assertExists()
        composeRule.onNodeWithText("other").assertDoesNotExist()
    }

    @Test
    fun testErrorHandling() {
        // Arrange
        val errorMessage = "Ein Fehler ist aufgetreten"

        // Act
        composeRule.setContent {
            FriendsScreen(
                viewModel = viewModel.apply {
                    updateError(errorMessage)
                }
            )
        }

        // Assert
        composeRule.onNodeWithText(errorMessage).assertExists()
    }
} 