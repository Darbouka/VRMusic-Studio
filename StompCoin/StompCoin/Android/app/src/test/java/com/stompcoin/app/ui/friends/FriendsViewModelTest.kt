package com.stompcoin.app.ui.friends

import com.stompcoin.app.data.model.Friend
import com.stompcoin.app.data.model.FriendActivity
import com.stompcoin.app.data.model.FriendRequest
import com.stompcoin.app.data.model.FriendStats
import com.stompcoin.app.data.repository.FriendRepository
import io.mockk.coEvery
import io.mockk.coVerify
import io.mockk.mockk
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.ExperimentalCoroutinesApi
import kotlinx.coroutines.flow.flowOf
import kotlinx.coroutines.test.*
import org.junit.After
import org.junit.Before
import org.junit.Test

@ExperimentalCoroutinesApi
class FriendsViewModelTest {
    private lateinit var viewModel: FriendsViewModel
    private lateinit var repository: FriendRepository
    private val testDispatcher = StandardTestDispatcher()

    @Before
    fun setup() {
        Dispatchers.setMain(testDispatcher)
        repository = mockk()
        viewModel = FriendsViewModel(repository)
    }

    @After
    fun tearDown() {
        Dispatchers.resetMain()
    }

    @Test
    fun `loadFriends should update friends list`() = runTest {
        // Arrange
        val friends = listOf(
            Friend("1", "user1", "friend1", "ACCEPTED"),
            Friend("2", "user1", "friend2", "ACCEPTED")
        )
        coEvery { repository.getFriends(any()) } returns flowOf(friends)

        // Act
        viewModel.loadFriends("user1")
        testDispatcher.scheduler.advanceUntilIdle()

        // Assert
        assert(viewModel.uiState.value.friends == friends)
    }

    @Test
    fun `loadPendingRequests should update pending requests list`() = runTest {
        // Arrange
        val requests = listOf(
            FriendRequest("1", "sender1", "user1", "PENDING", "Hallo!"),
            FriendRequest("2", "sender2", "user1", "PENDING", "Freunde?")
        )
        coEvery { repository.getPendingRequests(any()) } returns flowOf(requests)

        // Act
        viewModel.loadPendingRequests("user1")
        testDispatcher.scheduler.advanceUntilIdle()

        // Assert
        assert(viewModel.uiState.value.pendingRequests == requests)
    }

    @Test
    fun `loadFriendActivities should update activities list`() = runTest {
        // Arrange
        val activities = listOf(
            FriendActivity("1", "user1", "STEPS_COMPLETED", "10000 Schritte erreicht!"),
            FriendActivity("2", "user1", "CHALLENGE_WON", "Challenge gewonnen!")
        )
        coEvery { repository.getFriendActivities(any()) } returns flowOf(activities)

        // Act
        viewModel.loadFriendActivities("user1")
        testDispatcher.scheduler.advanceUntilIdle()

        // Assert
        assert(viewModel.uiState.value.friendActivities == activities)
    }

    @Test
    fun `loadFriendStats should update stats`() = runTest {
        // Arrange
        val stats = FriendStats(
            totalFriends = 10,
            activeFriends = 5,
            pendingRequests = 2,
            recentActivities = emptyList()
        )
        coEvery { repository.getFriendStats(any()) } returns flowOf(stats)

        // Act
        viewModel.loadFriendStats("user1")
        testDispatcher.scheduler.advanceUntilIdle()

        // Assert
        assert(viewModel.uiState.value.friendStats == stats)
    }

    @Test
    fun `sendFriendRequest should call repository`() = runTest {
        // Arrange
        val request = FriendRequest("1", "user1", "friend1", "PENDING", "Hallo!")
        coEvery { repository.sendFriendRequest(any()) } returns flowOf(request)

        // Act
        viewModel.sendFriendRequest("friend1")
        testDispatcher.scheduler.advanceUntilIdle()

        // Assert
        coVerify { repository.sendFriendRequest(any()) }
    }

    @Test
    fun `acceptFriendRequest should call repository`() = runTest {
        // Arrange
        coEvery { repository.acceptFriendRequest(any()) } returns flowOf(Unit)

        // Act
        viewModel.acceptFriendRequest("request1")
        testDispatcher.scheduler.advanceUntilIdle()

        // Assert
        coVerify { repository.acceptFriendRequest("request1") }
    }

    @Test
    fun `rejectFriendRequest should call repository`() = runTest {
        // Arrange
        coEvery { repository.rejectFriendRequest(any()) } returns flowOf(Unit)

        // Act
        viewModel.rejectFriendRequest("request1")
        testDispatcher.scheduler.advanceUntilIdle()

        // Assert
        coVerify { repository.rejectFriendRequest("request1") }
    }

    @Test
    fun `removeFriend should call repository`() = runTest {
        // Arrange
        coEvery { repository.removeFriend(any()) } returns flowOf(Unit)

        // Act
        viewModel.removeFriend("friend1")
        testDispatcher.scheduler.advanceUntilIdle()

        // Assert
        coVerify { repository.removeFriend("friend1") }
    }

    @Test
    fun `searchUsers should update search results`() = runTest {
        // Arrange
        val query = "test"
        val results = listOf(
            Friend("1", "user1", "test1", "PENDING"),
            Friend("2", "user1", "test2", "PENDING")
        )
        coEvery { repository.searchUsers(query) } returns flowOf(results)

        // Act
        viewModel.searchUsers(query)
        testDispatcher.scheduler.advanceUntilIdle()

        // Assert
        assert(viewModel.uiState.value.searchResults == results)
    }

    @Test
    fun `clearError should reset error state`() = runTest {
        // Arrange
        viewModel.setError("Test error")

        // Act
        viewModel.clearError()
        testDispatcher.scheduler.advanceUntilIdle()

        // Assert
        assert(viewModel.uiState.value.error == null)
    }
} 