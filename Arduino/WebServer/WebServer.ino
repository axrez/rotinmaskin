#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "diller"
#define STAPSK  "12345678"
#endif

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

const int led = 13;

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/html", "<!DOCTYPE html> <html lang='en'> <head> <meta charset='UTF-8' /> <meta name='viewport' content='width=device-width, initial-scale=1.0' /> <meta http-equiv='X-UA-Compatible' content='ie=edge' /> <!-- <link rel='stylesheet' href='style.css' /> --> <title>M&M's sortering</title> <style> * { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; } h1 { font-size: 1.7rem; font-weight: 300; } body { display: flex; flex-direction: column; align-items: center; } .header { text-transform: uppercase; } button { padding: 0 2rem; border: 0; cursor: pointer; border-radius: 3px; text-transform: uppercase; } .start { background: rgb(0, 185, 0); color: white; } .stop { background: rgb(124, 0, 0); color: white; } .btn-area > button { margin: 0.5rem; } </style> </head> <body> <section class='head'> <h1 class='header'>Kontrolpanel</h1> <div class='divider'></div> </section> <section class='main'> <div class='btn-area'> <button onclick='handleStart()' class='start' ><p>start</p></button> <button class='stop'><p>stop</p></button> </div> </section> <section class='footer'><h6>test 2</h6></section> </body> <script> const btn = document.getElementsByClassName('start'); const handleStart = () => { fetch('192.168.4.1') .then(res => console.log('start')) } </script> </html>");
  digitalWrite(led, 0);
}

void handleStart() {
  Serial.println("start");
  server.send(200);
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

  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}
