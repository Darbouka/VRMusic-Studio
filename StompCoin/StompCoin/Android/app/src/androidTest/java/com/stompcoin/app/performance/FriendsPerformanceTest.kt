package com.stompcoin.app.performance

import androidx.benchmark.junit4.BenchmarkRule
import androidx.benchmark.junit4.measureRepeated
import androidx.test.ext.junit.runners.AndroidJUnit4
import com.stompcoin.app.data.model.Friend
import com.stompcoin.app.data.model.FriendActivity
import com.stompcoin.app.data.model.FriendRequest
import com.stompcoin.app.data.model.FriendStats
import com.stompcoin.app.data.repository.FriendRepository
import com.stompcoin.app.ui.friends.FriendsViewModel
import dagger.hilt.android.testing.HiltAndroidRule
import dagger.hilt.android.testing.HiltAndroidTest
import kotlinx.coroutines.flow.first
import kotlinx.coroutines.runBlocking
import org.junit.Before
import org.junit.Rule
import org.junit.Test
import org.junit.runner.RunWith
import javax.inject.Inject

@HiltAndroidTest
@RunWith(AndroidJUnit4::class)
class FriendsPerformanceTest {
    @get:Rule(order = 0)
    val hiltRule = HiltAndroidRule(this)

    @get:Rule(order = 1)
    val benchmarkRule = BenchmarkRule()

    @Inject
    lateinit var repository: FriendRepository

    private lateinit var viewModel: FriendsViewModel

    @Before
    fun setup() {
        hiltRule.inject()
        viewModel = FriendsViewModel()
    }

    @Test
    fun benchmarkLoadFriends() = runBlocking {
        // Arrange
        val friends = List(1000) { index ->
            Friend(
                id = index.toString(),
                userId = "user1",
                friendId = "friend$index",
                status = "ACCEPTED"
            )
        }

        // Act & Assert
        benchmarkRule.measureRepeated {
            repository.getFriends("user1").first()
        }
    }

    @Test
    fun benchmarkSearchUsers() = runBlocking {
        // Arrange
        val friends = List(1000) { index ->
            Friend(
                id = index.toString(),
                userId = "user1",
                friendId = "test$index",
                status = "PENDING"
            )
        }

        // Act & Assert
        benchmarkRule.measureRepeated {
            repository.searchUsers("test").first()
        }
    }

    @Test
    fun benchmarkFriendActivities() = runBlocking {
        // Arrange
        val activities = List(1000) { index ->
            FriendActivity(
                id = index.toString(),
                userId = "user1",
                type = "STEPS_COMPLETED",
                description = "Aktivität $index"
            )
        }

        // Act & Assert
        benchmarkRule.measureRepeated {
            repository.getFriendActivities("user1").first()
        }
    }

    @Test
    fun benchmarkFriendStats() = runBlocking {
        // Arrange
        val stats = FriendStats(
            totalFriends = 1000,
            activeFriends = 500,
            pendingRequests = 100,
            recentActivities = List(100) { index ->
                FriendActivity(
                    id = index.toString(),
                    userId = "user1",
                    type = "STEPS_COMPLETED",
                    description = "Aktivität $index"
                )
            }
        )

        // Act & Assert
        benchmarkRule.measureRepeated {
            repository.getFriendStats("user1").first()
        }
    }

    @Test
    fun benchmarkFriendRequestFlow() = runBlocking {
        // Arrange
        val request = FriendRequest(
            id = "1",
            senderId = "user1",
            receiverId = "friend1",
            status = "PENDING",
            message = "Hallo!"
        )

        // Act & Assert
        benchmarkRule.measureRepeated {
            repository.sendFriendRequest(request).first()
            repository.acceptFriendRequest(request.id).first()
        }
    }

    @Test
    fun benchmarkUIUpdates() {
        // Arrange
        val friends = List(100) { index ->
            Friend(
                id = index.toString(),
                userId = "user1",
                friendId = "friend$index",
                status = "ACCEPTED"
            )
        }

        // Act & Assert
        benchmarkRule.measureRepeated {
            viewModel.updateFriends(friends)
        }
    }

    @Test
    fun benchmarkSearchWithLargeDataset() = runBlocking {
        // Arrange
        val friends = List(10000) { index ->
            Friend(
                id = index.toString(),
                userId = "user1",
                friendId = "test$index",
                status = "PENDING"
            )
        }

        // Act & Assert
        benchmarkRule.measureRepeated {
            repository.searchUsers("test").first()
        }
    }

    @Test
    fun benchmarkConcurrentOperations() = runBlocking {
        // Arrange
        val requests = List(100) { index ->
            FriendRequest(
                id = index.toString(),
                senderId = "user1",
                receiverId = "friend$index",
                status = "PENDING",
                message = "Hallo!"
            )
        }

        // Act & Assert
        benchmarkRule.measureRepeated {
            requests.forEach { request ->
                repository.sendFriendRequest(request).first()
            }
        }
    }
} 