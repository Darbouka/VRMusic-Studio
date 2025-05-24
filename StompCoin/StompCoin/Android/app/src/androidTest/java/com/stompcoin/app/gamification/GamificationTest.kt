package com.stompcoin.app.gamification

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
class GamificationTest {
    @get:Rule
    var hiltRule = HiltAndroidRule(this)

    @Inject
    lateinit var gamificationManager: GamificationManager

    private lateinit var context: Context

    @Before
    fun setup() {
        hiltRule.inject()
        context = ApplicationProvider.getApplicationContext()
    }

    @Test
    fun `should handle level progression`() = runBlocking {
        // Arrange
        val userProgress = UserProgress(
            userId = "user1",
            currentLevel = 5,
            experience = 4500
        )

        // Act
        val result = gamificationManager.updateProgress(userProgress)

        // Assert
        assert(result.isSuccess)
        assert(result.newLevel == 6)
    }

    @Test
    fun `should handle achievement unlocking`() = runBlocking {
        // Arrange
        val achievement = Achievement(
            userId = "user1",
            type = "steps_milestone",
            data = mapOf("steps" to 100000)
        )

        // Act
        val result = gamificationManager.unlockAchievement(achievement)

        // Assert
        assert(result.isSuccess)
        assert(result.achievementId != null)
    }

    @Test
    fun `should handle reward distribution`() = runBlocking {
        // Arrange
        val reward = Reward(
            userId = "user1",
            type = "coins",
            amount = 100,
            reason = "level_up"
        )

        // Act
        val result = gamificationManager.distributeReward(reward)

        // Assert
        assert(result.isSuccess)
        assert(result.transactionId != null)
    }

    @Test
    fun `should handle challenge creation`() = runBlocking {
        // Arrange
        val challenge = Challenge(
            creatorId = "user1",
            type = "steps",
            goal = 10000,
            duration = 7
        )

        // Act
        val result = gamificationManager.createChallenge(challenge)

        // Assert
        assert(result.isSuccess)
        assert(result.challengeId != null)
    }

    @Test
    fun `should handle challenge completion`() = runBlocking {
        // Arrange
        val challengeId = "challenge_123"
        val completion = ChallengeCompletion(
            userId = "user1",
            challengeId = challengeId,
            progress = 10000
        )

        // Act
        val result = gamificationManager.completeChallenge(completion)

        // Assert
        assert(result.isSuccess)
        assert(result.rewards.isNotEmpty())
    }

    @Test
    fun `should handle daily quests`() = runBlocking {
        // Arrange
        val quest = DailyQuest(
            userId = "user1",
            type = "steps",
            goal = 5000,
            reward = 50
        )

        // Act
        val result = gamificationManager.assignDailyQuest(quest)

        // Assert
        assert(result.isSuccess)
        assert(result.questId != null)
    }

    @Test
    fun `should handle streak tracking`() = runBlocking {
        // Arrange
        val streak = Streak(
            userId = "user1",
            type = "daily_login",
            currentStreak = 5
        )

        // Act
        val result = gamificationManager.updateStreak(streak)

        // Assert
        assert(result.isSuccess)
        assert(result.newStreak == 6)
    }

    @Test
    fun `should handle leaderboard updates`() = runBlocking {
        // Arrange
        val score = LeaderboardScore(
            userId = "user1",
            category = "weekly_steps",
            score = 50000
        )

        // Act
        val result = gamificationManager.updateLeaderboard(score)

        // Assert
        assert(result.isSuccess)
        assert(result.rank != null)
    }

    @Test
    fun `should handle badge awarding`() = runBlocking {
        // Arrange
        val badge = Badge(
            userId = "user1",
            type = "early_adopter",
            level = "gold"
        )

        // Act
        val result = gamificationManager.awardBadge(badge)

        // Assert
        assert(result.isSuccess)
        assert(result.badgeId != null)
    }

    @Test
    fun `should handle experience calculation`() = runBlocking {
        // Arrange
        val activity = UserActivity(
            userId = "user1",
            type = "steps",
            value = 10000
        )

        // Act
        val result = gamificationManager.calculateExperience(activity)

        // Assert
        assert(result.experience > 0)
    }

    @Test
    fun `should handle level requirements`() = runBlocking {
        // Arrange
        val level = 5

        // Act
        val requirements = gamificationManager.getLevelRequirements(level)

        // Assert
        assert(requirements.experienceRequired > 0)
        assert(requirements.rewards.isNotEmpty())
    }

    @Test
    fun `should handle achievement progress`() = runBlocking {
        // Arrange
        val achievement = Achievement(
            userId = "user1",
            type = "steps_milestone",
            data = mapOf("steps" to 50000)
        )

        // Act
        val progress = gamificationManager.getAchievementProgress(achievement)

        // Assert
        assert(progress.currentValue >= 0)
        assert(progress.targetValue > 0)
    }

    @Test
    fun `should handle reward redemption`() = runBlocking {
        // Arrange
        val redemption = RewardRedemption(
            userId = "user1",
            rewardType = "premium_feature",
            cost = 1000
        )

        // Act
        val result = gamificationManager.redeemReward(redemption)

        // Assert
        assert(result.isSuccess)
        assert(result.redemptionId != null)
    }

    @Test
    fun `should handle game balance`() = runBlocking {
        // Arrange
        val userId = "user1"

        // Act
        val balance = gamificationManager.getGameBalance(userId)

        // Assert
        assert(balance.coins >= 0)
        assert(balance.experience >= 0)
    }
} 