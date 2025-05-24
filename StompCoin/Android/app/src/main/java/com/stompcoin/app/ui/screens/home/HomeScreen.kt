package com.stompcoin.app.ui.screens.home

import androidx.compose.foundation.layout.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import androidx.hilt.navigation.compose.hiltViewModel
import com.stompcoin.app.data.model.User

@Composable
fun HomeScreen(
    onNavigateToMining: () -> Unit,
    onNavigateToProfile: () -> Unit,
    viewModel: HomeViewModel = hiltViewModel()
) {
    var user by remember { mutableStateOf<User?>(null) }
    var isLoading by remember { mutableStateOf(true) }

    LaunchedEffect(viewModel.userState) {
        when (viewModel.userState) {
            is UserState.Success -> {
                user = (viewModel.userState as UserState.Success).user
                isLoading = false
            }
            is UserState.Error -> {
                isLoading = false
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
        if (isLoading) {
            CircularProgressIndicator()
        } else {
            user?.let { currentUser ->
                Text(
                    text = "Willkommen, ${currentUser.username}!",
                    style = MaterialTheme.typography.headlineMedium,
                    color = MaterialTheme.colorScheme.primary
                )

                Spacer(modifier = Modifier.height(16.dp))

                Text(
                    text = "Dein Kontostand: ${String.format("%.2f", currentUser.coinBalance)} Coins",
                    style = MaterialTheme.typography.titleLarge
                )

                Spacer(modifier = Modifier.height(32.dp))

                Button(
                    onClick = onNavigateToMining,
                    modifier = Modifier.fillMaxWidth()
                ) {
                    Text("Mining starten")
                }

                Spacer(modifier = Modifier.height(16.dp))

                Button(
                    onClick = onNavigateToProfile,
                    modifier = Modifier.fillMaxWidth()
                ) {
                    Text("Profil")
                }
            }
        }
    }
} 