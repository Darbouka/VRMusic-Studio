package com.stompcoin.app.data.repository

import com.stompcoin.app.data.api.AuthApi
import com.stompcoin.app.data.model.LoginRequest
import com.stompcoin.app.data.model.RegisterRequest
import com.stompcoin.app.data.model.User
import com.stompcoin.app.data.local.UserPreferences
import kotlinx.coroutines.flow.Flow
import javax.inject.Inject
import javax.inject.Singleton

@Singleton
class AuthRepository @Inject constructor(
    private val authApi: AuthApi,
    private val userPreferences: UserPreferences
) {
    suspend fun login(email: String, password: String): User {
        val response = authApi.login(LoginRequest(email, password))
        userPreferences.saveToken(response.token)
        userPreferences.saveUser(response.user)
        return response.user
    }

    suspend fun register(username: String, email: String, password: String): User {
        val response = authApi.register(RegisterRequest(username, email, password))
        userPreferences.saveToken(response.token)
        userPreferences.saveUser(response.user)
        return response.user
    }

    suspend fun logout() {
        userPreferences.clearUser()
        userPreferences.clearToken()
    }

    fun getCurrentUser(): Flow<User?> = userPreferences.getUser()
    fun getToken(): Flow<String?> = userPreferences.getToken()
} 