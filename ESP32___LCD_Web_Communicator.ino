#include "pitches.h"
#include <WiFi.h>
#include <LiquidCrystal.h>

#define BUZZER_PIN 2 // GŁOŚNIK
#define BUTTON_PIN 4 // SWITCH GŁOŚNIKA
#define BUTTON_SCREEN_PIN 12 // CZYSZCZENIE EKRANU
#define BUTTON_PREVIOUS_SCREEN_PIN 17 // WRACANIE DO POPRZEDNIEJ WIADOMOSCI

const char* ssid = "UPC6217263";
const char* password = "ppcFTvn6hbd7";

const int rs = 22;  // RS do GPIO22
const int en = 23;  // EN do GPIO23
const int d4 = 18;  // D4 do GPIO18
const int d5 = 19;  // D5 do GPIO19
const int d6 = 21;  // D6 do GPIO21
const int d7 = 16;  // D7 do GPIO16
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

WiFiServer server(80);

String tekst = "";  // Zmienna do przechowywania tekstu
String autor = ""; // Zmienna do przechowywania autora tekstu 
String previous_tekst = "Brak wiadomosci!";

unsigned long startMillis = 0;
const unsigned long interval = 10000;

int melody[] = {
  NOTE_G4, NOTE_F4, NOTE_E4, NOTE_F4, NOTE_G4, NOTE_A4, NOTE_B4,
  NOTE_A4, NOTE_G4, NOTE_F4, NOTE_G4, NOTE_E4
};

int melody_msg[] = {
  NOTE_E5, NOTE_D5, NOTE_FS4, NOTE_GS4, 
  NOTE_CS5, NOTE_B4, NOTE_D4, NOTE_E4, 
  NOTE_B4, NOTE_A4, NOTE_CS4, NOTE_E4,
  NOTE_A4
};

int durations[] = {
  8, 8, 8, 8, 8, 8, 8,
  8, 8, 8, 8, 4
};

int durations_msg[] = {
  8, 8, 4, 4,
  8, 8, 4, 4,
  8, 8, 4, 4,
  2
};

bool silent_mode = false;
bool buttonState = false;  // Zmienna do przechowywania stanu przycisku
bool lastButtonState = false;  // Zmienna do monitorowania poprzedniego stanu przycisku
bool lastScreenButtonState = false;
bool msg_on_screen = false;

// Funkcja do zamiany polskich znaków na odpowiedniki bez ogonków
String removePolishChars(String str) {
  str.replace("ą", "a");
  str.replace("ę", "e");
  str.replace("ć", "c");
  str.replace("ł", "l");
  str.replace("ó", "o");
  str.replace("ś", "s");
  str.replace("ż", "z");
  str.replace("ź", "z");
  str.replace("Ą", "A");
  str.replace("Ę", "E");
  str.replace("Ć", "C");
  str.replace("Ł", "L");
  str.replace("Ó", "O");
  str.replace("Ś", "S");
  str.replace("Ż", "Z");
  str.replace("Ź", "Z");
  return str;
}

// Funkcja do dekodowania URL (przekształcanie %20 na spację itd.)
String urlDecode(String str) {
  String decodedStr = "";
  char currentChar;
  int i = 0;
  
  while (i < str.length()) {
    currentChar = str.charAt(i);
    if (currentChar == '%') {
      String hexValue = str.substring(i + 1, i + 3);
      decodedStr += (char) strtol(hexValue.c_str(), NULL, 16);
      i += 3;
    } else if (currentChar == '+') {
      decodedStr += ' ';
      i++;
    } else {
      decodedStr += currentChar;
      i++;
    }
  }
  return decodedStr;
}

