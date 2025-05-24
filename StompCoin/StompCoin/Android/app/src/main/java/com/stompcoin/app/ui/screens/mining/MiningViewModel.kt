package com.stompcoin.app.ui.screens.mining

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.stompcoin.app.data.model.MiningType
import com.stompcoin.app.data.repository.MiningRepository
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.launch
import javax.inject.Inject

sealed class MiningState {
    object Initial : MiningState()
    data class Mining(val coinsEarned: Double) : MiningState()
    object Stopped : MiningState()
    data class Error(val message: String) : MiningState()
}

@HiltViewModel
class MiningViewModel @Inject constructor(
    private val miningRepository: MiningRepository
) : ViewModel() {

    private val _miningState = MutableStateFlow<MiningState>(MiningState.Initial)
    val miningState: StateFlow<MiningState> = _miningState

    fun startMining(type: MiningType) {
        viewModelScope.launch {
            try {
                miningRepository.startMining(type).collect { session ->
                    _miningState.value = MiningState.Mining(session.coinsEarned)
                }
            } catch (e: Exception) {
                _miningState.value = MiningState.Error(e.message ?: "Ein Fehler ist aufgetreten")
            }
        }
    }

    fun stopMining() {
        viewModelScope.launch {
            try {
                miningRepository.stopMining()
                _miningState.value = MiningState.Stopped
            } catch (e: Exception) {
                _miningState.value = MiningState.Error(e.message ?: "Ein Fehler ist aufgetreten")
            }
        }
    }
} 