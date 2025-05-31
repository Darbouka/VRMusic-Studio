package com.stompcoin.app.data.managers

import android.app.NotificationChannel
import android.app.NotificationManager
import android.app.PendingIntent
import android.content.Context
import android.content.Intent
import android.os.Build
import androidx.core.app.NotificationCompat
import com.stompcoin.app.R
import com.stompcoin.app.ui.MainActivity
import dagger.hilt.android.qualifiers.ApplicationContext
import javax.inject.Inject
import javax.inject.Singleton

@Singleton
class OfflineNotificationManager @Inject constructor(
    @ApplicationContext private val context: Context
) {
    private val notificationManager = context.getSystemService(Context.NOTIFICATION_SERVICE) as NotificationManager
    private val channelId = "offline_notifications"
    private val notificationId = 1

    init {
        createNotificationChannel()
    }

    private fun createNotificationChannel() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val name = "Offline-Benachrichtigungen"
            val descriptionText = "Benachrichtigungen für Offline-Aktivitäten"
            val importance = NotificationManager.IMPORTANCE_DEFAULT
            val channel = NotificationChannel(channelId, name, importance).apply {
                description = descriptionText
            }
            notificationManager.createNotificationChannel(channel)
        }
    }

    fun showOfflineNotification(title: String, message: String) {
        val intent = Intent(context, MainActivity::class.java).apply {
            flags = Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_ACTIVITY_CLEAR_TASK
        }
        val pendingIntent = PendingIntent.getActivity(
            context,
            0,
            intent,
            PendingIntent.FLAG_IMMUTABLE
        )

        val notification = NotificationCompat.Builder(context, channelId)
            .setSmallIcon(R.drawable.ic_notification)
            .setContentTitle(title)
            .setContentText(message)
            .setPriority(NotificationCompat.PRIORITY_DEFAULT)
            .setContentIntent(pendingIntent)
            .setAutoCancel(true)
            .build()

        notificationManager.notify(notificationId, notification)
    }

    fun showOfflineSyncNotification() {
        showOfflineNotification(
            "Offline-Daten synchronisiert",
            "Deine Offline-Aktivitäten wurden erfolgreich synchronisiert."
        )
    }

    fun showOfflineDataSavedNotification() {
        showOfflineNotification(
            "Offline-Daten gespeichert",
            "Deine Aktivitäten wurden lokal gespeichert und werden synchronisiert, sobald du wieder online bist."
        )
    }

    fun showOfflineErrorNotification() {
        showOfflineNotification(
            "Offline-Fehler",
            "Es gab ein Problem beim Speichern deiner Offline-Daten. Bitte versuche es später erneut."
        )
    }

    fun cancelAllNotifications() {
        notificationManager.cancelAll()
    }
} 