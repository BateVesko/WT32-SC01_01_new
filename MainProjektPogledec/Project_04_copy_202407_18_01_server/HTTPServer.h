#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <WiFi.h>
#include <WebServer.h>

// Глобални променливи за HTTP сървъра
WebServer httpServer(80);
String ssidList;

// Деклариране на глобалните променливи за WiFi мрежата
String ssid = "";
String password = "";

void handleRoot() {
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
    while (WiFi.status() != WL_CONNECTED) {  //  && millis() - startAttemptTime < 15000
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

  // Инициализиране на HTTP сървъра
  httpServer.on("/", handleRoot);
  httpServer.on("/connect", HTTP_POST, handleConnect);
  httpServer.begin();
  Serial.println("HTTP server started");
  Serial.print("AP IP address: ");
  Serial.println(WiFi.softAPIP());
}

void handleClient() {
  httpServer.handleClient();
}

#endif // HTTPSERVER_H