void setup() {
  Serial.begin(115200); // Uruchomienie monitora szeregowego
  // Konfiguracja BUZZERA na PIN 2 oraz BUTTONA na PIN 4
  pinMode(BUZZER_PIN, OUTPUT); 
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUTTON_SCREEN_PIN, INPUT_PULLUP);
  pinMode(BUTTON_PREVIOUS_SCREEN_PIN, INPUT_PULLUP);

  
  // Konfiguracja wyświetlacza LCD
  lcd.begin(16, 2);
  lcd.print("Laczenie z WiFi");

  // Ustawienie nazwy hosta przed połączeniem z WiFi
  WiFi.setHostname("ESP32");
  WiFi.begin(ssid, password);

  // Próby połączenia z WiFi
  int retryCount = 0; // Licznik prób połączenia
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.setCursor(retryCount % 16, 1);
    lcd.print(".");
    retryCount++;
    if (retryCount > 20) {
      lcd.clear();
      lcd.print("Nie polaczono");
      lcd.setCursor(0, 1);
      lcd.print("WiFi!");
      delay(5000);
      retryCount = 0;
      lcd.clear();
      lcd.print("Laczenie z WiFi");
    }
  }
  
  // Po pomyślnym połączeniu
  lcd.clear();
  lcd.print("Polaczono WiFi!");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP()); // Wyświetl adres IP ESP32
  
  Serial.println("Polaczono z siecia WiFi");
  Serial.print("Adres IP: ");
  Serial.println(WiFi.localIP());

  server.begin(); // Rozpoczęcie nasłuchu na porcie 80
  Serial.println("Serwer uruchomiony, czekam na połączenie...");

  if (WiFi.status() == WL_CONNECTED and silent_mode == false) {
    int size = sizeof(durations) / sizeof(int);
    for (int note = 0; note < size; note++) {
      //to calculate the note duration, take one second divided by the note type.
      //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int duration = 1000 / durations[note];
      tone(BUZZER_PIN, melody[note], duration);
  
      //to distinguish the notes, set a minimum time between them.
      //the note's duration + 30% seems to work well:
      int pauseBetweenNotes = duration * 1.30;
      delay(pauseBetweenNotes);
      
      //stop the tone playing:
      noTone(BUZZER_PIN);
  }
  }
}

