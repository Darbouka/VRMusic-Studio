package com.stompcoin.app.ui.friends.components

import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.unit.dp
import com.stompcoin.app.data.model.User

@Composable
fun UserSearchResults(
    users: List<User>,
    onUserClick: (User) -> Unit,
    onAddFriend: (User) -> Unit,
    modifier: Modifier = Modifier
) {
    if (users.isEmpty()) {
        Box(
            modifier = modifier.fillMaxSize(),
            contentAlignment = Alignment.Center
        ) {
            Text(
                text = "Keine Benutzer gefunden",
                style = MaterialTheme.typography.bodyLarge
            )
        }
    } else {
        LazyColumn(
            modifier = modifier.fillMaxSize(),
            verticalArrangement = Arrangement.spacedBy(8.dp)
        ) {
            items(users) { user ->
                UserSearchItem(
                    user = user,
                    onUserClick = { onUserClick(user) },
                    onAddFriend = { onAddFriend(user) }
                )
            }
        }
    }
}

@Composable
fun UserSearchItem(
    user: User,
    onUserClick: () -> Unit,
    onAddFriend: () -> Unit
) {
    var showAddDialog by remember { mutableStateOf(false) }

    Card(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp)
            .clickable(onClick = onUserClick)
    ) {
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(16.dp),
            horizontalArrangement = Arrangement.SpaceBetween,
            verticalAlignment = Alignment.CenterVertically
        ) {
            Row(
                modifier = Modifier.weight(1f),
                verticalAlignment = Alignment.CenterVertically
            ) {
                Icon(
                    imageVector = Icons.Default.Person,
                    contentDescription = null,
                    modifier = Modifier.size(40.dp)
                )
                Spacer(modifier = Modifier.width(16.dp))
                Column {
                    Text(
                        text = user.username,
                        style = MaterialTheme.typography.titleMedium
                    )
                    Text(
                        text = user.email,
                        style = MaterialTheme.typography.bodyMedium,
                        maxLines = 1,
                        overflow = TextOverflow.Ellipsis
                    )
                }
            }
            IconButton(onClick = { showAddDialog = true }) {
                Icon(Icons.Default.PersonAdd, contentDescription = "Freund hinzufügen")
            }
        }
    }

    if (showAddDialog) {
        AddFriendDialog(
            user = user,
            onDismiss = { showAddDialog = false },
            onConfirm = {
                onAddFriend()
                showAddDialog = false
            }
        )
    }
}

@Composable
private fun AddFriendDialog(
    user: User,
    onDismiss: () -> Unit,
    onConfirm: () -> Unit
) {
    var message by remember { mutableStateOf("") }

    AlertDialog(
        onDismissRequest = onDismiss,
        title = { Text("Freund hinzufügen") },
        text = {
            Column {
                Text("Möchten Sie ${user.username} als Freund hinzufügen?")
                Spacer(modifier = Modifier.height(8.dp))
                OutlinedTextField(
                    value = message,
                    onValueChange = { message = it },
                    label = { Text("Nachricht (optional)") },
                    modifier = Modifier.fillMaxWidth()
                )
            }
        },
        confirmButton = {
            TextButton(onClick = onConfirm) {
                Text("Hinzufügen")
            }
        },
        dismissButton = {
            TextButton(onClick = onDismiss) {
                Text("Abbrechen")
            }
        }
    )
} 