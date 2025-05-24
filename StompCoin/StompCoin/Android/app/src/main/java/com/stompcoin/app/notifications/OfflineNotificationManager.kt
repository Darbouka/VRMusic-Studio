package com.stompcoin.app.notifications

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

    init {
        createNotificationChannels()
    }

    private fun createNotificationChannels() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            val offlineChannel = NotificationChannel(
                CHANNEL_OFFLINE,
                "Offline-Benachrichtigungen",
                NotificationManager.IMPORTANCE_DEFAULT
            ).apply {
                description = "Benachrichtigungen für Offline-Funktionen"
            }

            notificationManager.createNotificationChannel(offlineChannel)
        }
    }

    fun showOfflineModeNotification() {
        val intent = Intent(context, MainActivity::class.java).apply {
            flags = Intent.FLAG_ACTIVITY_NEW_TASK or Intent.FLAG_ACTIVITY_CLEAR_TASK
        }

        val pendingIntent = PendingIntent.getActivity(
            context,
            0,
            intent,
            PendingIntent.FLAG_IMMUTABLE
        )

        val notification = NotificationCompat.Builder(context, CHANNEL_OFFLINE)
            .setSmallIcon(R.drawable.ic_notification)
            .setContentTitle("Offline-Modus aktiv")
            .setContentText("Die App funktioniert im Offline-Modus. Änderungen werden synchronisiert, sobald Sie wieder online sind.")
            .setPriority(NotificationCompat.PRIORITY_DEFAULT)
            .setContentIntent(pendingIntent)
            .setAutoCancel(true)
            .build()

        notificationManager.notify(NOTIFICATION_OFFLINE_MODE, notification)
    }

    fun showSyncStartedNotification() {
        val notification = NotificationCompat.Builder(context, CHANNEL_OFFLINE)
            .setSmallIcon(R.drawable.ic_sync)
            .setContentTitle("Synchronisierung gestartet")
            .setContentText("Ihre Offline-Änderungen werden synchronisiert...")
            .setPriority(NotificationCompat.PRIORITY_LOW)
            .setOngoing(true)
            .build()

        notificationManager.notify(NOTIFICATION_SYNC_STARTED, notification)
    }

    fun showSyncCompletedNotification() {
        val notification = NotificationCompat.Builder(context, CHANNEL_OFFLINE)
            .setSmallIcon(R.drawable.ic_sync_complete)
            .setContentTitle("Synchronisierung abgeschlossen")
            .setContentText("Alle Offline-Änderungen wurden erfolgreich synchronisiert.")
            .setPriority(NotificationCompat.PRIORITY_DEFAULT)
            .setAutoCancel(true)
            .build()

        notificationManager.notify(NOTIFICATION_SYNC_COMPLETED, notification)
    }

    fun showSyncFailedNotification() {
        val notification = NotificationCompat.Builder(context, CHANNEL_OFFLINE)
            .setSmallIcon(R.drawable.ic_sync_error)
            .setContentTitle("Synchronisierung fehlgeschlagen")
            .setContentText("Die Synchronisierung konnte nicht abgeschlossen werden. Bitte versuchen Sie es später erneut.")
            .setPriority(NotificationCompat.PRIORITY_HIGH)
            .setAutoCancel(true)
            .build()

        notificationManager.notify(NOTIFICATION_SYNC_FAILED, notification)
    }

    fun showPendingChangesNotification(pendingCount: Int) {
        val notification = NotificationCompat.Builder(context, CHANNEL_OFFLINE)
            .setSmallIcon(R.drawable.ic_pending_changes)
            .setContentTitle("Ausstehende Änderungen")
            .setContentText("$pendingCount Änderungen warten auf Synchronisierung")
            .setPriority(NotificationCompat.PRIORITY_DEFAULT)
            .setAutoCancel(true)
            .build()

        notificationManager.notify(NOTIFICATION_PENDING_CHANGES, notification)
    }

    companion object {
        private const val CHANNEL_OFFLINE = "offline_channel"
        private const val NOTIFICATION_OFFLINE_MODE = 1
        private const val NOTIFICATION_SYNC_STARTED = 2
        private const val NOTIFICATION_SYNC_COMPLETED = 3
        private const val NOTIFICATION_SYNC_FAILED = 4
        private const val NOTIFICATION_PENDING_CHANGES = 5
    }
} 