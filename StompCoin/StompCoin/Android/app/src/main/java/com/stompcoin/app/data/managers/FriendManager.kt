package com.stompcoin.app.data.managers

import com.stompcoin.app.data.model.Friend
import com.stompcoin.app.data.model.FriendActivity
import com.stompcoin.app.data.model.FriendRequest
import com.stompcoin.app.data.model.FriendStatus
import com.stompcoin.app.data.repository.FriendRepository
import com.stompcoin.app.notifications.NotificationManager
import kotlinx.coroutines.flow.Flow
import kotlinx.coroutines.flow.map
import javax.inject.Inject
import javax.inject.Singleton

@Singleton
class FriendManager @Inject constructor(
    private val friendRepository: FriendRepository,
    private val notificationManager: NotificationManager
) {
    suspend fun sendFriendRequest(request: FriendRequest) {
        friendRepository.sendFriendRequest(request)
        notificationManager.showFriendRequestNotification(request.receiverId)
    }

    suspend fun acceptFriendRequest(requestId: String) {
        friendRepository.acceptFriendRequest(requestId)
        // Benachrichtigung an den Absender senden
        val request = friendRepository.getPendingRequests(requestId).map { requests ->
            requests.find { it.id == requestId }
        }
        request.collect { friendRequest ->
            friendRequest?.let {
                notificationManager.showFriendRequestAcceptedNotification(it.senderId)
            }
        }
    }

    suspend fun rejectFriendRequest(requestId: String) {
        friendRepository.rejectFriendRequest(requestId)
    }

    suspend fun blockUser(userId: String) {
        friendRepository.blockUser(userId)
    }

    suspend fun unblockUser(userId: String) {
        friendRepository.unblockUser(userId)
    }

    fun getFriends(userId: String): Flow<List<Friend>> {
        return friendRepository.getFriends(userId)
    }

    fun getPendingRequests(userId: String): Flow<List<FriendRequest>> {
        return friendRepository.getPendingRequests(userId)
    }

    fun getBlockedUsers(userId: String): Flow<List<Friend>> {
        return friendRepository.getBlockedUsers(userId)
    }

    suspend fun removeFriend(friendId: String) {
        friendRepository.removeFriend(friendId)
    }

    suspend fun updateFriendStatus(friendId: String, status: FriendStatus) {
        friendRepository.updateFriendStatus(friendId, status)
    }

    fun getFriendActivities(userId: String): Flow<List<FriendActivity>> {
        return friendRepository.getFriendActivities(userId)
    }

    suspend fun addFriendActivity(activity: FriendActivity) {
        friendRepository.addFriendActivity(activity)
        // Benachrichtigung an Freunde senden
        friendRepository.getFriends(activity.userId).collect { friends ->
            friends.forEach { friend ->
                notificationManager.showFriendActivityNotification(
                    friend.friendId,
                    activity.type,
                    activity.description
                )
            }
        }
    }

    suspend fun searchUsers(query: String): List<User> {
        return friendRepository.searchUsers(query)
    }

    suspend fun getMutualFriends(userId1: String, userId2: String): List<Friend> {
        return friendRepository.getMutualFriends(userId1, userId2)
    }

    suspend fun getFriendSuggestions(userId: String): List<User> {
        return friendRepository.getFriendSuggestions(userId)
    }

    suspend fun getFriendStats(userId: String): FriendStats {
        return friendRepository.getFriendStats(userId)
    }

    fun getActiveFriends(userId: String): Flow<List<Friend>> {
        return friendRepository.getFriends(userId).map { friends ->
            friends.filter { it.status == FriendStatus.ACCEPTED }
        }
    }

    suspend fun isFriend(userId1: String, userId2: String): Boolean {
        return friendRepository.getFriends(userId1).map { friends ->
            friends.any { it.friendId == userId2 && it.status == FriendStatus.ACCEPTED }
        }
    }
} 