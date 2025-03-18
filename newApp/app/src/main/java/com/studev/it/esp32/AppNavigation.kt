package com.studev.it.esp32

import androidx.compose.runtime.Composable
import androidx.lifecycle.viewmodel.compose.viewModel
import androidx.navigation.compose.NavHost
import androidx.navigation.compose.composable
import androidx.navigation.compose.rememberNavController
import com.studev.it.esp32.Screens.ESP32ConnectionScreen
import com.studev.it.esp32.Screens.MessageLogScreen
import com.studev.it.esp32.Screens.MessageScreen
import com.studev.it.esp32.ViewModels.MessageViewModel


@Composable
fun AppNavigation() {
    val navController = rememberNavController()
    val viewModel: MessageViewModel = viewModel() // Tworzenie instancji ViewModel

    NavHost(navController, startDestination = "esp_connection") {
        composable("esp_connection") {
            ESP32ConnectionScreen { navController.navigate("message_screen") }
        }
        composable("message_screen") {
            MessageScreen(viewModel) { navController.navigate("message_log") }
        }
        composable("message_log") {
            MessageLogScreen(viewModel) { navController.popBackStack() }
        }
    }
}
