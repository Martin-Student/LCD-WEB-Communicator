package com.example.lcd

import android.content.Intent
import android.os.Bundle
import android.widget.Button
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView

class MessageLogActivity : AppCompatActivity() {

    lateinit var backBtn: Button
    lateinit var deleteBtn: Button

    // Lista wpisów logu
    private val logEntries = mutableListOf<LogEntry>()

    // RecyclerView oraz jego adapter
    private lateinit var recyclerView: RecyclerView
    private lateinit var logAdapter: LogAdapter

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_message_log)

        backBtn = findViewById(R.id.back_btn)
        deleteBtn = findViewById(R.id.delete_btn)

        // Inicjalizacja RecyclerView
        recyclerView = findViewById(R.id.recyclerView)
        logAdapter = LogAdapter(logEntries)
        recyclerView.layoutManager = LinearLayoutManager(this)
        recyclerView.adapter = logAdapter

        // Załaduj wiadomości z SharedPreferences
        loadMessages()

        // Ustawienie przycisku powrotu
        backBtn.setOnClickListener {
            // Przejście z powrotem do SecondActivity
            val intent = Intent(this, SecondActivity::class.java)
            startActivity(intent)
        }
        deleteBtn.setOnClickListener {
            logEntries.clear()
            logAdapter.notifyDataSetChanged()

            // Wyczyszczenie zapisanych danych w SharedPreferences
            val sharedPreferences = getSharedPreferences("MessageLogPrefs", MODE_PRIVATE)
            val editor = sharedPreferences.edit()
            editor.clear()
            editor.apply()
            Toast.makeText(applicationContext, "Messages has been removed", Toast.LENGTH_SHORT).show()
        }
    }

    // Funkcja ładowania wiadomości z SharedPreferences
    private fun loadMessages() {
        val sharedPreferences = getSharedPreferences("MessageLogPrefs", MODE_PRIVATE)
        val savedMessages = sharedPreferences.getString("messages", "") ?: ""

        if (savedMessages.isNotEmpty()) {
            val messageList = savedMessages.split(";")
            logEntries.clear()

            for (entry in messageList) {
                val parts = entry.split("|")
                if (parts.size == 2) {
                    val author = parts[0]
                    val message = parts[1]
                    logEntries.add(LogEntry(author, message))
                }
            }

            // Ograniczenie do ostatnich 20 elementów
            while (logEntries.size > 20) {
                logEntries.removeAt(0)
            }

            logAdapter.notifyDataSetChanged()
        }
    }
}

// Klasa danych reprezentująca pojedynczy wpis w logu
data class LogEntry(val author: String, val message: String)

// Adapter dla RecyclerView
class LogAdapter(private val logEntries: List<LogEntry>) :
    RecyclerView.Adapter<LogAdapter.LogViewHolder>() {

    // ViewHolder przechowuje widoki dla pojedynczego elementu listy
    class LogViewHolder(view: android.view.View) : RecyclerView.ViewHolder(view) {
        val authorTextView: android.widget.TextView = view.findViewById(R.id.authorTextView)
        val messageTextView: android.widget.TextView = view.findViewById(R.id.messageTextView)
    }

    override fun onCreateViewHolder(parent: android.view.ViewGroup, viewType: Int): LogViewHolder {
        val view = android.view.LayoutInflater.from(parent.context)
            .inflate(R.layout.item_log, parent, false)
        return LogViewHolder(view)
    }

    override fun onBindViewHolder(holder: LogViewHolder, position: Int) {
        val entry = logEntries[position]
        holder.authorTextView.text = entry.author
        holder.messageTextView.text = entry.message
    }

    override fun getItemCount() = logEntries.size
}
