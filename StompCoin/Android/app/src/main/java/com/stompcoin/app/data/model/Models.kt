package com.stompcoin.app.data.model

data class User(
    val id: String,
    val username: String,
    val email: String,
    val coinBalance: Double,
    val isPremium: Boolean = false
)

data class LoginRequest(
    val email: String,
    val password: String
)

data class RegisterRequest(
    val username: String,
    val email: String,
    val password: String
)

data class AuthResponse(
    val token: String,
    val user: User
)

data class Event(
    val id: String,
    val title: String,
    val description: String,
    val location: Location,
    val startDate: String,
    val endDate: String,
    val maxParticipants: Int,
    val currentParticipants: Int,
    val genre: String,
    val organizer: User
)

data class Location(
    val latitude: Double,
    val longitude: Double,
    val address: String
)

data class MiningSession(
    val id: String,
    val userId: String,
    val type: MiningType,
    val startTime: String,
    val endTime: String?,
    val coinsEarned: Double
)

enum class MiningType {
    SOLO,
    POOL
}

data class LeaderboardEntry(
    val userId: String,
    val username: String,
    val score: Double,
    val rank: Int
) 