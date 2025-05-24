package com.stompcoin.app.data.managers

import android.annotation.SuppressLint
import android.content.Context
import android.location.Location
import android.os.Looper
import com.google.android.gms.location.*
import dagger.hilt.android.qualifiers.ApplicationContext
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import javax.inject.Inject
import javax.inject.Singleton

@Singleton
class LocationManager @Inject constructor(
    @ApplicationContext private val context: Context
) {
    private val fusedLocationClient: FusedLocationProviderClient = LocationServices.getFusedLocationProviderClient(context)
    private val locationRequest = LocationRequest.create().apply {
        priority = Priority.PRIORITY_BALANCED_POWER_ACCURACY
        interval = 300000 // 5 Minuten
        fastestInterval = 300000 // 5 Minuten
        maxWaitTime = 600000 // 10 Minuten
    }

    private val _currentLocation = MutableStateFlow<Location?>(null)
    val currentLocation: StateFlow<Location?> = _currentLocation

    private val _locationError = MutableStateFlow<String?>(null)
    val locationError: StateFlow<String?> = _locationError

    private var locationCallback: LocationCallback = object : LocationCallback() {
        override fun onLocationResult(locationResult: LocationResult) {
            locationResult.lastLocation?.let { location ->
                _currentLocation.value = location
            }
        }

        override fun onLocationAvailability(availability: LocationAvailability) {
            if (!availability.isLocationAvailable) {
                _locationError.value = "Standort ist nicht verfügbar"
            }
        }
    }

    @SuppressLint("MissingPermission")
    fun startLocationUpdates() {
        try {
            fusedLocationClient.requestLocationUpdates(
                locationRequest,
                locationCallback,
                Looper.getMainLooper()
            )
        } catch (e: SecurityException) {
            _locationError.value = "Standortberechtigung fehlt"
        }
    }

    fun stopLocationUpdates() {
        fusedLocationClient.removeLocationUpdates(locationCallback)
    }

    @SuppressLint("MissingPermission")
    fun getLastLocation(): Location? {
        return try {
            fusedLocationClient.lastLocation.result
        } catch (e: Exception) {
            _locationError.value = "Fehler beim Abrufen des letzten Standorts"
            null
        }
    }

    fun updateLocationRequest(
        priority: Int = Priority.PRIORITY_BALANCED_POWER_ACCURACY,
        interval: Long = 300000,
        fastestInterval: Long = 300000,
        maxWaitTime: Long = 600000
    ) {
        locationRequest.apply {
            this.priority = priority
            this.interval = interval
            this.fastestInterval = fastestInterval
            this.maxWaitTime = maxWaitTime
        }
    }
} 