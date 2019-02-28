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

const int led = 5;

bool running = false;
int startSpeed = 0;
int incrementSize = 100;
int threshhold[] = {0, 1000};

void handleRoot() {
  String root = "<!DOCTYPE html> <html lang='en'> <head> <meta charset='UTF-8' /> <meta name='viewport' content='width=device-width, initial-scale=1.0' /> <meta http-equiv='X-UA-Compatible' content='ie=edge' /> <!-- <link rel='stylesheet' href='style.css' /> --> <title>M&M's sortering</title> <style> * { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; } h1 { font-size: 1.7rem; font-weight: 300; } body { display: flex; flex-direction: column; align-items: center; } .header { text-transform: uppercase; } button { padding: 0 2rem; border: 0; cursor: pointer; border-radius: 3px; text-transform: uppercase; } .start { background: rgb(0, 185, 0); color: white; } .stop { background: rgb(124, 0, 0); color: white; } .btn-area > div > button { margin: 0.5rem; } .info-area { text-align: center; } .first-row { padding-left: 0.8rem; } .speedContianer { display: flex; justify-content: center; } #speed { padding-left: 0.2em; } </style> </head> <body> <section class='head'> <h1 class='header'>Kontrolpanel</h1> <div class='divider'></div> </section> <section class='main'> <div class='info-area'> <div id='state'></div> <div class='speedContianer'> <p>Speed: </p> <p id='speed'></p> </div> </div> <div class='btn-area'> <div class='first-row'> <button onclick='handleStart()' class='start' ><p>start</p></button> <button class='stop' onclick='handleStop()' ><p>stop</p></button> </div> <div class='second-row'> <button class='speedDown' onclick='handleSpeedDown()' ><p>Speed -</p></button> <button class='speedUp' onclick='handleSpeedUp()' ><p>Speed +</p></button> </div> </div> </section> </body> <script> let started =";
  root += running;
  root +="; let speed = ";
  root += startSpeed;
  root += "; let increment = ";
  root += incrementSize;
  root += "; const stateField = document.getElementById('state'); const speedField = document.getElementById('speed'); speedField.innerHTML = speed; stateField.innerHTML = started ? 'Started' : 'Stopped'; const handleStart = () => { fetch('/start') .then(res => res.status === 206 ? started = true : null) .then(res => stateField.innerHTML = started ? 'Started' : 'Stopped'); }; const handleStop = () => { fetch('/stop') .then(res => res.status === 207 ? started = false : null) .then(res => stateField.innerHTML = !started ? 'Stopped' : 'Started'); }; const handleSpeedUp = () => { fetch('/speedUp') .then(res => res.status === 210 ? speed += increment : null) .then(res => speedField.innerHTML = speed); }; const handleSpeedDown = () => { fetch('/speedDown') .then(res => res.status === 211 ? speed -= increment : null) .then(res => speedField.innerHTML = speed); }; </script> </html>";
  server.send(200, "text/html", root);
}

void handleStart() {
  Serial.println("start");
  server.send(206, "text/plain", "1");
  running = true;
}

void handleStop() {
  Serial.println("stop");
  server.send(207, "text/plain", "0");
  running = false;
}

void handleSpeedUp() {
  if((startSpeed + incrementSize) > threshhold[1]) {
    server.send(400);
  } else {
    startSpeed += incrementSize;
    Serial.println("Speed Up: "  + startSpeed);
    server.send(210);
    analogWrite(led, startSpeed);
  }
}

void handleSpeedDown() {
  if((startSpeed - incrementSize) < threshhold[0]) {
    server.send(400);
  } else {
    startSpeed -= incrementSize;
    Serial.println("Speed Down");
    server.send(211);
    analogWrite(led, startSpeed);
  }
}

// void handleMotor() {
//   if(server.arg("speed")) {
//     int  resSpeed = server.arg("speed");
//     if (resSpeed > threshhold[0] || resSpeed < threshhold[1]) {
//       String msg = "Speed is not within the threshhold (" + threshhold[0] + ", " + threshhold[1] + ")"
//       server.send(403, "text/plain", msg);
//     } else {
//       speed = resSpeed;
//       server.send(200, "text/plain", "Speed is now: " + speed);
//     };
//   };
//   if(server.arg("state")) {
//     String resState = server.arg("state");
//     if(resState == 'on') {
//       state = true;
//     } else if(resState == 'off') {
//       state = false;
//     } else {
//       server.send(400, "text/plain", "state must be set to either 'on' or 'off'");
//     };
//   };
// }

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

  // server.on("/motor", handleMotor);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  // monitorer pin og tæl antal af sorteret m&m's
  if(running) {
    analogWrite(led, startSpeed);
  } else {
    analogWrite(led, 0);
  }
}
