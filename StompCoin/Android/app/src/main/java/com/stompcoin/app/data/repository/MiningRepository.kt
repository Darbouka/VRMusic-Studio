package com.stompcoin.app.data.repository

import com.stompcoin.app.data.api.MiningApi
import com.stompcoin.app.data.model.MiningSession
import com.stompcoin.app.data.model.MiningType
import kotlinx.coroutines.flow.Flow
import javax.inject.Inject
import javax.inject.Singleton

@Singleton
class MiningRepository @Inject constructor(
    private val miningApi: MiningApi
) {
    fun startMining(type: MiningType): Flow<MiningSession> {
        return miningApi.startMining(type)
    }

    suspend fun stopMining() {
        miningApi.stopMining()
    }

    fun getMiningHistory(): Flow<List<MiningSession>> {
        return miningApi.getMiningHistory()
    }
} 