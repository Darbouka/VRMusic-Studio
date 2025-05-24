package com.stompcoin.app.data.repository

import com.stompcoin.app.data.model.Event

interface EventRepository {
    suspend fun getNearbyEvents(latitude: Double, longitude: Double, radius: Int): List<Event>
    suspend fun joinEvent(event: Event)
    suspend fun getEventDetails(eventId: String): Event
    suspend fun createEvent(event: Event): Event
    suspend fun updateEvent(event: Event)
    suspend fun deleteEvent(eventId: String)
} 