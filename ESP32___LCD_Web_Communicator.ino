#include <WiFi.h>
#include <LiquidCrystal.h>

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
  
  // Konfiguracja wyświetlacza LCD
  lcd.begin(16, 2);
  lcd.print("Laczenie z WiFi");

  // Opcjonalna konfiguracja statycznego IP (odkomentuj, jeśli chcesz użyć)
  /*
  IPAddress local_IP(192, 168, 1, 184); // Przykładowy adres IP
  IPAddress gateway(192, 168, 1, 1);    // Brama domyślna
  IPAddress subnet(255, 255, 255, 0);   // Maska podsieci
  WiFi.config(local_IP, gateway, subnet);
  */

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
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    String currentLine = "";  // Zmienna do przechowywania linii zapytania
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        currentLine += c;

        // Jeśli linia jest pusta, oznacza to koniec nagłówków
        if (c == '\n') {
          // Sprawdzamy, czy mamy parametry w zapytaniu GET
          if (currentLine.indexOf("GET /?text=") >= 0) {
            int textStart = currentLine.indexOf("text=") + 5;  // Znajdź początek tekstu
            int textEnd = currentLine.indexOf(" HTTP/");  // Znajdź koniec tekstu
            tekst = currentLine.substring(textStart, textEnd);  // Pobierz tekst

            // Dekodowanie URL
            tekst = urlDecode(tekst); // Dekodujemy URL, np. %26 na &
            
            // Usuwanie polskich znaków i ich zamiana na zwykłe litery
            tekst = removePolishChars(tekst);

            lcd.clear();  // Wyczyść LCD przed wyświetleniem nowego tekstu
            lcd.print(tekst);  // Wyświetl tekst na LCD
          }

          // Wysłanie odpowiedzi HTTP
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html; charset=UTF-8");  // Ustawiamy kodowanie na UTF-8
          client.println("Connection: close");
          client.println();  // Pusta linia oddzielająca nagłówki od treści
          client.println("<!DOCTYPE html><html>");
          client.println("<head>");
          client.println("<meta charset=\"UTF-8\">");  // Dodajemy meta tag dla kodowania UTF-8
          client.println("<title>ESP32 LCD</title>");
          client.println("</head>");
          client.println("<body>");
          client.println("<h1>Wyślij tekst na LCD</h1>");
          client.println("<form action=\"/\" method=\"GET\">");
          client.println("Tekst: <input type=\"text\" name=\"text\">");
          client.println("<input type=\"submit\" value=\"Wyślij\">");
          client.println("</form>");
          client.println("<h2>Otrzymany tekst:</h2>");
          client.println("<p>" + tekst + "</p>");
          client.println("</body></html>");
          client.println();

          break;  // Przerwij pętlę, kończąc odpowiedź
        }
      }
    }
    client.stop();  // Zamykamy połączenie
  }
}
