package com.stompcoin.app.data.api

import com.stompcoin.app.data.model.MiningSession
import com.stompcoin.app.data.model.MiningType
import kotlinx.coroutines.flow.Flow
import retrofit2.http.*

interface MiningApi {
    @POST("mining/start")
    fun startMining(@Body type: MiningType): Flow<MiningSession>

    @POST("mining/stop")
    suspend fun stopMining()

    @GET("mining/history")
    fun getMiningHistory(): Flow<List<MiningSession>>
} 