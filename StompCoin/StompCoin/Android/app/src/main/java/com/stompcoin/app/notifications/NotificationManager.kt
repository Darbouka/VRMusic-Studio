package com.stompcoin.app.notifications

import android.app.NotificationChannel
import android.app.NotificationManager
import android.app.PendingIntent
import android.content.Context
import android.content.Intent
import android.os.Build
import androidx.core.app.NotificationCompat
import com.google.firebase.messaging.FirebaseMessaging
import com.stompcoin.app.R
import com.stompcoin.app.data.model.Event
import com.stompcoin.app.ui.MainActivity
import dagger.hilt.android.qualifiers.ApplicationContext
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.tasks.await
import javax.inject.Inject
import javax.inject.Singleton

@Singleton
class NotificationManager @Inject constructor(
    @ApplicationContext private val context: Context
) {
    private val notificationManager = context.getSystemService(Context.NOTIFICATION_SERVICE) as NotificationManager
    private val _notificationToken = MutableStateFlow<String?>(null)
    val notificationToken: StateFlow<String?> = _notificationToken

    init {
        createNotificationChannels()
        getFCMToken()
    }

    private fun createNotificationChannels() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val channels = listOf(
                NotificationChannel(
                    CHANNEL_EVENTS,
                    "Events",
                    NotificationManager.IMPORTANCE_HIGH
                ),
                NotificationChannel(
                    CHANNEL_CHALLENGES,
                    "Challenges",
                    NotificationManager.IMPORTANCE_DEFAULT
                ),
                NotificationChannel(
                    CHANNEL_BOOSTS,
                    "Boosts",
                    NotificationManager.IMPORTANCE_DEFAULT
                ),
                NotificationChannel(
                    CHANNEL_REWARDS,
                    "Rewards",
                    NotificationManager.IMPORTANCE_HIGH
                )
            )
            notificationManager.createNotificationChannels(channels)
        }
    }

    private suspend fun getFCMToken() {
        try {
            val token = FirebaseMessaging.getInstance().token.await()
            _notificationToken.value = token
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    fun showEventNotification(event: Event) {
        val intent = Intent(context, MainActivity::class.java).apply {
            flags = Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_ACTIVITY_CLEAR_TASK
            putExtra("event_id", event.id)
        }

        val pendingIntent = PendingIntent.getActivity(
            context,
            0,
            intent,
            PendingIntent.FLAG_IMMUTABLE
        )

        val notification = NotificationCompat.Builder(context, CHANNEL_EVENTS)
            .setSmallIcon(R.drawable.ic_notification)
            .setContentTitle("Neues Event in deiner Nähe!")
            .setContentText(event.name)
            .setPriority(NotificationCompat.PRIORITY_HIGH)
            .setAutoCancel(true)
            .setContentIntent(pendingIntent)
            .build()

        notificationManager.notify(EVENT_NOTIFICATION_ID, notification)
    }

    fun showChallengeNotification(challengeName: String, reward: Int) {
        val notification = NotificationCompat.Builder(context, CHANNEL_CHALLENGES)
            .setSmallIcon(R.drawable.ic_notification)
            .setContentTitle("Neue Challenge verfügbar!")
            .setContentText("$challengeName - $reward StompCoins zu gewinnen")
            .setPriority(NotificationCompat.PRIORITY_DEFAULT)
            .setAutoCancel(true)
            .build()

        notificationManager.notify(CHALLENGE_NOTIFICATION_ID, notification)
    }

    fun showBoostNotification() {
        val notification = NotificationCompat.Builder(context, CHANNEL_BOOSTS)
            .setSmallIcon(R.drawable.ic_notification)
            .setContentTitle("Boost verfügbar!")
            .setContentText("Du hast einen neuen Boost zur Verfügung")
            .setPriority(NotificationCompat.PRIORITY_DEFAULT)
            .setAutoCancel(true)
            .build()

        notificationManager.notify(BOOST_NOTIFICATION_ID, notification)
    }

    fun showRewardNotification(amount: Int, type: String) {
        val notification = NotificationCompat.Builder(context, CHANNEL_REWARDS)
            .setSmallIcon(R.drawable.ic_notification)
            .setContentTitle("Belohnung erhalten!")
            .setContentText("Du hast $amount $type gewonnen")
            .setPriority(NotificationCompat.PRIORITY_HIGH)
            .setAutoCancel(true)
            .build()

        notificationManager.notify(REWARD_NOTIFICATION_ID, notification)
    }

    companion object {
        private const val CHANNEL_EVENTS = "events"
        private const val CHANNEL_CHALLENGES = "challenges"
        private const val CHANNEL_BOOSTS = "boosts"
        private const val CHANNEL_REWARDS = "rewards"

        private const val EVENT_NOTIFICATION_ID = 1
        private const val CHALLENGE_NOTIFICATION_ID = 2
        private const val BOOST_NOTIFICATION_ID = 3
        private const val REWARD_NOTIFICATION_ID = 4
    }
} 