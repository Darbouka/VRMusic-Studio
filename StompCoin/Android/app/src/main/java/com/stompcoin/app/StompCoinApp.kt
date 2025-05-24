package com.stompcoin.app

import androidx.compose.runtime.Composable
import androidx.navigation.compose.rememberNavController
import com.stompcoin.app.navigation.StompCoinNavigation

@Composable
fun StompCoinApp() {
    val navController = rememberNavController()
    StompCoinNavigation(navController = navController)
} 