package com.stompcoin.app.social

import android.content.Context
import androidx.test.core.app.ApplicationProvider
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
class SocialFeaturesTest {
    @get:Rule
    var hiltRule = HiltAndroidRule(this)

    @Inject
    lateinit var socialManager: SocialManager

    private lateinit var context: Context

    @Before
    fun setup() {
        hiltRule.inject()
        context = ApplicationProvider.getApplicationContext()
    }

    @Test
    fun `should handle friend requests`() = runBlocking {
        // Arrange
        val request = FriendRequest(
            senderId = "user1",
            receiverId = "user2",
            message = "Lass uns Freunde werden!"
        )

        // Act
        val result = socialManager.sendFriendRequest(request)

        // Assert
        assert(result.isSuccess)
        assert(result.requestId != null)
    }

    @Test
    fun `should handle friend acceptance`() = runBlocking {
        // Arrange
        val requestId = "request_123"

        // Act
        val result = socialManager.acceptFriendRequest(requestId)

        // Assert
        assert(result.isSuccess)
        assert(result.friendshipId != null)
    }

    @Test
    fun `should handle friend rejection`() = runBlocking {
        // Arrange
        val requestId = "request_123"

        // Act
        val result = socialManager.rejectFriendRequest(requestId)

        // Assert
        assert(result.isSuccess)
    }

    @Test
    fun `should handle friend removal`() = runBlocking {
        // Arrange
        val friendshipId = "friendship_123"

        // Act
        val result = socialManager.removeFriend(friendshipId)

        // Assert
        assert(result.isSuccess)
    }

    @Test
    fun `should handle friend activity sharing`() = runBlocking {
        // Arrange
        val activity = FriendActivity(
            userId = "user1",
            type = "steps_completed",
            data = mapOf("steps" to 10000)
        )

        // Act
        val result = socialManager.shareActivity(activity)

        // Assert
        assert(result.isSuccess)
        assert(result.activityId != null)
    }

    @Test
    fun `should handle friend challenges`() = runBlocking {
        // Arrange
        val challenge = Challenge(
            creatorId = "user1",
            type = "steps",
            goal = 10000,
            duration = 7
        )

        // Act
        val result = socialManager.createChallenge(challenge)

        // Assert
        assert(result.isSuccess)
        assert(result.challengeId != null)
    }

    @Test
    fun `should handle friend leaderboards`() = runBlocking {
        // Arrange
        val leaderboardType = "weekly_steps"

        // Act
        val result = socialManager.getLeaderboard(leaderboardType)

        // Assert
        assert(result.isSuccess)
        assert(result.entries.isNotEmpty())
    }

    @Test
    fun `should handle friend notifications`() = runBlocking {
        // Arrange
        val notification = FriendNotification(
            userId = "user1",
            type = "friend_request",
            data = mapOf("senderId" to "user2")
        )

        // Act
        val result = socialManager.sendNotification(notification)

        // Assert
        assert(result.isSuccess)
        assert(result.notificationId != null)
    }

    @Test
    fun `should handle friend messaging`() = runBlocking {
        // Arrange
        val message = FriendMessage(
            senderId = "user1",
            receiverId = "user2",
            content = "Hallo! Wie geht's?"
        )

        // Act
        val result = socialManager.sendMessage(message)

        // Assert
        assert(result.isSuccess)
        assert(result.messageId != null)
    }

    @Test
    fun `should handle friend achievements`() = runBlocking {
        // Arrange
        val achievement = Achievement(
            userId = "user1",
            type = "steps_milestone",
            data = mapOf("steps" to 100000)
        )

        // Act
        val result = socialManager.unlockAchievement(achievement)

        // Assert
        assert(result.isSuccess)
        assert(result.achievementId != null)
    }

    @Test
    fun `should handle friend groups`() = runBlocking {
        // Arrange
        val group = FriendGroup(
            name = "Fitness-Freunde",
            creatorId = "user1",
            memberIds = listOf("user2", "user3")
        )

        // Act
        val result = socialManager.createGroup(group)

        // Assert
        assert(result.isSuccess)
        assert(result.groupId != null)
    }

    @Test
    fun `should handle friend privacy settings`() = runBlocking {
        // Arrange
        val settings = PrivacySettings(
            userId = "user1",
            showSteps = true,
            showChallenges = true,
            showAchievements = false
        )

        // Act
        val result = socialManager.updatePrivacySettings(settings)

        // Assert
        assert(result.isSuccess)
    }

    @Test
    fun `should handle friend blocking`() = runBlocking {
        // Arrange
        val block = UserBlock(
            blockerId = "user1",
            blockedId = "user2"
        )

        // Act
        val result = socialManager.blockUser(block)

        // Assert
        assert(result.isSuccess)
    }

    @Test
    fun `should handle friend reporting`() = runBlocking {
        // Arrange
        val report = UserReport(
            reporterId = "user1",
            reportedId = "user2",
            reason = "Spam",
            details = "Unangemessene Nachrichten"
        )

        // Act
        val result = socialManager.reportUser(report)

        // Assert
        assert(result.isSuccess)
        assert(result.reportId != null)
    }
} 