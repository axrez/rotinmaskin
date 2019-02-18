#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "Rotinmaskin"
#define STAPSK  "12345678"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

const int led = 2;

bool running = false;

void handleRoot() {
  String root = "<!DOCTYPE html> <html lang='en'> <head> <meta charset='UTF-8' /> <meta name='viewport' content='width=device-width, initial-scale=1.0' /> <meta http-equiv='X-UA-Compatible' content='ie=edge' /> <!-- <link rel='stylesheet' href='style.css' /> --> <title>M&M's sortering</title> <style> * { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; } h1 { font-size: 1.7rem; font-weight: 300; } body { display: flex; flex-direction: column; align-items: center; } .header { text-transform: uppercase; } button { padding: 0 2rem; border: 0; cursor: pointer; border-radius: 3px; text-transform: uppercase; } .start { background: rgb(0, 185, 0); color: white; } .stop { background: rgb(124, 0, 0); color: white; } .btn-area > div > button { margin: 0.5rem; } .info-area { text-align: center; } .first-row { padding-left: 1rem; } </style> </head> <body> <section class='head'> <h1 class='header'>Kontrolpanel</h1> <div class='divider'></div> </section> <section class='main'> <div class='info-area'> <div id='state'></div> <div id='speed'></div> </div> <div class='btn-area'> <div class='first-row'> <button onclick='handleStart()' class='start' ><p>start</p></button> <button class='stop' onclick='handleStop()' ><p>stop</p></button> </div> <div class='second-row'> <button class='speedUp' onclick='handleSpeedUp()' ><p>Speed +</p></button> <button class='speedDown' onclick='handleSpeedDown()' ><p>Speed -</p></button> </div> </div> </section> <section class='footer'><h6>test 2</h6></sect/ion> </body> <script> let started = false; let speed = 0; const stateField = document.getElementById('state'); stateField.innerHTML = started ? 'Started' : 'Stopped'; const handleStart = () => { fetch('/start') .then(res => res.status === 206 ? started = true : null); stateField.innerHTML = started ? 'Stopped' : 'Started'; }; const handleStop = () => { fetch('/stop') .then(res => res.status === 207 ? started = false : null); stateField.innerHTML = started ? 'Stopped' : 'Started'; }; const handleSpeedUp = () => { fetch('/speedUp') .then(res => console.log(res.status)); }; const handleSpeedDown = () => { fetch('/speedDown') .then(res => console.log(res.status)); }; </script> </html>";
  digitalWrite(led, 0);
  server.send(200, "text/html", root);
  digitalWrite(led, 1);
}

void handleStart() {
  Serial.println("start");
  server.send(206, "text/plain", "1");
  running = true;
  digitalWrite(led, 0);
}

void handleStop() {
  Serial.println("stop");
  server.send(207, "text/plain", "0");
  running = false;
  digitalWrite(led, 1);
}

void handleSpeedUp() {
  Serial.println("Speed Up");
  server.send(210);
}

void handleSpeedDown() {
  Serial.println("Speed Down");
  server.send(211);
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
  WiFi.softAP(ssid, password);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/start", handleStart);

  server.on("/stop", handleStop);

  server.on("/speedUp", handleSpeedUp);

  server.on("/speedDown", handleSpeedDown);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}
