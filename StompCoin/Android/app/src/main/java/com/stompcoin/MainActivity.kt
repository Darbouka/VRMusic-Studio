package com.stompcoin

import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.*
import androidx.compose.material.*
import androidx.compose.runtime.*
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import androidx.navigation.compose.rememberNavController
import com.stompcoin.ui.theme.StompCoinTheme
import com.stompcoin.navigation.AppNavigation
import com.stompcoin.viewmodels.AppViewModel
import androidx.drawerlayout.widget.DrawerLayout
import androidx.navigation.ui.AppBarConfiguration
import androidx.navigation.ui.navigateUp
import androidx.navigation.ui.setupActionBarWithNavController
import androidx.navigation.ui.setupWithNavController
import com.google.android.material.navigation.NavigationView
import androidx.navigation.fragment.NavHostFragment

class MainActivity : ComponentActivity() {
    private lateinit var drawerLayout: DrawerLayout
    private lateinit var appBarConfiguration: AppBarConfiguration
    private lateinit var navController: NavHostFragment

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        // Toolbar einrichten
        val toolbar = findViewById<MaterialToolbar>(R.id.topAppBar)
        setSupportActionBar(toolbar)

        // Navigation einrichten
        drawerLayout = findViewById(R.id.drawerLayout)
        navController = supportFragmentManager.findFragmentById(R.id.nav_host_fragment) as NavHostFragment
        val navController = navController.navController

        // Navigation Drawer einrichten
        val navigationView = findViewById<NavigationView>(R.id.navigationView)
        navigationView.setupWithNavController(navController)

        // AppBar-Konfiguration
        appBarConfiguration = AppBarConfiguration(
            setOf(R.id.nav_home, R.id.nav_music, R.id.nav_leaderboard, R.id.nav_settings),
            drawerLayout
        )

        // ActionBar mit Navigation Controller verbinden
        setupActionBarWithNavController(navController, appBarConfiguration)
    }

    override fun onSupportNavigateUp(): Boolean {
        val navController = navController.navController
        return navController.navigateUp(appBarConfiguration) || super.onSupportNavigateUp()
    }
} 