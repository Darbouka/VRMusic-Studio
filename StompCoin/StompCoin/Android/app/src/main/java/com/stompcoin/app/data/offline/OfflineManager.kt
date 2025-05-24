package com.stompcoin.app.data.offline

import android.content.Context
import android.net.ConnectivityManager
import android.net.Network
import android.net.NetworkCapabilities
import android.net.NetworkRequest
import com.stompcoin.app.data.model.Event
import com.stompcoin.app.data.model.StepEntry
import com.stompcoin.app.data.model.Transaction
import com.stompcoin.app.data.repository.EventRepository
import com.stompcoin.app.data.repository.StepRepository
import com.stompcoin.app.data.repository.TransactionRepository
import dagger.hilt.android.qualifiers.ApplicationContext
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import javax.inject.Inject
import javax.inject.Singleton

@Singleton
class OfflineManager @Inject constructor(
    @ApplicationContext private val context: Context,
    private val eventRepository: EventRepository,
    private val stepRepository: StepRepository,
    private val transactionRepository: TransactionRepository
) {
    private val connectivityManager = context.getSystemService(Context.CONNECTIVITY_SERVICE) as ConnectivityManager
    private val _isOnline = MutableStateFlow(true)
    val isOnline: StateFlow<Boolean> = _isOnline

    private val pendingEvents = mutableListOf<Event>()
    private val pendingSteps = mutableListOf<StepEntry>()
    private val pendingTransactions = mutableListOf<Transaction>()

    init {
        setupNetworkCallback()
    }

    private fun setupNetworkCallback() {
        val networkCallback = object : ConnectivityManager.NetworkCallback() {
            override fun onAvailable(network: Network) {
                _isOnline.value = true
                syncPendingData()
            }

            override fun onLost(network: Network) {
                _isOnline.value = false
            }
        }

        val networkRequest = NetworkRequest.Builder()
            .addCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET)
            .build()

        connectivityManager.registerNetworkCallback(networkRequest, networkCallback)
    }

    suspend fun saveEventOffline(event: Event) {
        if (!isOnline.value) {
            pendingEvents.add(event)
        } else {
            eventRepository.createEvent(event)
        }
    }

    suspend fun saveStepsOffline(stepEntry: StepEntry) {
        if (!isOnline.value) {
            pendingSteps.add(stepEntry)
        } else {
            stepRepository.saveSteps(stepEntry)
        }
    }

    suspend fun saveTransactionOffline(transaction: Transaction) {
        if (!isOnline.value) {
            pendingTransactions.add(transaction)
        } else {
            transactionRepository.createTransaction(transaction)
        }
    }

    private suspend fun syncPendingData() {
        pendingEvents.forEach { event ->
            try {
                eventRepository.createEvent(event)
            } catch (e: Exception) {
                e.printStackTrace()
            }
        }
        pendingEvents.clear()

        pendingSteps.forEach { stepEntry ->
            try {
                stepRepository.saveSteps(stepEntry)
            } catch (e: Exception) {
                e.printStackTrace()
            }
        }
        pendingSteps.clear()

        pendingTransactions.forEach { transaction ->
            try {
                transactionRepository.createTransaction(transaction)
            } catch (e: Exception) {
                e.printStackTrace()
            }
        }
        pendingTransactions.clear()
    }

    fun isNetworkAvailable(): Boolean {
        val network = connectivityManager.activeNetwork
        val capabilities = connectivityManager.getNetworkCapabilities(network)
        return capabilities?.hasCapability(NetworkCapabilities.NET_CAPABILITY_INTERNET) == true
    }

    fun getPendingDataCount(): Int {
        return pendingEvents.size + pendingSteps.size + pendingTransactions.size
    }
} 