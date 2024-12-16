package com.example.lcd

import android.content.Intent
import android.net.Uri
import android.os.Bundle
import android.widget.EditText
import android.widget.Button
import android.util.Log
import android.widget.ImageView
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.Response
import java.io.IOException
import kotlin.concurrent.thread
import android.util.Patterns

class MainActivity : AppCompatActivity() {

    lateinit var esp32AdressInput: EditText
    lateinit var connectBtn: Button
    lateinit var gitHubLogo: ImageView

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)

        esp32AdressInput = findViewById(R.id.esp32_adress_input)
        connectBtn = findViewById(R.id.connect_btn)
        gitHubLogo = findViewById(R.id.github_logo)

        connectBtn.setOnClickListener {
            val esp32Address = esp32AdressInput.text.toString()
            Log.i("MainActivity", "Adres ESP32: $esp32Address")

            if (esp32Address.isNotEmpty()) {
                // Sprawdzamy, czy adres ma poprawny format URL
                if (isValidUrl(esp32Address)) {
                    // Sprawdzamy, czy strona jest online
                    checkIfOnline(esp32Address) { isOnline ->
                        if (isOnline) {
                            // Tworzenie Intentu
                            val intent = Intent(this, SecondActivity::class.java)
                            intent.putExtra("ESP32_ADDRESS", esp32Address)
                            startActivity(intent)
                        } else {
                            Toast.makeText(
                                applicationContext,
                                "Nie można połączyć się z ESP32",
                                Toast.LENGTH_SHORT
                            ).show()
                        }
                    }
                } else {
                    Toast.makeText(
                        applicationContext,
                        "Niepoprawny format adresu",
                        Toast.LENGTH_SHORT
                    ).show()
                }
            } else {
                Toast.makeText(applicationContext, "Wpisz adres ESP32!", Toast.LENGTH_SHORT).show()
            }
        }

        gitHubLogo.setOnClickListener {
            val url = "https://github.com/Martin-Student"
            val intent = Intent(Intent.ACTION_VIEW, Uri.parse(url))
            startActivity(intent)
        }
    }

    // Funkcja sprawdzająca, czy adres URL jest poprawny
    private fun isValidUrl(url: String): Boolean {
        return Patterns.WEB_URL.matcher(url).matches()
    }

    // Funkcja sprawdzająca, czy strona jest online
    private fun checkIfOnline(url: String, callback: (Boolean) -> Unit) {
        thread {
            val client = OkHttpClient()
            val request = Request.Builder()
                .url(url)
                .head() // Wysyłamy zapytanie HEAD, aby sprawdzić status
                .build()

            try {
                val response: Response = client.newCall(request).execute()
                // Sprawdzamy, czy odpowiedź jest pozytywna (2xx oznacza sukces)
                val isOnline = response.isSuccessful

                // Zwracamy wynik na główny wątek
                runOnUiThread {
                    callback(isOnline)
                }
            } catch (e: IOException) {
                // W przypadku błędu (np. brak połączenia) informujemy użytkownika
                runOnUiThread {
                    Toast.makeText(
                        applicationContext,
                        "Błąd połączenia z serwerem",
                        Toast.LENGTH_SHORT
                    ).show()
                    callback(false)
                }
            }
        }
    }
}