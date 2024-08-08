#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <WiFi.h>
#include <ESPAsyncWebServer.h>
//#include <WebServer.h>

// Глобални променливи за HTTP сървъра
//extern WebServer httpServer;
extern AsyncWebServer httpServer;
extern String ssidList;

//extern String ssid;
//extern String password;
extern String lastKnownSSID;
extern String lastKnownPassword;
extern int counter;

// Променливи за температури
extern int temp1;
extern int temp2;
extern int temp3;

//String ssidList;

void handleRoot(AsyncWebServerRequest *request);
void handleConnect(AsyncWebServerRequest *request);
void handleWelcome(AsyncWebServerRequest *request);
void displayIPAddress();
void printNetworkDetails();

// bool setupWiFi() {
//   const char* predefined_ssid = "TP-Link_1AEA";
//   const char* predefined_password = "83590566";

//   WiFi.begin(predefined_ssid, predefined_password);
//   Serial.println("Trying to connect to predefined WiFi...");

//   int retries = 10;
//   while (WiFi.status() != WL_CONNECTED && retries > 0) {
//     delay(1000);
//     Serial.print(".");
//     retries--;
//   }

//   if (WiFi.status() == WL_CONNECTED) {
//     Serial.println("");
//     Serial.print("Connected to ");
//     Serial.println(predefined_ssid);
//     Serial.print("IP address: ");
//     Serial.println(WiFi.localIP());
//     WiFi.softAPdisconnect(true);  // Прекратява AP режима
//     return true; // Свързване успешно
//   } else {
//     Serial.println("");
//     Serial.println("Failed to connect to predefined WiFi.");
//     return false; // Свързване неуспешно
//   }
// }

// void startAPAndHTTPServer() {
//   Serial.println("Starting AP mode...");
//   WiFi.softAP("BatevotoVeskovo_ESP32-AP_1", "1234567890");
//   IPAddress IP = WiFi.softAPIP();
//   Serial.print("AP IP address: ");
//   Serial.println(IP);

//   // Започване на HTTP сървъра
//   httpServer.on("/", handleRoot);
//   httpServer.on("/connect", handleConnect);
//   httpServer.on("/welcome", handleWelcome);
//   httpServer.begin();
//   Serial.println("HTTP server started");

//   int n = WiFi.scanNetworks();
//   ssidList = "";
//   for (int i = 0; i < n; i++) {
//     ssidList += "<option value=\"" + WiFi.SSID(i) + "\">" + WiFi.SSID(i) + "</option>";
//   }
// }

void handleRoot(AsyncWebServerRequest *request) {
  if (WiFi.status() == WL_CONNECTED) {
    request->redirect("/welcome");
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

  request->send(200, "text/html", content);
}

void handleConnect(AsyncWebServerRequest *request) {
  if (request->hasParam("ssid", true) && request->hasParam("password", true)) {
    String ssid = request->getParam("ssid", true)->value();
    String password = request->getParam("password", true)->value();

    Serial.println("Attempting to connect to WiFi...");

    WiFi.begin(ssid.c_str(), password.c_str());

    int connAttempts = 20;
    while (WiFi.status() != WL_CONNECTED && connAttempts-- > 0) {
      delay(250);
      Serial.print(".");
      // Ресет на watchdog-а
      vTaskDelay(1);  // yield(); също може да се ползва
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println();
      Serial.println("Connected!");
      request->send(200, "text/html", "Connected to " + ssid);
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());


      displayIPAddress();            // -> 020_display_units.h. Показва IP горе на дисплея.
      lastKnownSSID = ssid;          // **************************
      lastKnownPassword = password;  // ***********************************

      printNetworkDetails();

      // WiFi.softAPdisconnect(true);
      // WiFi.mode(WIFI_STA);

      // // Пренасочване към новата страница
      //delay(1000);  // Изчакване за стабилна връзка
      request->redirect("/welcome");
      // httpServer.sendHeader("Location", "/welcome", true);
      // //httpServer.send(302, "text/plain", "");
      //request->send(302, "text/plain", "";

      // // Изключване на Access Point режима
      WiFi.softAPdisconnect(true);  // Прекратява AP режима
      WiFi.mode(WIFI_STA);          // Превключва платката в режим станция само

      Serial.println("Switched to station mode.");
    } else {
      Serial.println();
      Serial.println("Failed to connect.");
      WiFi.disconnect();
      WiFi.mode(WIFI_AP_STA);
      WiFi.softAP("BatevotoVeskovo_ESP32-AP_1", "1234567890");
      //Serial.println("Failed to connect, returning to AP mode.");
      request->send(200, "text/html", "Failed to connect to " + ssid + ". Returned to AP mode.");
    }
  } else {
    request->send(400, "text/plain", "Invalid request");
  }
}

