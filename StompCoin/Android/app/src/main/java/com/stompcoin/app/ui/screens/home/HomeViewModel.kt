package com.stompcoin.app.ui.screens.home

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.stompcoin.app.data.model.User
import com.stompcoin.app.data.repository.AuthRepository
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.launch
import javax.inject.Inject

sealed class UserState {
    object Initial : UserState()
    object Loading : UserState()
    data class Success(val user: User) : UserState()
    data class Error(val message: String) : UserState()
}

@HiltViewModel
class HomeViewModel @Inject constructor(
    private val authRepository: AuthRepository
) : ViewModel() {

    private val _userState = MutableStateFlow<UserState>(UserState.Initial)
    val userState: StateFlow<UserState> = _userState

    init {
        loadUser()
    }

    private fun loadUser() {
        viewModelScope.launch {
            _userState.value = UserState.Loading
            try {
                authRepository.getCurrentUser().collect { user ->
                    user?.let {
                        _userState.value = UserState.Success(it)
                    } ?: run {
                        _userState.value = UserState.Error("Benutzer nicht gefunden")
                    }
                }
            } catch (e: Exception) {
                _userState.value = UserState.Error(e.message ?: "Ein Fehler ist aufgetreten")
            }
        }
    }
} 