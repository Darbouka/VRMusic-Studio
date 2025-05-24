package com.stompcoin.app.data.model

import java.time.LocalDateTime

data class Event(
    val id: String,
    val name: String,
    val description: String,
    val location: Location,
    val startDate: LocalDateTime,
    val endDate: LocalDateTime,
    val musicGenres: List<MusicGenre>,
    val challenges: List<Challenge>,
    var participants: List<User> = emptyList(),
    var rewardMultiplier: Double = 1.0,
    var boostEndTime: Long? = null,
    val maxParticipants: Int? = null,
    val createdAt: Long = System.currentTimeMillis(),
    var updatedAt: Long = System.currentTimeMillis()
)

data class Location(
    val latitude: Double,
    val longitude: Double,
    val name: String,
    val address: String
)

data class Challenge(
    val id: String,
    val name: String,
    val description: String,
    val reward: Int,
    var participants: List<User> = emptyList(),
    val startDate: LocalDateTime,
    val endDate: LocalDateTime
) 