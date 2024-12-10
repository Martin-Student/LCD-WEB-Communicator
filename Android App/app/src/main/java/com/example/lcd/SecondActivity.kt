package com.example.lcd
import android.content.Intent
import android.os.Build
import android.os.Bundle
import android.util.Log
import android.widget.Button
import android.widget.EditText
import android.widget.Toast
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AppCompatActivity
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch
import java.net.HttpURLConnection
import java.net.URL
import java.time.LocalDateTime
import java.time.format.DateTimeFormatter
import java.io.OutputStream


class SecondActivity : AppCompatActivity() {

    lateinit var sendBtn: Button
    lateinit var authorInput: EditText
    lateinit var msgInput: EditText
    lateinit var backBtn : Button
    lateinit var msgLogBtn : Button

    @RequiresApi(Build.VERSION_CODES.O)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_second)

        // Pobierz referencje do przycisku i pól tekstowych
        sendBtn = findViewById(R.id.message_btn)
        backBtn = findViewById(R.id.back_btn)
        authorInput = findViewById(R.id.author_input)
        msgInput = findViewById(R.id.msg_input)
        msgLogBtn = findViewById(R.id.msg_log_btn)

        val esp32Address = intent.getStringExtra("ESP32_ADDRESS")
        Log.i("SecondActivity", "Otrzymany adres ESP32: $esp32Address")


        // Funkcja do zapisywania wiadomości w SharedPreferences
        fun saveMessage(author: String, message: String) {
            val sharedPreferences = getSharedPreferences("MessageLogPrefs", MODE_PRIVATE)
            val editor = sharedPreferences.edit()

            // Pobierz istniejące wiadomości
            val existingMessages = sharedPreferences.getString("messages", "") ?: ""
            val newMessage = "$author|$message"

            // Dodaj nową wiadomość do istniejących
            val updatedMessages = if (existingMessages.isNotEmpty()) {
                "$existingMessages;$newMessage"
            } else {
                newMessage
            }

            // Zapisz zaktualizowaną listę wiadomości
            editor.putString("messages", updatedMessages)
            editor.apply()
        }

        sendBtn.setOnClickListener {
            // Zapisanie daty i godziny w chwili kliknięcia
            val currentDateTime: LocalDateTime = LocalDateTime.now()
            // Opcjonalnie: Formatowanie daty na łańcuch
            val formattedDate: String = currentDateTime.format(DateTimeFormatter.ofPattern("HH:mm dd.MM.yyyy"))
            // Wyświetlenie lub zapisanie daty
            Log.d("Date", "Aktualna data i godzina: $formattedDate")
            // DATA jako String
            val savedDateString = formattedDate

            // Pobierz dane z pól tekstowych
            val author_without_date = authorInput.text.toString()
            val message = msgInput.text.toString()

            val author = author_without_date + " (" + savedDateString + ")"

            val esp32Url = esp32Address + "/?text=$message&autor=$author_without_date"

            // Sprawdź, czy teksty nie są puste
            if (author.isNotEmpty() && message.isNotEmpty()) {
                // Przygotuj URL z parametrami GET
                Log.i("SecondActivity", "Otrzymany adres ESP32: $esp32Url")

                // Wywołanie zapytania GET w tle
                GlobalScope.launch(Dispatchers.IO) {
                    val response = sendGetRequest(esp32Url)
                    // Wracamy na główny wątek, żeby pokazać komunikat
                    launch(Dispatchers.Main) {
                        if (response != null) {
                            Toast.makeText(applicationContext, "Message has been sent", Toast.LENGTH_SHORT).show()
                        } else {
                            Toast.makeText(applicationContext, "Error! Try to connect again", Toast.LENGTH_SHORT).show()
                        }
                    }
                }

                // Zapisz wiadomość w SharedPreferences
                saveMessage(author, message)

                msgInput.text.clear() // czyści pole msg

            } else {
                Toast.makeText(applicationContext, "Proszę wypełnić oba pola", Toast.LENGTH_SHORT).show()
            }
        }
        backBtn.setOnClickListener {
            // Tworzymy Intent do przejścia na ThirdActivity
            val intent = Intent(this, MainActivity::class.java)

            // Uruchamiamy nową aktywność
            startActivity(intent)
        }

        msgLogBtn.setOnClickListener {
            // Tworzymy Intent do przejścia na ThirdActivity
            val intent = Intent(this, MessageLogActivity::class.java)

            // Uruchamiamy nową aktywność
            startActivity(intent)
        }
    }

    // Funkcja wysyłająca zapytanie GET
    private fun sendGetRequest(url: String): String? {
        try {
            // Tworzymy obiekt URL
            val urlObj = URL(url)
            val connection = urlObj.openConnection() as HttpURLConnection
            connection.requestMethod = "GET"
            connection.connectTimeout = 5000 // Maksymalny czas oczekiwania (5 sekund)
            connection.readTimeout = 5000

            // Sprawdzamy odpowiedź
            if (connection.responseCode == HttpURLConnection.HTTP_OK) {
                val inputStream = connection.inputStream
                val response = inputStream.bufferedReader().use { it.readText() }
                return response
            } else {
                return null
            }
        } catch (e: Exception) {
            e.printStackTrace()
            return null
        }
    }
}
