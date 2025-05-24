package com.stompcoin.app.offline

import android.content.Context
import androidx.room.Room
import androidx.test.core.app.ApplicationProvider
import com.stompcoin.app.data.local.StompCoinDatabase
import com.stompcoin.app.data.model.Friend
import com.stompcoin.app.data.model.FriendActivity
import com.stompcoin.app.data.model.FriendRequest
import com.stompcoin.app.data.repository.FriendRepository
import com.stompcoin.app.data.sync.SyncManager
import com.stompcoin.app.network.NetworkOptimizer
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
class OfflineFriendsTest {
    @get:Rule
    var hiltRule = HiltAndroidRule(this)

    @Inject
    lateinit var networkOptimizer: NetworkOptimizer

    @Inject
    lateinit var syncManager: SyncManager

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

        repository = FriendRepositoryImpl(database, mockk())
    }

    @After
    fun cleanup() {
        database.close()
    }

    @Test
    fun `should work offline when network is unavailable`() = runBlocking {
        // Arrange
        val friend = Friend("1", "user1", "friend1", "ACCEPTED")
        database.friendDao().insertFriend(friend)

        // Simuliere Offline-Modus
        networkOptimizer.setNetworkAvailable(false)

        // Act
        val result = repository.getFriends("user1").first()

        // Assert
        assert(result.contains(friend))
    }

    @Test
    fun `should queue friend request when offline`() = runBlocking {
        // Arrange
        val request = FriendRequest("1", "user1", "friend1", "PENDING", "Hallo!")
        networkOptimizer.setNetworkAvailable(false)

        // Act
        repository.sendFriendRequest(request).first()

        // Assert
        val savedRequest = database.friendDao().getFriendRequest("1")
        assert(savedRequest == request)
        assert(savedRequest?.status == "PENDING")
    }

    @Test
    fun `should sync pending requests when back online`() = runBlocking {
        // Arrange
        val request = FriendRequest("1", "user1", "friend1", "PENDING", "Hallo!")
        database.friendDao().insertFriendRequest(request)

        // Act
        networkOptimizer.setNetworkAvailable(true)
        syncManager.syncNow()

        // Assert
        val syncedRequest = database.friendDao().getFriendRequest("1")
        assert(syncedRequest?.status == "SYNCED")
    }

    @Test
    fun `should cache friend activities for offline access`() = runBlocking {
        // Arrange
        val activities = listOf(
            FriendActivity("1", "user1", "STEPS_COMPLETED", "10000 Schritte erreicht!"),
            FriendActivity("2", "user1", "CHALLENGE_WON", "Challenge gewonnen!")
        )
        activities.forEach { activity ->
            database.friendDao().insertFriendActivity(activity)
        }

        // Simuliere Offline-Modus
        networkOptimizer.setNetworkAvailable(false)

        // Act
        val result = repository.getFriendActivities("user1").first()

        // Assert
        assert(result == activities)
    }

    @Test
    fun `should handle offline friend removal`() = runBlocking {
        // Arrange
        val friend = Friend("1", "user1", "friend1", "ACCEPTED")
        database.friendDao().insertFriend(friend)
        networkOptimizer.setNetworkAvailable(false)

        // Act
        repository.removeFriend("1").first()

        // Assert
        val removedFriend = database.friendDao().getFriend("1")
        assert(removedFriend == null)
    }

    @Test
    fun `should sync friend removal when back online`() = runBlocking {
        // Arrange
        val friend = Friend("1", "user1", "friend1", "ACCEPTED")
        database.friendDao().insertFriend(friend)
        networkOptimizer.setNetworkAvailable(false)
        repository.removeFriend("1").first()

        // Act
        networkOptimizer.setNetworkAvailable(true)
        syncManager.syncNow()

        // Assert
        val syncedFriend = database.friendDao().getFriend("1")
        assert(syncedFriend == null)
    }

    @Test
    fun `should handle offline friend request acceptance`() = runBlocking {
        // Arrange
        val request = FriendRequest("1", "sender1", "user1", "PENDING", "Hallo!")
        database.friendDao().insertFriendRequest(request)
        networkOptimizer.setNetworkAvailable(false)

        // Act
        repository.acceptFriendRequest("1").first()

        // Assert
        val acceptedRequest = database.friendDao().getFriendRequest("1")
        assert(acceptedRequest?.status == "ACCEPTED")
    }

    @Test
    fun `should sync accepted requests when back online`() = runBlocking {
        // Arrange
        val request = FriendRequest("1", "sender1", "user1", "PENDING", "Hallo!")
        database.friendDao().insertFriendRequest(request)
        networkOptimizer.setNetworkAvailable(false)
        repository.acceptFriendRequest("1").first()

        // Act
        networkOptimizer.setNetworkAvailable(true)
        syncManager.syncNow()

        // Assert
        val syncedRequest = database.friendDao().getFriendRequest("1")
        assert(syncedRequest?.status == "SYNCED")
    }
} 