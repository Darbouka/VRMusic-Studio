package com.stompcoin.app.data.api

import com.stompcoin.app.data.model.AuthResponse
import com.stompcoin.app.data.model.LoginRequest
import com.stompcoin.app.data.model.RegisterRequest
import retrofit2.http.Body
import retrofit2.http.POST

interface AuthApi {
    @POST("auth/login")
    suspend fun login(@Body request: LoginRequest): AuthResponse

    @POST("auth/register")
    suspend fun register(@Body request: RegisterRequest): AuthResponse
} 