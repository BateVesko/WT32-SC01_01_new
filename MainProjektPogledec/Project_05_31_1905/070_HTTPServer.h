#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <WiFi.h>
#include <WebServer.h>

// Глобални променливи за HTTP сървъра
extern WebServer httpServer;
extern String ssidList;
extern String ssid;
extern String password;
extern int counter;

// Променливи за температури
extern int temp1;
extern int temp2;
extern int temp3;

void handleRoot();
void handleConnect();
void handleWelcome();
void displayIPAddress();

bool setupWiFi() {
  const char* predefined_ssid = "TP-Link_1AEA";
  const char* predefined_password = "83590566";

  WiFi.begin(predefined_ssid, predefined_password);
  Serial.println("Trying to connect to predefined WiFi...");

  int retries = 10;
  while (WiFi.status() != WL_CONNECTED && retries > 0) {
    delay(1000);
    Serial.print(".");
    retries--;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(predefined_ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    WiFi.softAPdisconnect(true);  // Прекратява AP режима
    return true; // Свързване успешно
  } else {
    Serial.println("");
    Serial.println("Failed to connect to predefined WiFi.");
    return false; // Свързване неуспешно
  }
}

void startAPAndHTTPServer() {
  Serial.println("Starting AP mode...");
  WiFi.softAP("Your_AP_SSID", "Your_AP_Password");
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Започване на HTTP сървъра
  httpServer.on("/", handleRoot);
  httpServer.on("/connect", handleConnect);
  httpServer.on("/welcome", handleWelcome);
  httpServer.begin();
  Serial.println("HTTP server started");
}

void handleRoot() {
  if (WiFi.status() == WL_CONNECTED) {
    httpServer.sendHeader("Location", "/welcome", true);
    httpServer.send(302, "text/plain", "");
    return;
  }

  String content = "<!DOCTYPE html>";
  content += "<html lang='en'>";
  content += "<head>";
  content += "<meta charset='UTF-8'>";
  content += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  content += "<title>Select WiFi</title>";
  content += "<style>";
  content += "h1 { color: blue; background-color: pink; }";
  content += "</style>";
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
    while (WiFi.status() != WL_CONNECTED) {
      delay(1000);
      Serial.println("Connecting to WiFi...");
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.print("Connected to ");
      Serial.print(ssid);
      Serial.print(" with IP address: ");
      Serial.println(WiFi.localIP());
      displayIPAddress();  // -> 020_display_units.h. Показва IP горе на дисплея.

      // Пренасочване към новата страница
      delay(1000);  // Изчакване за стабилна връзка
      httpServer.sendHeader("Location", "/welcome", true);
      httpServer.send(302, "text/plain", "");

      // Изключване на Access Point режима
      WiFi.softAPdisconnect(true);  // Прекратява AP режима
      WiFi.mode(WIFI_STA);  // Превключва платката в режим станция само
    } else {
      httpServer.send(400, "text/plain", "Failed to connect to WiFi");
    }
  } else {
    httpServer.send(400, "text/plain", "Invalid request");
  }
}

void handleWelcome() {
  String imageUrl = "https://batevesko.github.io/redutaBig11.png?v=" + String(millis());

  String content = "<!DOCTYPE html>";
  content += "<html lang='en'>";
  content += "<head>";
  content += "<meta charset='UTF-8'>";
  content += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  content += "<title>Welcome</title>";
  content += "<style>";
  content += "body { display: flex; flex-direction: column; align-items: center; justify-content: center; width: 100%; max-width: 1600px; height: 90vh; margin: auto,auto; border: 2px solid blue; background-color: rgb(0, 100, 100);}";
  content += ".container { position: relative; width: 100%; max-width: 1000px; height: auto; border: 2px solid red; background-color: rgb(0, 200, 200); }";
  content += ".image-container { position: relative; width: 100%; padding-bottom: 67.6%; }"; // Съотношение на иконката
  content += ".fullscreen-img { position: absolute; top: 0; left: 0; width: 100%; height: 100%; object-fit: contain; }"; // Ограничете височината на иконката и запазете съотношението
  content += ".temperature { position: absolute; background: rgb(173, 216, 230); color: rgb(0, 0, 255); border: 2px solid rgb(0, 0, 255); border-radius: 10px; width: 60px; height: 30px; display: flex; justify-content: center; align-items: center; font-size: 24px; }";
  content += "#temp1 { top: 23%; left: 20%; }"; // Относителни позиции спрямо изображението
  content += "#temp2 { top: 13%; left: 82%; }";
  content += "#temp3 { top: 59%; left: 80%; }";
  content += "h1 { margin-top: 50px; color: white;}"; // Отстояние от горния край
  content += "h2 { margin-bottom: 50px; color: yellow;}"; // Отстояние от долния край
  content += "</style>";
  content += "</head>";
  content += "<body>";
  content += "<h1>Welcome, Bate Vesko!!!</h1>";
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

#endif // HTTPSERVER_H
