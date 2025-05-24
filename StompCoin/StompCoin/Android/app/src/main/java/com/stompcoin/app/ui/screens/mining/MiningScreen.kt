package com.stompcoin.app.ui.screens.mining

import androidx.compose.foundation.layout.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import androidx.hilt.navigation.compose.hiltViewModel
import com.stompcoin.app.data.model.MiningType

@Composable
fun MiningScreen(
    onNavigateBack: () -> Unit,
    viewModel: MiningViewModel = hiltViewModel()
) {
    var selectedMiningType by remember { mutableStateOf(MiningType.SOLO) }
    var isMining by remember { mutableStateOf(false) }
    var coinsEarned by remember { mutableStateOf(0.0) }

    LaunchedEffect(viewModel.miningState) {
        when (viewModel.miningState) {
            is MiningState.Mining -> {
                isMining = true
                coinsEarned = (viewModel.miningState as MiningState.Mining).coinsEarned
            }
            is MiningState.Stopped -> {
                isMining = false
            }
            is MiningState.Error -> {
                isMining = false
            }
            else -> {}
        }
    }

    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(16.dp),
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        Text(
            text = "Mining",
            style = MaterialTheme.typography.headlineMedium,
            color = MaterialTheme.colorScheme.primary
        )

        Spacer(modifier = Modifier.height(32.dp))

        Row(
            modifier = Modifier.fillMaxWidth(),
            horizontalArrangement = Arrangement.SpaceEvenly
        ) {
            MiningTypeButton(
                text = "Solo Mining",
                selected = selectedMiningType == MiningType.SOLO,
                onClick = { selectedMiningType = MiningType.SOLO }
            )
            MiningTypeButton(
                text = "Pool Mining",
                selected = selectedMiningType == MiningType.POOL,
                onClick = { selectedMiningType = MiningType.POOL }
            )
        }

        Spacer(modifier = Modifier.height(32.dp))

        Text(
            text = "Coins verdient: ${String.format("%.2f", coinsEarned)}",
            style = MaterialTheme.typography.headlineSmall
        )

        Spacer(modifier = Modifier.height(32.dp))

        Button(
            onClick = {
                if (isMining) {
                    viewModel.stopMining()
                } else {
                    viewModel.startMining(selectedMiningType)
                }
            },
            modifier = Modifier.fillMaxWidth()
        ) {
            Text(if (isMining) "Mining stoppen" else "Mining starten")
        }

        Spacer(modifier = Modifier.height(16.dp))

        TextButton(
            onClick = onNavigateBack
        ) {
            Text("Zurück")
        }
    }
}

@Composable
private fun MiningTypeButton(
    text: String,
    selected: Boolean,
    onClick: () -> Unit
) {
    Button(
        onClick = onClick,
        colors = ButtonDefaults.buttonColors(
            containerColor = if (selected) 
                MaterialTheme.colorScheme.primary 
            else 
                MaterialTheme.colorScheme.secondary
        )
    ) {
        Text(text)
    }
} 