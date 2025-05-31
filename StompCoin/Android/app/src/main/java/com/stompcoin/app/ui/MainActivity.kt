package com.stompcoin.app.ui

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.*
import androidx.compose.material.*
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import androidx.navigation.compose.rememberNavController
import com.stompcoin.app.ui.theme.StompCoinTheme
import dagger.hilt.android.AndroidEntryPoint

@AndroidEntryPoint
class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContent {
            StompCoinTheme {
                val navController = rememberNavController()
                var selectedTab by remember { mutableStateOf(0) }
                
                Scaffold(
                    topBar = {
                        TopAppBar(
                            title = { Text("StompCoin") },
                            backgroundColor = MaterialTheme.colors.primary
                        )
                    },
                    bottomBar = {
                        BottomNavigation {
                            BottomNavigationItem(
                                icon = { Icon(Icons.Default.Home, contentDescription = "Home") },
                                label = { Text("Home") },
                                selected = selectedTab == 0,
                                onClick = { selectedTab = 0 }
                            )
                            BottomNavigationItem(
                                icon = { Icon(Icons.Default.MusicNote, contentDescription = "Music") },
                                label = { Text("Music") },
                                selected = selectedTab == 1,
                                onClick = { selectedTab = 1 }
                            )
                            BottomNavigationItem(
                                icon = { Icon(Icons.Default.Leaderboard, contentDescription = "Leaderboard") },
                                label = { Text("Leaderboard") },
                                selected = selectedTab == 2,
                                onClick = { selectedTab = 2 }
                            )
                            BottomNavigationItem(
                                icon = { Icon(Icons.Default.Settings, contentDescription = "Settings") },
                                label = { Text("Settings") },
                                selected = selectedTab == 3,
                                onClick = { selectedTab = 3 }
                            )
                        }
                    }
                ) { paddingValues ->
                    Box(modifier = Modifier.padding(paddingValues)) {
                        when (selectedTab) {
                            0 -> HomeScreen()
                            1 -> MusicGenreScreen()
                            2 -> LeaderboardScreen()
                            3 -> SettingsScreen()
                        }
                    }
                }
            }
        }
    }
} 