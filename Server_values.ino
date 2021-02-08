//notes:
//1. To read click value, use http://192.168.29.128/click/
//2. To set click value, use http://192.168.29.128/click/0
//3. Similarly for other endpoints
//4. Static ip of the server is 192.168.29.128




#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

const char* ssid = "abc";
const char* password = "xyza";

WebServer server(80);

const int led = 13;

volatile int startBot = 0, clickNow = 0, numberOfClicks = 0;



// Set your Static IP address
IPAddress local_IP(192, 168, 29, 128);
// Set your Gateway IP address
IPAddress gateway(192, 168, 1, 1);

IPAddress subnet(255, 255, 0, 0);


void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp32!");
  digitalWrite(led, 0);
}



void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
    // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("STA Failed to configure");
  }
  
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp32")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);


  server.onNotFound(handleNotFound);


  //click
  server.on("/click/{}", []() {
    String inputVal = server.pathArg(0);
    if (inputVal == "") {
      server.send(200, "text/plain", "value:" + String(clickNow ));
    }
    else
    {
      clickNow = inputVal.toInt();
      server.send(200, "text/plain", "value:" + String(clickNow));
    }
  });

  //start
  server.on("/start/{}", []() {
    String inputVal = server.pathArg(0);
    if (inputVal == "") {
      server.send(200, "text/plain", "value:" + String(startBot) );
    }
    else
    {
      startBot = inputVal.toInt();
      server.send(200, "text/plain", "value:" + String(startBot));
    }
  });

  //number of clicks
  server.on("/number/{}", []() {
    String inputVal = server.pathArg(0);
    if (inputVal == "") {
      server.send(200, "text/plain", "value:" + String(numberOfClicks) );
    }
    else
    {
      numberOfClicks = inputVal.toInt();
      server.send(200, "text/plain", "value:" + String(numberOfClicks));
    }
  });


  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
}
