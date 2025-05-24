package com.stompcoin.app.monetization

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
class MonetizationTest {
    @get:Rule
    var hiltRule = HiltAndroidRule(this)

    @Inject
    lateinit var monetizationManager: MonetizationManager

    private lateinit var context: Context

    @Before
    fun setup() {
        hiltRule.inject()
        context = ApplicationProvider.getApplicationContext()
    }

    @Test
    fun `should handle in-app purchases`() = runBlocking {
        // Arrange
        val purchase = InAppPurchase(
            productId = "premium_monthly",
            price = 4.99,
            currency = "EUR"
        )

        // Act
        val result = monetizationManager.processPurchase(purchase)

        // Assert
        assert(result.isSuccess)
        assert(result.receipt != null)
    }

    @Test
    fun `should handle subscription management`() = runBlocking {
        // Arrange
        val subscription = Subscription(
            planId = "premium_yearly",
            price = 49.99,
            currency = "EUR",
            period = "yearly"
        )

        // Act
        val result = monetizationManager.manageSubscription(subscription)

        // Assert
        assert(result.isSuccess)
        assert(result.expiryDate != null)
    }

    @Test
    fun `should handle ad display`() = runBlocking {
        // Arrange
        val adRequest = AdRequest(
            type = "rewarded",
            placement = "main_screen"
        )

        // Act
        val result = monetizationManager.displayAd(adRequest)

        // Assert
        assert(result.isSuccess)
        assert(result.adId != null)
    }

    @Test
    fun `should handle reward distribution`() = runBlocking {
        // Arrange
        val reward = Reward(
            type = "coins",
            amount = 100,
            reason = "ad_watch"
        )

        // Act
        val result = monetizationManager.distributeReward(reward)

        // Assert
        assert(result.isSuccess)
        assert(result.transactionId != null)
    }

    @Test
    fun `should handle purchase validation`() = runBlocking {
        // Arrange
        val purchase = InAppPurchase(
            productId = "premium_monthly",
            price = 4.99,
            currency = "EUR"
        )

        // Act
        val result = monetizationManager.validatePurchase(purchase)

        // Assert
        assert(result.isValid)
        assert(result.errors.isEmpty())
    }

    @Test
    fun `should handle subscription renewal`() = runBlocking {
        // Arrange
        val subscription = Subscription(
            planId = "premium_yearly",
            price = 49.99,
            currency = "EUR",
            period = "yearly"
        )

        // Act
        val result = monetizationManager.renewSubscription(subscription)

        // Assert
        assert(result.isSuccess)
        assert(result.newExpiryDate != null)
    }

    @Test
    fun `should handle ad revenue tracking`() = runBlocking {
        // Arrange
        val adImpression = AdImpression(
            adId = "ad_123",
            type = "rewarded",
            revenue = 0.01
        )

        // Act
        val result = monetizationManager.trackAdRevenue(adImpression)

        // Assert
        assert(result.isSuccess)
        assert(result.revenue > 0)
    }

    @Test
    fun `should handle purchase refund`() = runBlocking {
        // Arrange
        val purchase = InAppPurchase(
            productId = "premium_monthly",
            price = 4.99,
            currency = "EUR"
        )

        // Act
        val result = monetizationManager.processRefund(purchase)

        // Assert
        assert(result.isSuccess)
        assert(result.refundId != null)
    }

    @Test
    fun `should handle subscription cancellation`() = runBlocking {
        // Arrange
        val subscription = Subscription(
            planId = "premium_yearly",
            price = 49.99,
            currency = "EUR",
            period = "yearly"
        )

        // Act
        val result = monetizationManager.cancelSubscription(subscription)

        // Assert
        assert(result.isSuccess)
        assert(result.cancellationDate != null)
    }

    @Test
    fun `should handle revenue analytics`() = runBlocking {
        // Arrange
        val timeRange = TimeRange(
            start = System.currentTimeMillis() - 86400000, // 24 hours ago
            end = System.currentTimeMillis()
        )

        // Act
        val analytics = monetizationManager.getRevenueAnalytics(timeRange)

        // Assert
        assert(analytics.totalRevenue > 0)
        assert(analytics.breakdown.isNotEmpty())
    }

    @Test
    fun `should handle purchase history`() = runBlocking {
        // Arrange
        val userId = "user_123"

        // Act
        val history = monetizationManager.getPurchaseHistory(userId)

        // Assert
        assert(history.purchases.isNotEmpty())
        assert(history.totalSpent > 0)
    }

    @Test
    fun `should handle subscription status`() = runBlocking {
        // Arrange
        val userId = "user_123"

        // Act
        val status = monetizationManager.getSubscriptionStatus(userId)

        // Assert
        assert(status.isActive)
        assert(status.expiryDate != null)
    }
} 