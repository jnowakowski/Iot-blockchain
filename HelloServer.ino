#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>

const char* ssid = "Krakow";
const char* password = "8.warszawa.8";
const char* APItoken = "XGGVXPW9Z6VYDABG8ZY1CC3BYAH1AUME4P";

ESP8266WebServer server(80);

const int led = 13;

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp8266!");
  digitalWrite(led, 0);
}

void handleNotFound(){
  digitalWrite(led, 1);
  String message = "URI: ";
  String address = server.uri();
  message += address;
  message += '\n\n';

  WiFiClientSecure client;

  if (!client.connect("api.etherscan.io", 443)) {
    server.send(200, "text/plain", "error connecting to etherscan API");
    Serial.println("error connecting to etherscan API");
    return;
  }

  String url = "/api?module=account&action=balance&address=";
  url += "0xddbd2b932c763ba5b1b7ae3b362eac3e8d40121a";
  url += "&tag=latest&apikey=";
  url += APItoken;
  client.println("GET " + url +" HTTP/1.1");
  client.println("Host: api.etherscan.io");
  client.println("Connection: close");
  client.println();

  while(client.connected()) {
    String line = client.readStringUntil('\n');
    if (line.indexOf("result") > 0) {
      Serial.println("### " + line);
      message += (line + '\n');
      StaticJsonBuffer<200> jsonBuffer;
      JsonObject& root = jsonBuffer.parseObject(line);
      String wei = root["result"];
      message += "\nwei: ";
      message += wei;
      Serial.println("Wei: " + wei);
    }
  }

  server.send(200, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void){
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
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

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  // server.on("/", handleRoot);
  server.on("/", handleNotFound);

  server.on("/inline", [](){
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void){
  server.handleClient();
}

// message += "\nMethod: ";
// message += (server.method() == HTTP_GET)?"GET":"POST";
// message += "\nArguments: ";
// message += server.args();
// message += "\n";
// for (uint8_t i=0; i<server.args(); i++){
//   message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
// }
