#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <WiFi.h>
#include <WebServer.h>

// Глобални променливи за HTTP сървъра
WebServer httpServer(80);
String ssidList;
bool isConnected = false;

// Деклариране на глобалните променливи за WiFi мрежата
const char* homeSSID = "A1_6531";
const char* homePassword = "48575443BD107EA3";

String ssid = "";
String password = "";

extern int counter; // Деклариране на counter като външна променлива

// Съществуваща функция handleRoot
void handleRoot() {
  String imageUrl = "https://batevesko.github.io/redutaBig04.png?v=" + String(millis());

  String content = "<!DOCTYPE html>";
  content += "<html lang='en'>";
  content += "<head>";
  content += "<meta charset='UTF-8'>";
  content += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  content += "<title>Hello, Bate Vesko!!!</title>";
  content += "<style>";
  content += "body { display: flex; flex-direction: column; align-items: center; justify-content: center; width: 100%; max-width: 1600px; height: 90vh; margin: auto,auto; border: 2px solid blue; background-color: rgb(250, 250, 0);}";
  content += ".container { position: relative; width: 100%; max-width: 1000px; height: auto; border: 2px solid red; background-color: rgb(250, 166, 56); }";
  content += ".image-container { position: relative; width: 100%; padding-bottom: 67.6%; }"; // Съотношение на иконката
  content += ".fullscreen-img { position: absolute; top: 0; left: 0; width: 100%; height: 100%; object-fit: contain; }"; // Ограничете височината на иконката и запазете съотношението
  content += ".temperature { position: absolute; background: yellow; color: red; border: 2px solid blue; border-radius: 10px; width: 60px; height: 30px; display: flex; justify-content: center; align-items: center; font-size: 24px; }";
  content += "#temp1 { top: 27%; left: 22%; }"; // Относителни позиции спрямо изображението
  content += "#temp2 { top: 13%; left: 79%; }";
  content += "#temp3 { top: 65%; left: 75%; }";
  content += "h1 { margin-top: 50px; }"; // Отстояние от горния край
  content += "h2 { margin-bottom: 50px; }"; // Отстояние от долния край
  content += "</style>";
  content += "</head>";
  content += "<body>";
  content += "<h1>Hello, Bate Vesko!!!</h1>";
  content += "<div class='container'>";
  content += "<div class='image-container'>";
  content += "<img src='" + imageUrl + "' alt='Reduta Icon' class='fullscreen-img'>";
  content += "<div class='temperature' id='temp1'>" + String(temp1) + "&#176;C" + "</div>";
  content += "<div class='temperature' id='temp2'>" + String(temp2) + "&#176;C" + "</div>";
  content += "<div class='temperature' id='temp3'>" + String(temp3) + "&#176;C" + "</div>";
  content += "</div>";
  content += "</div>";
  content += "<h2>Free heap: " + String(ESP.getFreeHeap()) + " Counter: " + String(counter) + "</h2>";
  content += "</body>";
  content += "</html>";

  httpServer.send(200, "text/html", content);
}

// Нови функции за сканиране и избор на WiFi мрежа
void handleWiFiSetup() {
  String content = "<!DOCTYPE html>";
  content += "<html lang='en'>";
  content += "<head>";
  content += "<meta charset='UTF-8'>";
  content += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  content += "<title>Select WiFi</title>";
  content += "</head>";
  content += "<body>";
  content += "<h1>Select WiFi Network</h1>";
  content += "<form action=\"/connect\" method=\"post\">";
  content += "<label for=\"ssid\">SSID:</label>";
  content += "<select name=\"ssid\">";
  content += ssidList;
  content += "</select><br>";
  content += "<label for=\"password\">Password:</label>";
  content += "<input type=\"password\" id=\"password\" name=\"password\"><br><br>";
  content += "<input type=\"submit\" value=\"Connect\">";
  content += "</form>";
  content += "</body>";
  content += "</html>";

  httpServer.send(200, "text/html", content);
}

void handleConnect() {
  if (httpServer.hasArg("ssid") && httpServer.hasArg("password")) {
    ssid = httpServer.arg("ssid");
    password = httpServer.arg("password");

    WiFi.begin(ssid.c_str(), password.c_str());
    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) {
      delay(500);
      Serial.println("Connecting to WiFi...");
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("Connected to ");
      Serial.print(ssid);
      Serial.print(" with IP address: ");
      Serial.println(WiFi.localIP());
      httpServer.send(200, "text/plain", "Connected to " + ssid);
    } else {
      Serial.println("Failed to connect.");
      httpServer.send(200, "text/plain", "Failed to connect.");
    }
  } else {
    httpServer.send(400, "text/plain", "Invalid request");
  }
}

void initWiFiAndServer() {
  Serial.begin(115200);

  WiFi.begin(homeSSID, homePassword);
  unsigned long startAttemptTime = millis();

  while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 15000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    isConnected = true;
    Serial.println("");
    Serial.print("Connected to ");
    Serial.print(homeSSID);
    Serial.print(" with IP address: ");
    Serial.println(WiFi.localIP());

    httpServer.on("/", handleRoot);
    httpServer.begin();
    Serial.println("HTTP server started");
  } else {
    Serial.println("");
    Serial.println("Failed to connect to home network. Starting Access Point...");

    WiFi.mode(WIFI_AP_STA);
    WiFi.softAP("ESP32-Access-Point", "12345678");

    delay(100); // Добавен малък интервал за стабилизиране

    // Сканиране на наличните WiFi мрежи
    int n = WiFi.scanNetworks();
    if (n == 0) {
      Serial.println("No networks found");
    } else {
      Serial.println("Networks found:");
      for (int i = 0; i < n; ++i) {
        ssidList += "<option value=\"" + WiFi.SSID(i) + "\">" + WiFi.SSID(i) + "</option>";
        Serial.println(WiFi.SSID(i));
      }
    }

    httpServer.on("/", handleWiFiSetup);
    httpServer.on("/connect", HTTP_POST, handleConnect);
    httpServer.begin();
    Serial.println("HTTP server started");
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
  }
}

void handleClient() {
  httpServer.handleClient();
}

#endif // HTTPSERVER_H
