// Libraries
#include <WiFi.h>
#include <WebServer.h>



// Settings
#define AP_SSID "ROUET-AP-IOT"
#define AP_PASSWORD "123456789iot"
IPAddress local_ip(192,168,1,1);
IPAddress dns(192,168,1,255);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

#define LED_RED_PIN 12
#define LED_GREEN_PIN 13
#define LED_BLUE_PIN 14



// Webpage
#define WEBPAGE_STYLE "*{margin:0;padding:0;font-family:monospace;font-weight:600;font-size:16px;letter-spacing:5px;text-align:center;color:var(--primary-color);text-decoration:none;user-select:none}body,html{width:100%;height:100%;overflow:hidden}body{--primary-color:#eee;--background-color:#121212;background-color:var(--background-color);color:var(--primary-color)}body.on{--primary-color:#222;--background-color:#fff}#grain{position:absolute;top:0;right:0;width:200px;height:200px;overflow:hidden;mask-image:radial-gradient(circle farthest-side at 100% 0, var(--primary-color), transparent)}#content{position:absolute;left:50%;top:50%;transform:translate(-50%, -50%);background-color:var(--background-color);display:flex;flex-direction:column;gap:16px}#box{border:1px solid var(--primary-color);padding:20px}"
const char webpage_template[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
  <head>
    <title>LED Remote Control</title>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=0"/>
    <style>%s</style>
  </head>
  <body class="%s">
    <div id="grain">
      <svg viewBox='0 0 200 200' xmlns='http://www.w3.org/2000/svg'>
        <filter id='noiseFilter'>
          <feTurbulence 
            type='fractalNoise' 
            baseFrequency='1' 
            numOctaves='3' 
            stitchTiles='stitch'/>
        </filter>
        <rect width='200' height='200' filter='url(#noiseFilter)'/>
      </svg>
    </div>
    <div id="content">
      <div id="box">
        <h2 id="ledstate">%s, is the led</h2>
      </div>
      <div id="box">
        <a href="/%s">Toggle</a>
      </div>
    </div>
  </body>
</html>
)rawliteral";



// Class Components
WebServer server(80);



// Common functions
void setLedColor(unsigned char red, unsigned char green, unsigned char blue) {
  analogWrite(LED_RED_PIN, red);
  analogWrite(LED_GREEN_PIN, green);
  analogWrite(LED_BLUE_PIN, blue);
}
void setSimpleLedColor(bool red, bool green, bool blue) {
  setLedColor(red ? 255 : 0, green ? 255 : 0, blue ? 255 : 0);
}



// Code
bool ledState = false;

void setup() {
  // Serial Communication Initialization
  Serial.begin(115200);
  Serial.println("");

  // LED Initialization
  analogWrite(LED_RED_PIN, 0);
  analogWrite(LED_GREEN_PIN, 0);
  analogWrite(LED_BLUE_PIN, 0);

  // WIFI Access Point Initialization
  if (!WiFi.config(local_ip, dns, gateway, subnet)) {
    Serial.println("Failed to setup network settings");
  }
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  server.begin();
  server.on("/", HTTP_GET, handleRoot);
  server.on("/on", HTTP_GET, handleOn);
  server.on("/off", HTTP_GET, handleOff);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("ESP32 IP: ");
  Serial.println(IP);
}

void loop() {
  server.handleClient();
}

// Handler for root page
void handleRoot() {
  char webpage[2048];
  snprintf(webpage, sizeof(webpage), webpage_template,
           WEBPAGE_STYLE,
           ledState ? "on" : "off",  // body class
           ledState ? "ON" : "OFF",  // LED state text
           ledState ? "off" : "on"   // toggle link
  );
  server.send(200, "text/html", webpage);
}

// Handler for turning LED on
void handleOn() {
  ledState = true;
  setSimpleLedColor(true, false, false);
  handleRoot();
}

// Handler for turning LED off
void handleOff() {
  ledState = false;
  setSimpleLedColor(false, false, false);
  handleRoot();
}
