package com.example.lcd
import android.content.Intent
import android.net.Uri
import android.os.Bundle
import android.widget.EditText
import android.widget.Button
import android.util.Log
import android.widget.ImageView
import android.widget.Toast
import androidx.activity.enableEdgeToEdge
import androidx.appcompat.app.AppCompatActivity


class MainActivity : AppCompatActivity() {

    lateinit var esp32AdressInput: EditText
    lateinit var connectBtn: Button
    lateinit var gitHubLogo : ImageView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContentView(R.layout.activity_main)

        esp32AdressInput = findViewById(R.id.esp32_adress_input)
        connectBtn = findViewById(R.id.connect_btn)
        gitHubLogo = findViewById(R.id.github_logo)

        connectBtn.setOnClickListener {
            val esp32_adress = esp32AdressInput.text.toString()
            Log.i("MainActivity", "Adres ESP32: $esp32_adress")
            if (esp32_adress.isNotEmpty()) {
                // Tworzenie Intentu
                val intent = Intent(this, SecondActivity::class.java)

                // Dodawanie danych do Intentu
                intent.putExtra("ESP32_ADDRESS", esp32_adress)

                // Przełączanie aktywności
                startActivity(intent)
            }
            else {
                Toast.makeText(applicationContext, "Enter ESP adress!", Toast.LENGTH_SHORT).show()
            }
        }
        gitHubLogo.setOnClickListener {
            val url = "https://github.com/Martin-Student"  // Podaj tutaj link, który ma zostać otwarty
            val intent = Intent(Intent.ACTION_VIEW, Uri.parse(url))
            startActivity(intent)

        }
    }
}
