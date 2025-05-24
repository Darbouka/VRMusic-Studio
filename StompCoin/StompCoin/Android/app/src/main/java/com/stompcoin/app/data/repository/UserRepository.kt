package com.stompcoin.app.data.repository

import com.stompcoin.app.data.model.User

interface UserRepository {
    suspend fun createUser(user: User): User
    suspend fun getUser(userId: String): User
    suspend fun getUserByEmail(email: String): User?
    suspend fun updateUser(user: User)
    suspend fun deleteUser(userId: String)
    suspend fun updateUserBalance(userId: String, stompCoins: Double, omCoins: Double)
    suspend fun updateUserPreferences(userId: String, preferences: Map<String, Any>)
} 