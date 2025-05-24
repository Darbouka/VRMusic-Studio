package com.stompcoin.app.data.model

import java.time.LocalDate

data class StepEntry(
    val id: String = java.util.UUID.randomUUID().toString(),
    val userId: String,
    val date: LocalDate,
    val count: Int,
    val distance: Double? = null,
    val calories: Double? = null,
    val isFitnessFloor: Boolean = false,
    val fitnessFloorDuration: Long? = null,
    val createdAt: Long = System.currentTimeMillis(),
    var updatedAt: Long = System.currentTimeMillis()
) 