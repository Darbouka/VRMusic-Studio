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

    fun startCounting() {
        if (stepSensor == null) {
            _error.value = "Schrittzähler ist auf diesem Gerät nicht verfügbar"
            return
        }

        sensorManager.registerListener(this, stepSensor, SensorManager.SENSOR_DELAY_NORMAL)
        _isCounting.value = true
        _error.value = null
    }

    fun stopCounting() {
        sensorManager.unregisterListener(this)
        _isCounting.value = false
    }

    fun activateFitnessFloor() {
        _isFitnessFloorActive.value = true
        fitnessFloorStartTime = System.currentTimeMillis()
    }

    fun deactivateFitnessFloor() {
        _isFitnessFloorActive.value = false
        fitnessFloorStartTime = null
    }

    override fun onSensorChanged(event: SensorEvent) {
        if (event.sensor.type == Sensor.TYPE_STEP_COUNTER) {
            _steps.value = event.values[0].toInt()
            _lastUpdate.value = System.currentTimeMillis()
        }
    }

    override fun onAccuracyChanged(sensor: Sensor?, accuracy: Int) {
        // Nicht benötigt
    }

    fun calculateFitnessDistance(): Double? {
        if (!_isFitnessFloorActive.value || fitnessFloorStartTime == null) return null
        // Durchschnittliche Schrittlänge in Metern (ca. 0,762m)
        val averageStepLength = 0.762
        return _steps.value * averageStepLength
    }

    fun calculateFitnessCalories(): Double? {
        if (!_isFitnessFloorActive.value || fitnessFloorStartTime == null) return null
        // Durchschnittlicher Kalorienverbrauch pro Schritt (ca. 0,04 kcal)
        val caloriesPerStep = 0.04
        return _steps.value * caloriesPerStep
    }

    fun getFitnessDuration(): Long? {
        if (!_isFitnessFloorActive.value || fitnessFloorStartTime == null) return null
        return System.currentTimeMillis() - fitnessFloorStartTime!!
    }
} 