// Funkcja do przewijania tekstu na LCD
void scrollText(String text) {
  int textLength = text.length();
  
  // Jeśli tekst jest dłuższy niż 16 znaków, przewijamy go
  if (textLength > 16) {
    
    // Zaczynamy przewijanie
    for (int i = 0; i <= textLength - 16; i++) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Autor: " + autor);
      lcd.setCursor(0, 1);
      lcd.print(text.substring(i, i + 16));  // Wyświetlamy fragment tekstu
      delay(500);  // Czas trwania przesunięcia
      // Przewijanie tekstu w lewo
      //lcd.scrollDisplayLeft();
    }
  } else {
    // Jeśli tekst jest krótszy niż 16 znaków, wyświetlamy go na raz
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Autor: " + autor);
    lcd.setCursor(0, 1);
    lcd.print(text);
  }
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - startMillis >= interval && msg_on_screen == false) {
    lcd.clear();  // Wyczyść LCD przed wyświetleniem nowego tekstu
    lcd.setCursor(0, 0);  // Ustaw kursor na początek pierwszej linii
    lcd.print(".... Czekam ....");  // Wyświetl część tekstu
    lcd.setCursor(0, 1);  // Ustaw kursor na początek drugiej linii
    lcd.print("..na wiadomosc..");  // Wyświetl pozostałą część tekstu
    startMillis = currentTime;
  }

  bool currentButtonState = digitalRead(BUTTON_PIN);  // Odczyt stanu przycisku
  bool currentScreenButtonState = digitalRead(BUTTON_SCREEN_PIN);  // Odczyt stanu przycisku

  // Obsługa przycisku zmiany trybu
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    buttonState = !buttonState;
    silent_mode = buttonState;
    Serial.println(buttonState ? "Silent Mode: TRUE" : "Silent Mode: FALSE");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tryb dzwieku: ");
    lcd.setCursor(0, 1);
    lcd.print(buttonState ? "WYCISZONY" : "GLOSNY");
    delay(200);
    msg_on_screen = false;
  }
  lastButtonState = currentButtonState;

  // Obsługa przycisku czyszczenia ekranu
  if (currentScreenButtonState == LOW && lastScreenButtonState == HIGH) {
    previous_tekst = tekst;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(".... Czekam ....");
    lcd.setCursor(0, 1);
    lcd.print("..na wiadomosc..");
    Serial.println("Ekran został wyczyszczony!");
    Serial.println("Poprzednia wiadomość: " + previous_tekst);
    delay(200);
    msg_on_screen = false;
  }
  lastScreenButtonState = currentScreenButtonState;

  // Obsługa przycisku poprzedniego tekstu
  if (digitalRead(BUTTON_PREVIOUS_SCREEN_PIN) == LOW) {
    if (previous_tekst.length() > 16) {
    // Zaczynamy przewijanie
      for (int i = 0; i <= previous_tekst.length() - 16; i++) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Poprzedni tekst: ");
        lcd.setCursor(0, 1);
        lcd.print(previous_tekst.substring(i, i + 16));  // Wyświetlamy fragment tekstu
        delay(500);  // Czas trwania przesunięcia
        msg_on_screen = false;

        // Przewijanie tekstu w lewo
        //lcd.scrollDisplayLeft();
      }
    } else {
      // Jeśli tekst jest krótszy niż 16 znaków, wyświetlamy go na raz
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Poprzedni tekst: ");
      lcd.setCursor(0, 1);
      lcd.print(previous_tekst);
      delay(200);
      msg_on_screen = false;
  }
  }

  // Obsługa klienta WiFi
  WiFiClient client = server.available();
  if (client) {
    String currentLine = "";  // Zmienna do przechowywania linii zapytania
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        currentLine += c;

        if (c == '\n') {  // Koniec nagłówków HTTP
          if (currentLine.indexOf("GET /?text=") >= 0) {
            int textStart = currentLine.indexOf("text=") + 5;
            int textEnd = currentLine.indexOf(" HTTP/");
            int ampersandPos = currentLine.indexOf('&', textStart);
            if (ampersandPos > 0 && ampersandPos < textEnd) {
              textEnd = ampersandPos;
            }
            tekst = currentLine.substring(textStart, textEnd);
            tekst = urlDecode(tekst);
            tekst = removePolishChars(tekst);

            if (currentLine.indexOf("&autor=") >= 0) {
              int autorStart = currentLine.indexOf("autor=") + 6;
              int autorEnd = currentLine.indexOf(" HTTP/");
              int ampersandPosAutor = currentLine.indexOf('&', autorStart);
              if (ampersandPosAutor > 0 && ampersandPosAutor < autorEnd) {
                autorEnd = ampersandPosAutor;
              }
              autor = currentLine.substring(autorStart, autorEnd);
              autor = urlDecode(autor);
              autor = removePolishChars(autor);
              if (autor.length() == 0) {
                autor = "Nieznany";
              }
            }

            Serial.println("Tekst: " + tekst);
            Serial.println("Autor: " + autor);

            if (!silent_mode) {
              int size = sizeof(durations_msg) / sizeof(int);
              for (int note = 0; note < size; note++) {
                int duration = 1000 / durations_msg[note];
                tone(BUZZER_PIN, melody_msg[note], duration);
                int pauseBetweenNotes = duration * 1.30;
                delay(pauseBetweenNotes);
                noTone(BUZZER_PIN);
              }
            }

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Autor: " + autor);
            lcd.setCursor(0, 1);
            scrollText(tekst);  // Wywołanie funkcji do przewijania tekstu
            msg_on_screen = true;

          }

          // Odpowiedź HTTP
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html; charset=UTF-8");
          client.println("Connection: close");
          client.println();
          client.println("<!DOCTYPE html><html lang=\"pl\">");
          client.println("<head>");
          client.println("<meta charset=\"UTF-8\">");
          client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
          client.println("<title>ESP32 LCD - Wyślij Tekst</title>");
          client.println("<style>");
          client.println("body { font-family: 'Arial', sans-serif; background-color: #f0f4f8; color: #333; margin: 0; padding: 0; display: flex; justify-content: center; align-items: center; height: 100vh;}");
          client.println("h1 { font-size: 2.5em; color: #4CAF50; text-align: center; margin-bottom: 20px; }");
          client.println("h2 { font-size: 1.8em; color: #555; margin-top: 20px; }");
          client.println("p { font-size: 1.2em; color: #666; line-height: 1.6; text-align: center; }");
          client.println("form { background-color: #ffffff; padding: 20px; border-radius: 8px; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); max-width: 400px; width: 100%;}");
          client.println("input[type='text'] { width: 100%; padding: 10px; margin: 10px 0; border: 1px solid #ddd; border-radius: 5px; font-size: 1em; }");
          client.println("input[type='submit'] { background-color: #4CAF50; color: white; border: none; padding: 10px 20px; border-radius: 5px; font-size: 1.2em; cursor: pointer; transition: background-color 0.3s ease;}");
          client.println("input[type='submit']:hover { background-color: #45a049; }");
          client.println("</style>");
          client.println("</head>");
          client.println("<body>");
          client.println("<div>");
          client.println("<h1>Wyślij tekst na LCD</h1>");
          client.println("<form action=\"/\" method=\"GET\">");
          client.println("<label for=\"text\">Tekst:</label>");
          client.println("<input type=\"text\" name=\"text\" id=\"text\" placeholder=\"Wpisz tekst na LCD\">");
          client.println("<label for=\"autor\">Autor:</label>");
          client.println("<input type=\"text\" name=\"autor\" id=\"autor\" placeholder=\"Wpisz autora\">");
          client.println("<input type=\"submit\" value=\"Wyślij\">");
          client.println("</form>");
          client.println("<h2>Otrzymany tekst:</h2>");
          client.println("<p>" + tekst + "</p>");
          client.println("<h2>Autor:</h2>");
          client.println("<p>" + autor + "</p>");
          client.println("</div>");
          client.println("</body>");
          client.println("</html>");
          break;

        }
      }
    }
    client.stop();  // Zamknięcie połączenia
  }
}