// void handleConnect() {
//   if (httpServer.hasArg("ssid") && httpServer.hasArg("password")) {
//     ssid = httpServer.arg("ssid");
//     password = httpServer.arg("password");

//     WiFi.begin(ssid.c_str(), password.c_str());
//     while (WiFi.status() != WL_CONNECTED) {
//       delay(1000);
//       Serial.println("Connecting to WiFi...");
//     }

//     if (WiFi.status() == WL_CONNECTED) {
//       Serial.print("Connected to ");
//       Serial.print(ssid);
//       Serial.print(" with IP address: ");
//       Serial.println(WiFi.localIP());
//       displayIPAddress();  // -> 020_display_units.h. Показва IP горе на дисплея.
//       lastKnownSSID = ssid;  // **************************
//       lastKnownPassword = password;  // ***********************************

//       printNetworkDetails();

//       // Пренасочване към новата страница
//       delay(1000);  // Изчакване за стабилна връзка
//       httpServer.sendHeader("Location", "/welcome", true);
//       httpServer.send(302, "text/plain", "");

//       // Изключване на Access Point режима
//       WiFi.softAPdisconnect(true);  // Прекратява AP режима
//       WiFi.mode(WIFI_STA);  // Превключва платката в режим станция само
//     } else {
//       httpServer.send(400, "text/plain", "Failed to connect to WiFi");
//     }
//   } else {
//     httpServer.send(400, "text/plain", "Invalid request");
//   }
// }

void handleWelcome(AsyncWebServerRequest *request) {
  String imageUrl = "https://batevesko.github.io/redutaBig12.png?v=" + String(millis());

  String content = "<!DOCTYPE html>";
  content += "<html lang='en'>";
  content += "<head>";
  content += "<meta charset='UTF-8'>";
  content += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  content += "<title>Welcome</title>";
  content += "<style>";
  content += "body { display: flex; flex-direction: column; align-items: center; justify-content: center; width: 100%; max-width: 1600px; height: 90vh; margin: auto,auto; border: 2px solid blue; background-color: rgb(0, 100, 100);}";
  content += ".container { position: relative; width: 100%; max-width: 1000px; height: auto; border: 2px solid red; background-color: rgb(0, 200, 200); }";
  content += ".image-container { position: relative; width: 100%; padding-bottom: 67.6%; }";                              // Съотношение на иконката
  content += ".fullscreen-img { position: absolute; top: 0; left: 0; width: 100%; height: 100%; object-fit: contain; }";  // Ограничете височината на иконката и запазете съотношението
  content += ".temperature { position: absolute; background: rgb(173, 216, 230); color: rgb(0, 0, 255); border: 2px solid rgb(0, 0, 255); border-radius: 10px; width: 60px; height: 30px; display: flex; justify-content: center; align-items: center; font-size: 24px; }";
  content += "#temp1 { top: 23%; left: 20%; }";  // Относителни позиции спрямо изображението
  content += "#temp2 { top: 13%; left: 82%; }";
  content += "#temp3 { top: 59%; left: 80%; }";
  content += "h1 { margin-top: 50px; color: white;}";      // Отстояние от горния край
  content += "h2 { margin-bottom: 50px; color: yellow;}";  // Отстояние от долния край
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

  request->send(200, "text/html", content);
}

void printNetworkDetails() {
  Serial.print("Connected to: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Subnet Mask: ");
  Serial.println(WiFi.subnetMask());
  Serial.print("Gateway IP: ");
  Serial.println(WiFi.gatewayIP());
  Serial.print("DNS IP: ");
  Serial.println(WiFi.dnsIP());
}

#endif  // HTTPSERVER_H
