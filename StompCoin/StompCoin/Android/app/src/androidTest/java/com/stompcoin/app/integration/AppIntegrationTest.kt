package com.stompcoin.app.integration

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
class AppIntegrationTest {
    @get:Rule
    var hiltRule = HiltAndroidRule(this)

    @Inject
    lateinit var appManager: AppManager

    @Inject
    lateinit var stepTracker: StepTracker

    @Inject
    lateinit var coinManager: CoinManager

    @Inject
    lateinit var userManager: UserManager

    private lateinit var context: Context

    @Before
    fun setup() {
        hiltRule.inject()
        context = ApplicationProvider.getApplicationContext()
    }

    @Test
    fun `should track steps and convert to coins`() = runBlocking {
        // Arrange
        val userId = "testuser"
        val steps = 10000

        // Act
        stepTracker.trackSteps(userId, steps)
        val coins = coinManager.calculateCoins(steps)

        // Assert
        assert(coins > 0)
        assert(stepTracker.getDailySteps(userId) == steps)
    }

    @Test
    fun `should handle daily step goals`() = runBlocking {
        // Arrange
        val userId = "testuser"
        val dailyGoal = 8000

        // Act
        stepTracker.setDailyGoal(userId, dailyGoal)
        stepTracker.trackSteps(userId, 10000)

        // Assert
        assert(stepTracker.isDailyGoalAchieved(userId))
        assert(stepTracker.getDailyProgress(userId) >= 1.0)
    }

    @Test
    fun `should handle coin transactions`() = runBlocking {
        // Arrange
        val userId = "testuser"
        val initialCoins = 1000
        val transactionAmount = 100

        // Act
        coinManager.addCoins(userId, initialCoins)
        val transaction = coinManager.createTransaction(userId, transactionAmount)

        // Assert
        assert(transaction.isSuccess)
        assert(coinManager.getBalance(userId) == initialCoins + transactionAmount)
    }

    @Test
    fun `should handle user achievements`() = runBlocking {
        // Arrange
        val userId = "testuser"
        val achievement = Achievement(
            type = "steps_milestone",
            threshold = 100000
        )

        // Act
        stepTracker.trackSteps(userId, 100000)
        val result = userManager.checkAchievement(userId, achievement)

        // Assert
        assert(result.isUnlocked)
        assert(result.reward > 0)
    }

    @Test
    fun `should handle user level progression`() = runBlocking {
        // Arrange
        val userId = "testuser"
        val initialLevel = 1

        // Act
        userManager.setLevel(userId, initialLevel)
        stepTracker.trackSteps(userId, 50000)
        val newLevel = userManager.calculateLevel(userId)

        // Assert
        assert(newLevel > initialLevel)
        assert(userManager.getLevelProgress(userId) > 0)
    }

    @Test
    fun `should handle coin rewards for streaks`() = runBlocking {
        // Arrange
        val userId = "testuser"
        val streakDays = 7

        // Act
        repeat(streakDays) {
            stepTracker.trackSteps(userId, 10000)
        }
        val streakReward = coinManager.calculateStreakReward(streakDays)

        // Assert
        assert(streakReward > 0)
        assert(stepTracker.getCurrentStreak(userId) == streakDays)
    }

    @Test
    fun `should handle coin rewards for challenges`() = runBlocking {
        // Arrange
        val userId = "testuser"
        val challenge = Challenge(
            type = "steps",
            goal = 50000,
            duration = 7
        )

        // Act
        val challengeResult = userManager.startChallenge(userId, challenge)
        stepTracker.trackSteps(userId, 50000)
        val reward = coinManager.calculateChallengeReward(challenge)

        // Assert
        assert(challengeResult.isSuccess)
        assert(reward > 0)
        assert(userManager.isChallengeCompleted(userId, challenge))
    }

    @Test
    fun `should handle coin rewards for social interactions`() = runBlocking {
        // Arrange
        val userId = "testuser"
        val friendId = "friend1"

        // Act
        userManager.addFriend(userId, friendId)
        val socialReward = coinManager.calculateSocialReward()

        // Assert
        assert(socialReward > 0)
        assert(userManager.getFriends(userId).contains(friendId))
    }

    @Test
    fun `should handle coin rewards for daily login`() = runBlocking {
        // Arrange
        val userId = "testuser"

        // Act
        val loginReward = coinManager.calculateLoginReward()

        // Assert
        assert(loginReward > 0)
        assert(userManager.getLoginStreak(userId) > 0)
    }

    @Test
    fun `should handle coin rewards for special events`() = runBlocking {
        // Arrange
        val userId = "testuser"
        val event = SpecialEvent(
            type = "holiday",
            multiplier = 2.0
        )

        // Act
        val eventReward = coinManager.calculateEventReward(event)

        // Assert
        assert(eventReward > 0)
        assert(coinManager.isEventActive(event))
    }

    @Test
    fun `should handle coin rewards for achievements`() = runBlocking {
        // Arrange
        val userId = "testuser"
        val achievement = Achievement(
            type = "steps_milestone",
            threshold = 100000
        )

        // Act
        stepTracker.trackSteps(userId, 100000)
        val achievementReward = coinManager.calculateAchievementReward(achievement)

        // Assert
        assert(achievementReward > 0)
        assert(userManager.hasAchievement(userId, achievement))
    }

    @Test
    fun `should handle coin rewards for level up`() = runBlocking {
        // Arrange
        val userId = "testuser"
        val currentLevel = 5

        // Act
        userManager.setLevel(userId, currentLevel)
        stepTracker.trackSteps(userId, 100000)
        val levelUpReward = coinManager.calculateLevelUpReward(currentLevel + 1)

        // Assert
        assert(levelUpReward > 0)
        assert(userManager.getLevel(userId) > currentLevel)
    }

    @Test
    fun `should handle coin rewards for referrals`() = runBlocking {
        // Arrange
        val userId = "testuser"
        val referredUser = "newuser"

        // Act
        val referralReward = coinManager.calculateReferralReward()

        // Assert
        assert(referralReward > 0)
        assert(userManager.getReferrals(userId).contains(referredUser))
    }
} 