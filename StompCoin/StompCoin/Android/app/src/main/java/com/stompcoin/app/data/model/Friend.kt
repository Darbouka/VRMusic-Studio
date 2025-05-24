package com.stompcoin.app.data.model

import java.time.LocalDateTime

data class Friend(
    val id: String,
    val userId: String,
    val friendId: String,
    val status: FriendStatus,
    val createdAt: LocalDateTime = LocalDateTime.now(),
    val updatedAt: LocalDateTime = LocalDateTime.now()
)

enum class FriendStatus {
    PENDING,
    ACCEPTED,
    REJECTED,
    BLOCKED
}

data class FriendRequest(
    val id: String,
    val senderId: String,
    val receiverId: String,
    val message: String? = null,
    val createdAt: LocalDateTime = LocalDateTime.now()
)

data class FriendActivity(
    val id: String,
    val userId: String,
    val type: ActivityType,
    val description: String,
    val timestamp: LocalDateTime = LocalDateTime.now()
)

enum class ActivityType {
    STEPS_COMPLETED,
    CHALLENGE_WON,
    EVENT_ATTENDED,
    ACHIEVEMENT_UNLOCKED,
    COINS_EARNED
} 