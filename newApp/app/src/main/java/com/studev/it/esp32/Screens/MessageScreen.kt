package com.studev.it.esp32.Screens

import androidx.compose.runtime.Composable
import androidx.compose.runtime.remember
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.Button
import androidx.compose.material3.ButtonDefaults
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.LocalTextStyle
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.material3.TextField
import androidx.compose.material3.TextFieldDefaults
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import com.studev.it.esp32.R
import androidx.compose.runtime.setValue
import androidx.compose.ui.draw.scale
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.text.input.TextFieldValue
import com.studev.it.esp32.ViewModels.MessageViewModel

@Composable
fun MessageScreen(viewModel: MessageViewModel, onNavigateToLog: () -> Unit) {
    var author by remember { mutableStateOf(TextFieldValue()) }
    var message by remember { mutableStateOf(TextFieldValue()) }

    Box(
        modifier = Modifier
            .fillMaxSize()
            .background(Color.Black)
            .padding(16.dp),
        contentAlignment = Alignment.Center
    ) {
        Column(
            horizontalAlignment = Alignment.CenterHorizontally,
            verticalArrangement = Arrangement.Center,
            modifier = Modifier.fillMaxWidth()
        ) {
            Image(
                painter = painterResource(id = R.drawable.esp_msg),
                contentDescription = "Logo",
                modifier = Modifier.size(160.dp)
            )
            Spacer(modifier = Modifier.height(24.dp))

            Text("Author:", fontSize = 18.sp, color = Color.White)
            TextField(
                value = author,
                onValueChange = { author = it },
                placeholder = { Text("Enter the author of the message here", color = Color.Gray) },
                modifier = Modifier.fillMaxWidth(),
                singleLine = true,
                shape = RoundedCornerShape(8.dp)
            )

            Spacer(modifier = Modifier.height(24.dp))

            Text("Message to send:", fontSize = 18.sp, color = Color.White)
            TextField(
                value = message,
                onValueChange = { message = it },
                placeholder = { Text("Enter your message here", color = Color.Gray) },
                modifier = Modifier.fillMaxWidth(),
                shape = RoundedCornerShape(8.dp)
            )

            Spacer(modifier = Modifier.height(32.dp))

            Button(
                onClick = {
                    viewModel.addMessage(author.text, message.text)
                    author = TextFieldValue()
                    message = TextFieldValue()
                },
                modifier = Modifier.fillMaxWidth()
            ) {
                Text("Send Message")
            }

            Spacer(modifier = Modifier.height(16.dp))

            Button(
                onClick = { onNavigateToLog() },
                modifier = Modifier.fillMaxWidth()
            ) {
                Text("Messages Log")
            }
        }
    }
}
