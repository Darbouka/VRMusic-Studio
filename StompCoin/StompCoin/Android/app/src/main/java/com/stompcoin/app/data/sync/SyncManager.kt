package com.stompcoin.app.data.sync

import android.content.Context
import androidx.work.*
import com.stompcoin.app.data.local.*
import com.stompcoin.app.data.remote.StompCoinApi
import dagger.hilt.android.qualifiers.ApplicationContext
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.map
import java.util.concurrent.TimeUnit
import javax.inject.Inject
import javax.inject.Singleton

@Singleton
class SyncManager @Inject constructor(
    @ApplicationContext private val context: Context,
    private val database: StompCoinDatabase,
    private val api: StompCoinApi
) {
    private val workManager = WorkManager.getInstance(context)

    init {
        setupPeriodicSync()
    }

    private fun setupPeriodicSync() {
        val syncRequest = PeriodicWorkRequestBuilder<SyncWorker>(
            15, TimeUnit.MINUTES,
            5, TimeUnit.MINUTES
        )
            .setConstraints(
                Constraints.Builder()
                    .setRequiredNetworkType(NetworkType.CONNECTED)
                    .build()
            )
            .build()

        workManager.enqueueUniquePeriodicWork(
            "periodic_sync",
            ExistingPeriodicWorkPolicy.KEEP,
            syncRequest
        )
    }

    fun syncNow() {
        val syncRequest = OneTimeWorkRequestBuilder<SyncWorker>()
            .setConstraints(
                Constraints.Builder()
                    .setRequiredNetworkType(NetworkType.CONNECTED)
                    .build()
            )
            .build()

        workManager.enqueue(syncRequest)
    }

    fun getSyncStatus(): Flow<SyncStatus> {
        return workManager.getWorkInfosForUniqueWorkLiveData("periodic_sync")
            .asFlow()
            .map { workInfos ->
                when {
                    workInfos.any { it.state == WorkInfo.State.RUNNING } ->
                        SyncStatus.SYNCING
                    workInfos.any { it.state == WorkInfo.State.FAILED } ->
                        SyncStatus.FAILED
                    workInfos.any { it.state == WorkInfo.State.SUCCEEDED } ->
                        SyncStatus.SYNCED
                    else -> SyncStatus.IDLE
                }
            }
    }

    suspend fun syncUsers() {
        try {
            val remoteUsers = api.getUsers()
            remoteUsers.forEach { user ->
                database.userDao().insertUser(user)
            }
        } catch (e: Exception) {
            // Handle error
        }
    }

    suspend fun syncFriends() {
        try {
            val remoteFriends = api.getFriends()
            remoteFriends.forEach { friend ->
                database.friendDao().insertFriend(friend)
            }
        } catch (e: Exception) {
            // Handle error
        }
    }

    suspend fun syncEvents() {
        try {
            val remoteEvents = api.getEvents()
            remoteEvents.forEach { event ->
                database.eventDao().insertEvent(event)
            }
        } catch (e: Exception) {
            // Handle error
        }
    }

    suspend fun syncChallenges() {
        try {
            val remoteChallenges = api.getChallenges()
            remoteChallenges.forEach { challenge ->
                database.challengeDao().insertChallenge(challenge)
            }
        } catch (e: Exception) {
            // Handle error
        }
    }

    suspend fun syncAchievements() {
        try {
            val remoteAchievements = api.getAchievements()
            remoteAchievements.forEach { achievement ->
                database.achievementDao().insertAchievement(achievement)
            }
        } catch (e: Exception) {
            // Handle error
        }
    }

    suspend fun syncTransactions() {
        try {
            val remoteTransactions = api.getTransactions()
            remoteTransactions.forEach { transaction ->
                database.transactionDao().insertTransaction(transaction)
            }
        } catch (e: Exception) {
            // Handle error
        }
    }

    suspend fun syncStepCounts() {
        try {
            val remoteStepCounts = api.getStepCounts()
            remoteStepCounts.forEach { stepCount ->
                database.stepCountDao().insertStepCount(stepCount)
            }
        } catch (e: Exception) {
            // Handle error
        }
    }
}

enum class SyncStatus {
    IDLE,
    SYNCING,
    SYNCED,
    FAILED
} 