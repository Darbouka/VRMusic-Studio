package com.stompcoin.app.data.repository

import com.stompcoin.app.data.model.StepEntry
import java.time.LocalDate

interface StepRepository {
    suspend fun saveSteps(stepEntry: StepEntry)
    suspend fun getStepsForDate(date: LocalDate): StepEntry?
    suspend fun getStepsForDateRange(startDate: LocalDate, endDate: LocalDate): List<StepEntry>
    suspend fun getTotalStepsForDate(date: LocalDate): Int
    suspend fun getTotalStepsForDateRange(startDate: LocalDate, endDate: LocalDate): Int
    suspend fun deleteStepsForDate(date: LocalDate)
} 