package com.stompcoin.app.data.repository

import android.content.Context
import androidx.room.Room
import androidx.test.core.app.ApplicationProvider
import com.stompcoin.app.data.local.StompCoinDatabase
import com.stompcoin.app.data.model.Friend
import com.stompcoin.app.data.model.FriendActivity
import com.stompcoin.app.data.model.FriendRequest
import com.stompcoin.app.data.model.FriendStats
import com.stompcoin.app.data.remote.StompCoinApi
import dagger.hilt.android.testing.HiltAndroidRule
import dagger.hilt.android.testing.HiltAndroidTest
import kotlinx.coroutines.flow.first
import kotlinx.coroutines.runBlocking
import org.junit.After
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
class FriendRepositoryTest {
    @get:Rule
    var hiltRule = HiltAndroidRule(this)

    @Inject
    lateinit var api: StompCoinApi

    private lateinit var database: StompCoinDatabase
    private lateinit var repository: FriendRepository

    @Before
    fun setup() {
        hiltRule.inject()

        val context = ApplicationProvider.getApplicationContext<Context>()
        database = Room.inMemoryDatabaseBuilder(
            context,
            StompCoinDatabase::class.java
        ).allowMainThreadQueries().build()

        repository = FriendRepositoryImpl(database, api)
    }

    @After
    fun cleanup() {
        database.close()
    }

    @Test
    fun `getFriends should return friends from database`() = runBlocking {
        // Arrange
        val friends = listOf(
            Friend("1", "user1", "friend1", "ACCEPTED"),
            Friend("2", "user1", "friend2", "ACCEPTED")
        )
        friends.forEach { friend ->
            database.friendDao().insertFriend(friend)
        }

        // Act
        val result = repository.getFriends("user1").first()

        // Assert
        assert(result == friends)
    }

    @Test
    fun `getPendingRequests should return pending requests from database`() = runBlocking {
        // Arrange
        val requests = listOf(
            FriendRequest("1", "sender1", "user1", "PENDING", "Hallo!"),
            FriendRequest("2", "sender2", "user1", "PENDING", "Freunde?")
        )
        requests.forEach { request ->
            database.friendDao().insertFriendRequest(request)
        }

        // Act
        val result = repository.getPendingRequests("user1").first()

        // Assert
        assert(result == requests)
    }

    @Test
    fun `getFriendActivities should return activities from database`() = runBlocking {
        // Arrange
        val activities = listOf(
            FriendActivity("1", "user1", "STEPS_COMPLETED", "10000 Schritte erreicht!"),
            FriendActivity("2", "user1", "CHALLENGE_WON", "Challenge gewonnen!")
        )
        activities.forEach { activity ->
            database.friendDao().insertFriendActivity(activity)
        }

        // Act
        val result = repository.getFriendActivities("user1").first()

        // Assert
        assert(result == activities)
    }

    @Test
    fun `getFriendStats should return stats from database`() = runBlocking {
        // Arrange
        val stats = FriendStats(
            totalFriends = 10,
            activeFriends = 5,
            pendingRequests = 2,
            recentActivities = emptyList()
        )
        database.friendDao().insertFriendStats(stats)

        // Act
        val result = repository.getFriendStats("user1").first()

        // Assert
        assert(result == stats)
    }

    @Test
    fun `sendFriendRequest should save request to database`() = runBlocking {
        // Arrange
        val request = FriendRequest("1", "user1", "friend1", "PENDING", "Hallo!")

        // Act
        repository.sendFriendRequest(request).first()

        // Assert
        val savedRequest = database.friendDao().getFriendRequest("1")
        assert(savedRequest == request)
    }

    @Test
    fun `acceptFriendRequest should update request status in database`() = runBlocking {
        // Arrange
        val request = FriendRequest("1", "sender1", "user1", "PENDING", "Hallo!")
        database.friendDao().insertFriendRequest(request)

        // Act
        repository.acceptFriendRequest("1").first()

        // Assert
        val updatedRequest = database.friendDao().getFriendRequest("1")
        assert(updatedRequest?.status == "ACCEPTED")
    }

    @Test
    fun `rejectFriendRequest should update request status in database`() = runBlocking {
        // Arrange
        val request = FriendRequest("1", "sender1", "user1", "PENDING", "Hallo!")
        database.friendDao().insertFriendRequest(request)

        // Act
        repository.rejectFriendRequest("1").first()

        // Assert
        val updatedRequest = database.friendDao().getFriendRequest("1")
        assert(updatedRequest?.status == "REJECTED")
    }

    @Test
    fun `removeFriend should delete friend from database`() = runBlocking {
        // Arrange
        val friend = Friend("1", "user1", "friend1", "ACCEPTED")
        database.friendDao().insertFriend(friend)

        // Act
        repository.removeFriend("1").first()

        // Assert
        val deletedFriend = database.friendDao().getFriend("1")
        assert(deletedFriend == null)
    }

    @Test
    fun `searchUsers should return matching users from database`() = runBlocking {
        // Arrange
        val friends = listOf(
            Friend("1", "user1", "test1", "PENDING"),
            Friend("2", "user1", "test2", "PENDING"),
            Friend("3", "user1", "other", "PENDING")
        )
        friends.forEach { friend ->
            database.friendDao().insertFriend(friend)
        }

        // Act
        val result = repository.searchUsers("test").first()

        // Assert
        assert(result.size == 2)
        assert(result.all { it.friendId.contains("test") })
    }
} 