package com.studev.it.esp32.ViewModels

import androidx.lifecycle.ViewModel
import androidx.compose.runtime.mutableStateListOf
import androidx.compose.runtime.getValue
import androidx.compose.runtime.setValue

class MessageViewModel : ViewModel() {
    var messages = mutableStateListOf<Pair<String, String>>() // Lista par: (author, message)
        private set

    fun addMessage(author: String, message: String) {
        messages.add(author to message)
    }

    fun clearMessages() {
        messages.clear()
    }
}
