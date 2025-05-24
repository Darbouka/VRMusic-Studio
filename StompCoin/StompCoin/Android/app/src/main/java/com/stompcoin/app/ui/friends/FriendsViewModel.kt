package com.stompcoin.app.ui.friends

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.stompcoin.app.data.managers.FriendManager
import com.stompcoin.app.data.model.Friend
import com.stompcoin.app.data.model.FriendActivity
import com.stompcoin.app.data.model.FriendRequest
import com.stompcoin.app.data.model.FriendStats
import com.stompcoin.app.data.model.User
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.flow.update
import kotlinx.coroutines.launch
import javax.inject.Inject

@HiltViewModel
class FriendsViewModel @Inject constructor(
    private val friendManager: FriendManager
) : ViewModel() {

    private val _uiState = MutableStateFlow(FriendsUiState())
    val uiState: StateFlow<FriendsUiState> = _uiState.asStateFlow()

    init {
        loadFriends()
        loadPendingRequests()
        loadFriendActivities()
        loadFriendStats()
    }

    private fun loadFriends() {
        viewModelScope.launch {
            friendManager.getFriends(_uiState.value.currentUserId).collect { friends ->
                _uiState.update { it.copy(friends = friends) }
            }
        }
    }

    private fun loadPendingRequests() {
        viewModelScope.launch {
            friendManager.getPendingRequests(_uiState.value.currentUserId).collect { requests ->
                _uiState.update { it.copy(pendingRequests = requests) }
            }
        }
    }

    private fun loadFriendActivities() {
        viewModelScope.launch {
            friendManager.getFriendActivities(_uiState.value.currentUserId).collect { activities ->
                _uiState.update { it.copy(friendActivities = activities) }
            }
        }
    }

    private fun loadFriendStats() {
        viewModelScope.launch {
            val stats = friendManager.getFriendStats(_uiState.value.currentUserId)
            _uiState.update { it.copy(friendStats = stats) }
        }
    }

    fun searchUsers(query: String) {
        viewModelScope.launch {
            _uiState.update { it.copy(isLoading = true) }
            try {
                val users = friendManager.searchUsers(query)
                _uiState.update { it.copy(searchResults = users, isLoading = false) }
            } catch (e: Exception) {
                _uiState.update { 
                    it.copy(
                        error = e.message ?: "Fehler bei der Suche",
                        isLoading = false
                    )
                }
            }
        }
    }

    fun sendFriendRequest(userId: String, message: String? = null) {
        viewModelScope.launch {
            try {
                val request = FriendRequest(
                    id = "",
                    senderId = _uiState.value.currentUserId,
                    receiverId = userId,
                    message = message
                )
                friendManager.sendFriendRequest(request)
                _uiState.update { it.copy(error = null) }
            } catch (e: Exception) {
                _uiState.update { it.copy(error = e.message ?: "Fehler beim Senden der Anfrage") }
            }
        }
    }

    fun acceptFriendRequest(requestId: String) {
        viewModelScope.launch {
            try {
                friendManager.acceptFriendRequest(requestId)
                _uiState.update { it.copy(error = null) }
            } catch (e: Exception) {
                _uiState.update { it.copy(error = e.message ?: "Fehler beim Akzeptieren der Anfrage") }
            }
        }
    }

    fun rejectFriendRequest(requestId: String) {
        viewModelScope.launch {
            try {
                friendManager.rejectFriendRequest(requestId)
                _uiState.update { it.copy(error = null) }
            } catch (e: Exception) {
                _uiState.update { it.copy(error = e.message ?: "Fehler beim Ablehnen der Anfrage") }
            }
        }
    }

    fun removeFriend(friendId: String) {
        viewModelScope.launch {
            try {
                friendManager.removeFriend(friendId)
                _uiState.update { it.copy(error = null) }
            } catch (e: Exception) {
                _uiState.update { it.copy(error = e.message ?: "Fehler beim Entfernen des Freundes") }
            }
        }
    }

    fun blockUser(userId: String) {
        viewModelScope.launch {
            try {
                friendManager.blockUser(userId)
                _uiState.update { it.copy(error = null) }
            } catch (e: Exception) {
                _uiState.update { it.copy(error = e.message ?: "Fehler beim Blockieren des Benutzers") }
            }
        }
    }

    fun clearError() {
        _uiState.update { it.copy(error = null) }
    }
}

data class FriendsUiState(
    val currentUserId: String = "", // Wird vom AuthManager gesetzt
    val friends: List<Friend> = emptyList(),
    val pendingRequests: List<FriendRequest> = emptyList(),
    val friendActivities: List<FriendActivity> = emptyList(),
    val friendStats: FriendStats? = null,
    val searchResults: List<User> = emptyList(),
    val isLoading: Boolean = false,
    val error: String? = null
) 