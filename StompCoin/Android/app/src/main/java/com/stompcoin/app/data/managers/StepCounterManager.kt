package com.stompcoin.app.data.managers

import android.content.Context
import android.hardware.Sensor
import android.hardware.SensorEvent
import android.hardware.SensorEventListener
import android.hardware.SensorManager
import com.stompcoin.app.data.repository.StepRepository
import dagger.hilt.android.qualifiers.ApplicationContext
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import javax.inject.Inject
import javax.inject.Singleton

@Singleton
class StepCounterManager @Inject constructor(
    @ApplicationContext private val context: Context,
    private val stepRepository: StepRepository
) : SensorEventListener {
    private val sensorManager = context.getSystemService(Context.SENSOR_SERVICE) as SensorManager
    private val stepSensor = sensorManager.getDefaultSensor(Sensor.TYPE_STEP_COUNTER)
    private val accelerometer = sensorManager.getDefaultSensor(Sensor.TYPE_ACCELEROMETER)

    private val _steps = MutableStateFlow(0)
    val steps: StateFlow<Int> = _steps

    private val _isCounting = MutableStateFlow(false)
    val isCounting: StateFlow<Boolean> = _isCounting

    private val _lastUpdate = MutableStateFlow<Long?>(null)
    val lastUpdate: StateFlow<Long?> = _lastUpdate

    private val _error = MutableStateFlow<String?>(null)
    val error: StateFlow<String?> = _error

    private val _isFitnessFloorActive = MutableStateFlow(false)
    val isFitnessFloorActive: StateFlow<Boolean> = _isFitnessFloorActive

    private var fitnessFloorStartTime: Long? = null
    private var lastStepCount = 0
    private var isUsingAccelerometer = false

    fun startCounting() {
        if (stepSensor == null) {
            // Fallback auf Beschleunigungssensor
            if (accelerometer != null) {
                sensorManager.registerListener(this, accelerometer, SensorManager.SENSOR_DELAY_NORMAL)
                isUsingAccelerometer = true
                _isCounting.value = true
                _error.value = null
            } else {
                _error.value = "Kein Bewegungssensor auf diesem Gerät verfügbar"
            }
            return
        }

        sensorManager.registerListener(this, stepSensor, SensorManager.SENSOR_DELAY_NORMAL)
        _isCounting.value = true
        _error.value = null
    }

    fun stopCounting() {
        if (isUsingAccelerometer) {
            sensorManager.unregisterListener(this, accelerometer)
        } else {
            sensorManager.unregisterListener(this, stepSensor)
        }
        _isCounting.value = false
    }

    override fun onSensorChanged(event: SensorEvent) {
        if (event.sensor.type == Sensor.TYPE_STEP_COUNTER) {
            val totalSteps = event.values[0].toInt()
            val stepsSinceLastUpdate = totalSteps - lastStepCount
            if (stepsSinceLastUpdate > 0) {
                _steps.value += stepsSinceLastUpdate
                lastStepCount = totalSteps
                _lastUpdate.value = System.currentTimeMillis()
            }
        } else if (event.sensor.type == Sensor.TYPE_ACCELEROMETER) {
            // Einfache Schritterkennung basierend auf Beschleunigung
            val x = event.values[0]
            val y = event.values[1]
            val z = event.values[2]
            
            val acceleration = Math.sqrt((x * x + y * y + z * z).toDouble())
            if (acceleration > 12.0) { // Schwellenwert für Schritt
                _steps.value++
                _lastUpdate.value = System.currentTimeMillis()
            }
        }
    }

    override fun onAccuracyChanged(sensor: Sensor?, accuracy: Int) {
        // Nicht benötigt
    }

    fun resetSteps() {
        _steps.value = 0
        lastStepCount = 0
        _lastUpdate.value = System.currentTimeMillis()
    }

    fun startFitnessFloor() {
        _isFitnessFloorActive.value = true
        fitnessFloorStartTime = System.currentTimeMillis()
    }

    fun stopFitnessFloor() {
        _isFitnessFloorActive.value = false
        fitnessFloorStartTime = null
    }
} 