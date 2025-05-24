package com.stompcoin.app.data.repository

import com.stompcoin.app.data.model.Friend
import com.stompcoin.app.data.model.FriendActivity
import com.stompcoin.app.data.model.FriendRequest
import com.stompcoin.app.data.model.FriendStatus
import kotlinx.coroutines.flow.Flow

interface FriendRepository {
    suspend fun sendFriendRequest(request: FriendRequest)
    suspend fun acceptFriendRequest(requestId: String)
    suspend fun rejectFriendRequest(requestId: String)
    suspend fun blockUser(userId: String)
    suspend fun unblockUser(userId: String)
    
    fun getFriends(userId: String): Flow<List<Friend>>
    fun getPendingRequests(userId: String): Flow<List<FriendRequest>>
    fun getBlockedUsers(userId: String): Flow<List<Friend>>
    
    suspend fun removeFriend(friendId: String)
    suspend fun updateFriendStatus(friendId: String, status: FriendStatus)
    
    fun getFriendActivities(userId: String): Flow<List<FriendActivity>>
    suspend fun addFriendActivity(activity: FriendActivity)
    
    suspend fun searchUsers(query: String): List<User>
    suspend fun getMutualFriends(userId1: String, userId2: String): List<Friend>
    
    suspend fun getFriendSuggestions(userId: String): List<User>
    suspend fun getFriendStats(userId: String): FriendStats
}

data class FriendStats(
    val totalFriends: Int,
    val activeFriends: Int,
    val pendingRequests: Int,
    val blockedUsers: Int,
    val mutualFriends: Map<String, Int>,
    val recentActivities: List<FriendActivity>
) 