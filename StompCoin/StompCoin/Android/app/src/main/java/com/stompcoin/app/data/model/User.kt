package com.stompcoin.app.data.model

import java.time.LocalDate

data class User(
    val id: String,
    var username: String,
    var email: String,
    var stompCoins: Double = 0.0,
    var omCoins: Double = 0.0,
    var birthDate: LocalDate? = null,
    var country: String? = null,
    var city: String? = null,
    var favoriteMusicGenres: List<MusicGenre> = emptyList(),
    var dailyStepLimit: Int = 10000,
    var weeklyStepLimit: Int = 70000,
    var weekendStepLimit: Int = 15000,
    var videoBoostsRemaining: Int = 4,
    var currentBoostMultiplier: Double = 1.0,
    var lastBoostActivation: Long? = null,
    var createdAt: Long = System.currentTimeMillis(),
    var updatedAt: Long = System.currentTimeMillis()
)

enum class MusicGenre {
    PSYTRANCE,
    TECHNO,
    HOUSE,
    DNB,
    TRANCE,
    HARDSTYLE,
    OTHER
} 