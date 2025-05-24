package com.stompcoin.app.ui.friends.components

import androidx.compose.foundation.layout.*
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.*
import androidx.compose.material3.*
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import com.stompcoin.app.data.model.FriendStats

@Composable
fun FriendStatsCard(
    stats: FriendStats,
    modifier: Modifier = Modifier
) {
    Card(
        modifier = modifier
            .fillMaxWidth()
            .padding(16.dp)
    ) {
        Column(
            modifier = Modifier
                .fillMaxWidth()
                .padding(16.dp)
        ) {
            Text(
                text = "Freundesstatistiken",
                style = MaterialTheme.typography.titleLarge,
                modifier = Modifier.padding(bottom = 16.dp)
            )

            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.SpaceEvenly
            ) {
                StatItem(
                    icon = Icons.Default.People,
                    value = stats.totalFriends.toString(),
                    label = "Freunde"
                )
                StatItem(
                    icon = Icons.Default.Person,
                    value = stats.activeFriends.toString(),
                    label = "Aktiv"
                )
                StatItem(
                    icon = Icons.Default.Notifications,
                    value = stats.pendingRequests.toString(),
                    label = "Anfragen"
                )
            }

            if (stats.recentActivities.isNotEmpty()) {
                Text(
                    text = "Letzte Aktivitäten",
                    style = MaterialTheme.typography.titleMedium,
                    modifier = Modifier.padding(top = 16.dp, bottom = 8.dp)
                )
                stats.recentActivities.take(3).forEach { activity ->
                    ActivityItem(activity = activity)
                }
            }
        }
    }
}

@Composable
private fun StatItem(
    icon: androidx.compose.ui.graphics.vector.ImageVector,
    value: String,
    label: String
) {
    Column(
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        Icon(
            imageVector = icon,
            contentDescription = null,
            modifier = Modifier.size(24.dp)
        )
        Text(
            text = value,
            style = MaterialTheme.typography.titleMedium
        )
        Text(
            text = label,
            style = MaterialTheme.typography.bodySmall
        )
    }
}

@Composable
private fun ActivityItem(activity: com.stompcoin.app.data.model.FriendActivity) {
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .padding(vertical = 4.dp),
        verticalAlignment = Alignment.CenterVertically
    ) {
        Icon(
            imageVector = when (activity.type) {
                com.stompcoin.app.data.model.ActivityType.STEPS_COMPLETED -> Icons.Default.DirectionsWalk
                com.stompcoin.app.data.model.ActivityType.CHALLENGE_WON -> Icons.Default.EmojiEvents
                com.stompcoin.app.data.model.ActivityType.EVENT_ATTENDED -> Icons.Default.Event
                com.stompcoin.app.data.model.ActivityType.ACHIEVEMENT_UNLOCKED -> Icons.Default.Star
                com.stompcoin.app.data.model.ActivityType.COINS_EARNED -> Icons.Default.MonetizationOn
            },
            contentDescription = null,
            modifier = Modifier.size(20.dp)
        )
        Spacer(modifier = Modifier.width(8.dp))
        Text(
            text = activity.description,
            style = MaterialTheme.typography.bodySmall
        )
    }
} 