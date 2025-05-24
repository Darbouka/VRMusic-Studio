package com.stompcoin.app.ui.friends

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
import androidx.hilt.navigation.compose.hiltViewModel
import com.stompcoin.app.data.model.Friend
import com.stompcoin.app.data.model.FriendActivity
import com.stompcoin.app.data.model.FriendRequest
import com.stompcoin.app.data.model.User
import com.stompcoin.app.ui.friends.components.FriendStatsCard
import com.stompcoin.app.ui.friends.components.UserSearchResults
import java.time.format.DateTimeFormatter

@Composable
fun FriendsScreen(
    viewModel: FriendsViewModel = hiltViewModel(),
    onNavigateToProfile: (String) -> Unit
) {
    val uiState by viewModel.uiState.collectAsState()
    var showSearchBar by remember { mutableStateOf(false) }
    var searchQuery by remember { mutableStateOf("") }

    Scaffold(
        topBar = {
            TopAppBar(
                title = { Text("Freunde") },
                actions = {
                    IconButton(onClick = { showSearchBar = !showSearchBar }) {
                        Icon(Icons.Default.Search, contentDescription = "Suchen")
                    }
                }
            )
        }
    ) { paddingValues ->
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(paddingValues)
        ) {
            if (showSearchBar) {
                SearchBar(
                    query = searchQuery,
                    onQueryChange = { 
                        searchQuery = it
                        viewModel.searchUsers(it)
                    },
                    modifier = Modifier.fillMaxWidth()
                )
                
                if (searchQuery.isNotEmpty()) {
                    UserSearchResults(
                        users = uiState.searchResults,
                        onUserClick = { onNavigateToProfile(it.id) },
                        onAddFriend = { viewModel.sendFriendRequest(it.id) },
                        modifier = Modifier.weight(1f)
                    )
                }
            } else {
                if (uiState.isLoading) {
                    Box(
                        modifier = Modifier.fillMaxSize(),
                        contentAlignment = Alignment.Center
                    ) {
                        CircularProgressIndicator()
                    }
                } else {
                    LazyColumn(
                        modifier = Modifier.fillMaxSize(),
                        verticalArrangement = Arrangement.spacedBy(8.dp)
                    ) {
                        uiState.friendStats?.let { stats ->
                            item {
                                FriendStatsCard(stats = stats)
                            }
                        }

                        if (uiState.pendingRequests.isNotEmpty()) {
                            item {
                                Text(
                                    "Ausstehende Anfragen",
                                    style = MaterialTheme.typography.titleMedium,
                                    modifier = Modifier.padding(16.dp)
                                )
                            }
                            items(uiState.pendingRequests) { request ->
                                FriendRequestItem(
                                    request = request,
                                    onAccept = { viewModel.acceptFriendRequest(request.id) },
                                    onReject = { viewModel.rejectFriendRequest(request.id) }
                                )
                            }
                        }

                        item {
                            Text(
                                "Freunde",
                                style = MaterialTheme.typography.titleMedium,
                                modifier = Modifier.padding(16.dp)
                            )
                        }
                        items(uiState.friends) { friend ->
                            FriendItem(
                                friend = friend,
                                onProfileClick = { onNavigateToProfile(friend.friendId) },
                                onRemove = { viewModel.removeFriend(friend.id) }
                            )
                        }

                        if (uiState.friendActivities.isNotEmpty()) {
                            item {
                                Text(
                                    "Aktivitäten",
                                    style = MaterialTheme.typography.titleMedium,
                                    modifier = Modifier.padding(16.dp)
                                )
                            }
                            items(uiState.friendActivities) { activity ->
                                FriendActivityItem(activity = activity)
                            }
                        }
                    }
                }
            }

            if (uiState.error != null) {
                Snackbar(
                    modifier = Modifier.padding(16.dp),
                    action = {
                        TextButton(onClick = { viewModel.clearError() }) {
                            Text("OK")
                        }
                    }
                ) {
                    Text(uiState.error)
                }
            }
        }
    }
}

@Composable
fun FriendItem(
    friend: Friend,
    onProfileClick: () -> Unit,
    onRemove: () -> Unit
) {
    Card(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp)
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
                IconButton(onClick = onProfileClick) {
                    Icon(Icons.Default.Person, contentDescription = "Profil")
                }
                Spacer(modifier = Modifier.width(8.dp))
                Text(
                    text = friend.friendId,
                    style = MaterialTheme.typography.bodyLarge
                )
            }
            IconButton(onClick = onRemove) {
                Icon(Icons.Default.Delete, contentDescription = "Entfernen")
            }
        }
    }
}

@Composable
fun FriendRequestItem(
    request: FriendRequest,
    onAccept: () -> Unit,
    onReject: () -> Unit
) {
    Card(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp)
    ) {
        Column(
            modifier = Modifier
                .fillMaxWidth()
                .padding(16.dp)
        ) {
            Text(
                text = "Anfrage von ${request.senderId}",
                style = MaterialTheme.typography.bodyLarge
            )
            request.message?.let {
                Text(
                    text = it,
                    style = MaterialTheme.typography.bodyMedium,
                    maxLines = 2,
                    overflow = TextOverflow.Ellipsis
                )
            }
            Row(
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(top = 8.dp),
                horizontalArrangement = Arrangement.End
            ) {
                TextButton(onClick = onReject) {
                    Text("Ablehnen")
                }
                Spacer(modifier = Modifier.width(8.dp))
                Button(onClick = onAccept) {
                    Text("Akzeptieren")
                }
            }
        }
    }
}

@Composable
fun FriendActivityItem(activity: FriendActivity) {
    Card(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 16.dp)
    ) {
        Row(
            modifier = Modifier
                .fillMaxWidth()
                .padding(16.dp),
            verticalAlignment = Alignment.CenterVertically
        ) {
            Icon(
                imageVector = when (activity.type) {
                    ActivityType.STEPS_COMPLETED -> Icons.Default.DirectionsWalk
                    ActivityType.CHALLENGE_WON -> Icons.Default.EmojiEvents
                    ActivityType.EVENT_ATTENDED -> Icons.Default.Event
                    ActivityType.ACHIEVEMENT_UNLOCKED -> Icons.Default.Star
                    ActivityType.COINS_EARNED -> Icons.Default.MonetizationOn
                },
                contentDescription = null
            )
            Spacer(modifier = Modifier.width(16.dp))
            Column {
                Text(
                    text = activity.description,
                    style = MaterialTheme.typography.bodyMedium
                )
                Text(
                    text = activity.timestamp.format(DateTimeFormatter.ofPattern("dd.MM.yyyy HH:mm")),
                    style = MaterialTheme.typography.bodySmall
                )
            }
        }
    }
}

@Composable
fun SearchBar(
    query: String,
    onQueryChange: (String) -> Unit,
    modifier: Modifier = Modifier
) {
    OutlinedTextField(
        value = query,
        onValueChange = onQueryChange,
        modifier = modifier.padding(16.dp),
        placeholder = { Text("Freunde suchen...") },
        leadingIcon = {
            Icon(Icons.Default.Search, contentDescription = "Suchen")
        },
        singleLine = true
    )
} 