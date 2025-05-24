package com.stompcoin.app.ui.screens.profile

import androidx.compose.foundation.layout.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import androidx.hilt.navigation.compose.hiltViewModel
import com.stompcoin.app.data.model.User

@Composable
fun ProfileScreen(
    onNavigateBack: () -> Unit,
    onLogout: () -> Unit,
    viewModel: ProfileViewModel = hiltViewModel()
) {
    var user by remember { mutableStateOf<User?>(null) }
    var isLoading by remember { mutableStateOf(true) }
    var showLogoutDialog by remember { mutableStateOf(false) }

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
                    text = "Profil",
                    style = MaterialTheme.typography.headlineMedium,
                    color = MaterialTheme.colorScheme.primary
                )

                Spacer(modifier = Modifier.height(32.dp))

                ProfileItem(
                    label = "Benutzername",
                    value = currentUser.username
                )

                ProfileItem(
                    label = "E-Mail",
                    value = currentUser.email
                )

                ProfileItem(
                    label = "Kontostand",
                    value = "${String.format("%.2f", currentUser.coinBalance)} Coins"
                )

                ProfileItem(
                    label = "Premium Status",
                    value = if (currentUser.isPremium) "Premium" else "Standard"
                )

                Spacer(modifier = Modifier.height(32.dp))

                Button(
                    onClick = { showLogoutDialog = true },
                    modifier = Modifier.fillMaxWidth(),
                    colors = ButtonDefaults.buttonColors(
                        containerColor = MaterialTheme.colorScheme.error
                    )
                ) {
                    Text("Abmelden")
                }

                Spacer(modifier = Modifier.height(16.dp))

                TextButton(
                    onClick = onNavigateBack
                ) {
                    Text("Zurück")
                }
            }
        }
    }

    if (showLogoutDialog) {
        AlertDialog(
            onDismissRequest = { showLogoutDialog = false },
            title = { Text("Abmelden") },
            text = { Text("Möchten Sie sich wirklich abmelden?") },
            confirmButton = {
                TextButton(
                    onClick = {
                        showLogoutDialog = false
                        viewModel.logout()
                        onLogout()
                    }
                ) {
                    Text("Ja")
                }
            },
            dismissButton = {
                TextButton(
                    onClick = { showLogoutDialog = false }
                ) {
                    Text("Nein")
                }
            }
        )
    }
}

@Composable
private fun ProfileItem(
    label: String,
    value: String
) {
    Column(
        modifier = Modifier
            .fillMaxWidth()
            .padding(vertical = 8.dp)
    ) {
        Text(
            text = label,
            style = MaterialTheme.typography.titleMedium,
            color = MaterialTheme.colorScheme.primary
        )
        Text(
            text = value,
            style = MaterialTheme.typography.bodyLarge
        )
    }
} 