package com.stompcoin.app.data.managers

import android.content.Context
import android.location.Location
import com.stompcoin.app.data.model.Event
import com.stompcoin.app.data.repository.EventRepository
import dagger.hilt.android.qualifiers.ApplicationContext
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import javax.inject.Inject
import javax.inject.Singleton

@Singleton
class EventDetectionManager @Inject constructor(
    @ApplicationContext private val context: Context,
    private val eventRepository: EventRepository
) {
    private val _currentEvent = MutableStateFlow<Event?>(null)
    val currentEvent: StateFlow<Event?> = _currentEvent

    private val _isEventDetectionEnabled = MutableStateFlow(false)
    val isEventDetectionEnabled: StateFlow<Boolean> = _isEventDetectionEnabled

    private val _availableBoosts = MutableStateFlow(4)
    val availableBoosts: StateFlow<Int> = _availableBoosts

    private val _activeBoost = MutableStateFlow<BoostType?>(null)
    val activeBoost: StateFlow<BoostType?> = _activeBoost

    private val _boostEndTime = MutableStateFlow<Long?>(null)
    val boostEndTime: StateFlow<Long?> = _boostEndTime

    enum class BoostType(val multiplier: Double, val duration: Long) {
        DOUBLE(2.0, 30 * 60 * 1000), // 30 Minuten
        TRIPLE(3.0, 30 * 60 * 1000)  // 30 Minuten
    }

    suspend fun startEventDetection() {
        _isEventDetectionEnabled.value = true
        // Hier Standort-Updates starten
    }

    suspend fun stopEventDetection() {
        _isEventDetectionEnabled.value = false
        // Hier Standort-Updates stoppen
    }

    suspend fun checkForNearbyEvents(location: Location) {
        try {
            val events = eventRepository.getNearbyEvents(
                latitude = location.latitude,
                longitude = location.longitude,
                radius = 5000 // 5km Radius
            )
            
            events.firstOrNull()?.let { event ->
                _currentEvent.value = event
            }
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    suspend fun joinEvent(event: Event) {
        try {
            eventRepository.joinEvent(event)
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    fun activateBoost(type: BoostType) {
        if (_availableBoosts.value > 0) {
            _activeBoost.value = type
            _availableBoosts.value -= 1
            _boostEndTime.value = System.currentTimeMillis() + type.duration
        }
    }
} 