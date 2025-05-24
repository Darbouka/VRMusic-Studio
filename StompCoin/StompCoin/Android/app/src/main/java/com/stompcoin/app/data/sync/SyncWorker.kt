package com.stompcoin.app.data.sync

import android.content.Context
import androidx.hilt.work.HiltWorker
import androidx.work.CoroutineWorker
import androidx.work.WorkerParameters
import com.stompcoin.app.data.local.StompCoinDatabase
import com.stompcoin.app.data.remote.StompCoinApi
import dagger.assisted.Assisted
import dagger.assisted.AssistedInject
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext

@HiltWorker
class SyncWorker @AssistedInject constructor(
    @Assisted appContext: Context,
    @Assisted workerParams: WorkerParameters,
    private val database: StompCoinDatabase,
    private val api: StompCoinApi
) : CoroutineWorker(appContext, workerParams) {

    override suspend fun doWork(): Result = withContext(Dispatchers.IO) {
        try {
            // Synchronisiere alle Daten
            syncUsers()
            syncFriends()
            syncEvents()
            syncChallenges()
            syncAchievements()
            syncTransactions()
            syncStepCounts()

            Result.success()
        } catch (e: Exception) {
            if (runAttemptCount < 3) {
                Result.retry()
            } else {
                Result.failure()
            }
        }
    }

    private suspend fun syncUsers() {
        try {
            val remoteUsers = api.getUsers()
            remoteUsers.forEach { user ->
                database.userDao().insertUser(user)
            }
        } catch (e: Exception) {
            // Log error
        }
    }

    private suspend fun syncFriends() {
        try {
            val remoteFriends = api.getFriends()
            remoteFriends.forEach { friend ->
                database.friendDao().insertFriend(friend)
            }
        } catch (e: Exception) {
            // Log error
        }
    }

    private suspend fun syncEvents() {
        try {
            val remoteEvents = api.getEvents()
            remoteEvents.forEach { event ->
                database.eventDao().insertEvent(event)
            }
        } catch (e: Exception) {
            // Log error
        }
    }

    private suspend fun syncChallenges() {
        try {
            val remoteChallenges = api.getChallenges()
            remoteChallenges.forEach { challenge ->
                database.challengeDao().insertChallenge(challenge)
            }
        } catch (e: Exception) {
            // Log error
        }
    }

    private suspend fun syncAchievements() {
        try {
            val remoteAchievements = api.getAchievements()
            remoteAchievements.forEach { achievement ->
                database.achievementDao().insertAchievement(achievement)
            }
        } catch (e: Exception) {
            // Log error
        }
    }

    private suspend fun syncTransactions() {
        try {
            val remoteTransactions = api.getTransactions()
            remoteTransactions.forEach { transaction ->
                database.transactionDao().insertTransaction(transaction)
            }
        } catch (e: Exception) {
            // Log error
        }
    }

    private suspend fun syncStepCounts() {
        try {
            val remoteStepCounts = api.getStepCounts()
            remoteStepCounts.forEach { stepCount ->
                database.stepCountDao().insertStepCount(stepCount)
            }
        } catch (e: Exception) {
            // Log error
        }
    }
} 