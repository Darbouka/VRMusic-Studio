package com.stompcoin.app.data.cache

import com.stompcoin.app.data.model.Event
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import java.util.concurrent.ConcurrentHashMap
import javax.inject.Inject
import javax.inject.Singleton

@Singleton
class EventCache @Inject constructor() {
    private val cache = ConcurrentHashMap<String, CacheEntry<Event>>()
    private val _cachedEvents = MutableStateFlow<List<Event>>(emptyList())
    val cachedEvents: StateFlow<List<Event>> = _cachedEvents

    data class CacheEntry<T>(
        val data: T,
        val timestamp: Long = System.currentTimeMillis()
    )

    companion object {
        private const val CACHE_DURATION = 5 * 60 * 1000 // 5 Minuten
    }

    fun putEvent(event: Event) {
        cache[event.id] = CacheEntry(event)
        updateCachedEventsList()
    }

    fun putEvents(events: List<Event>) {
        events.forEach { event ->
            cache[event.id] = CacheEntry(event)
        }
        updateCachedEventsList()
    }

    fun getEvent(eventId: String): Event? {
        val entry = cache[eventId] ?: return null
        if (isEntryValid(entry)) {
            return entry.data
        }
        cache.remove(eventId)
        return null
    }

    fun getEvents(): List<Event> {
        cleanExpiredEntries()
        return cache.values
            .filter { isEntryValid(it) }
            .map { it.data }
    }

    fun clear() {
        cache.clear()
        updateCachedEventsList()
    }

    private fun isEntryValid(entry: CacheEntry<*>): Boolean {
        return System.currentTimeMillis() - entry.timestamp < CACHE_DURATION
    }

    private fun cleanExpiredEntries() {
        cache.entries.removeIf { !isEntryValid(it.value) }
        updateCachedEventsList()
    }

    private fun updateCachedEventsList() {
        _cachedEvents.value = getEvents()
    }
} 