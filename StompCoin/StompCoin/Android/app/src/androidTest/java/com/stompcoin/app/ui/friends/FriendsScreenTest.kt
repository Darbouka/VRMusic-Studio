package com.stompcoin.app.ui.friends

import androidx.compose.ui.test.*
import androidx.compose.ui.test.junit4.createComposeRule
import com.stompcoin.app.data.model.Friend
import com.stompcoin.app.data.model.FriendActivity
import com.stompcoin.app.data.model.FriendRequest
import com.stompcoin.app.data.model.FriendStats
import org.junit.Rule
import org.junit.Test

class FriendsScreenTest {
    @get:Rule
    val composeTestRule = createComposeRule()

    @Test
    fun whenLoading_shouldShowLoadingIndicator() {
        // Arrange
        val uiState = FriendsUiState(
            isLoading = true,
            friends = emptyList(),
            pendingRequests = emptyList(),
            friendActivities = emptyList()
        )

        // Act
        composeTestRule.setContent {
            FriendsScreen(
                viewModel = mockViewModel(uiState),
                onNavigateToProfile = {}
            )
        }

        // Assert
        composeTestRule.onNodeWithContentDescription("Laden...").assertExists()
    }

    @Test
    fun whenFriendsLoaded_shouldDisplayFriendsList() {
        // Arrange
        val friends = listOf(
            Friend("1", "user1", "friend1", "ACCEPTED"),
            Friend("2", "user1", "friend2", "ACCEPTED")
        )
        val uiState = FriendsUiState(
            isLoading = false,
            friends = friends,
            pendingRequests = emptyList(),
            friendActivities = emptyList()
        )

        // Act
        composeTestRule.setContent {
            FriendsScreen(
                viewModel = mockViewModel(uiState),
                onNavigateToProfile = {}
            )
        }

        // Assert
        composeTestRule.onNodeWithText("Freunde").assertExists()
        friends.forEach { friend ->
            composeTestRule.onNodeWithText(friend.friendId).assertExists()
        }
    }

    @Test
    fun whenPendingRequestsExist_shouldDisplayRequestsList() {
        // Arrange
        val requests = listOf(
            FriendRequest("1", "sender1", "user1", "PENDING", "Hallo!"),
            FriendRequest("2", "sender2", "user1", "PENDING", "Freunde?")
        )
        val uiState = FriendsUiState(
            isLoading = false,
            friends = emptyList(),
            pendingRequests = requests,
            friendActivities = emptyList()
        )

        // Act
        composeTestRule.setContent {
            FriendsScreen(
                viewModel = mockViewModel(uiState),
                onNavigateToProfile = {}
            )
        }

        // Assert
        composeTestRule.onNodeWithText("Ausstehende Anfragen").assertExists()
        requests.forEach { request ->
            composeTestRule.onNodeWithText(request.senderId).assertExists()
            composeTestRule.onNodeWithText(request.message).assertExists()
        }
    }

    @Test
    fun whenFriendActivitiesExist_shouldDisplayActivitiesList() {
        // Arrange
        val activities = listOf(
            FriendActivity("1", "user1", "STEPS_COMPLETED", "10000 Schritte erreicht!"),
            FriendActivity("2", "user1", "CHALLENGE_WON", "Challenge gewonnen!")
        )
        val uiState = FriendsUiState(
            isLoading = false,
            friends = emptyList(),
            pendingRequests = emptyList(),
            friendActivities = activities
        )

        // Act
        composeTestRule.setContent {
            FriendsScreen(
                viewModel = mockViewModel(uiState),
                onNavigateToProfile = {}
            )
        }

        // Assert
        composeTestRule.onNodeWithText("Aktivitäten").assertExists()
        activities.forEach { activity ->
            composeTestRule.onNodeWithText(activity.description).assertExists()
        }
    }

    @Test
    fun whenSearching_shouldShowSearchResults() {
        // Arrange
        val searchResults = listOf(
            Friend("1", "user1", "test1", "PENDING"),
            Friend("2", "user1", "test2", "PENDING")
        )
        val uiState = FriendsUiState(
            isLoading = false,
            friends = emptyList(),
            pendingRequests = emptyList(),
            friendActivities = emptyList(),
            searchResults = searchResults
        )

        // Act
        composeTestRule.setContent {
            FriendsScreen(
                viewModel = mockViewModel(uiState),
                onNavigateToProfile = {}
            )
        }

        // Click search icon
        composeTestRule.onNodeWithContentDescription("Suchen").performClick()

        // Assert
        searchResults.forEach { friend ->
            composeTestRule.onNodeWithText(friend.friendId).assertExists()
        }
    }

    @Test
    fun whenErrorOccurs_shouldShowErrorSnackbar() {
        // Arrange
        val errorMessage = "Ein Fehler ist aufgetreten"
        val uiState = FriendsUiState(
            isLoading = false,
            friends = emptyList(),
            pendingRequests = emptyList(),
            friendActivities = emptyList(),
            error = errorMessage
        )

        // Act
        composeTestRule.setContent {
            FriendsScreen(
                viewModel = mockViewModel(uiState),
                onNavigateToProfile = {}
            )
        }

        // Assert
        composeTestRule.onNodeWithText(errorMessage).assertExists()
    }

    private fun mockViewModel(uiState: FriendsUiState): FriendsViewModel {
        return object : FriendsViewModel(mockk()) {
            override val uiState = kotlinx.coroutines.flow.MutableStateFlow(uiState)
        }
    }
} 