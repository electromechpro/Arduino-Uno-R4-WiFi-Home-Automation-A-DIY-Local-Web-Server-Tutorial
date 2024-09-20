// Uses R4 with 4 relay shield to test web page updating capabilities of R4.
// Bonus:  IP address is displayed on matrix so you don't need a serial monitor yay!

#include "ArduinoGraphics.h"
#include "Arduino_LED_Matrix.h"

ArduinoLEDMatrix matrix;

#include "WiFiS3.h"

#define LED1 7
#define LED2 6
#define LED3 5
#define LED4 4

char ssid[] = "HVS Starlink";  //Enter your WIFI SSID
char pass[] = "Dynatest2023";  //Enter your WIFI password
int keyIndex = 0;              // your network key index number (needed only for WEP)

String output1 = "off";
String output2 = "off";
String output3 = "off";
String output4 = "off";
String header;

unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 2000;

int status = WL_IDLE_STATUS;
WiFiServer server(80);


void setup() {
  Serial.begin(9600);
  matrix.begin();
  

  // set the LED pins to OUTPUT
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);

  // Turn off LEDs initially
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
  digitalWrite(LED3, LOW);
  digitalWrite(LED4, LOW);
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
    Serial.println("Please upgrade the firmware");
  }

  while (status != WL_CONNECTED) {
    Serial.print("Network named: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }
  server.begin();
  printWifiStatus();
}

void loop() {
  webServer();
  LEDMatrix();
}

void LEDMatrix() {

  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(50);
  IPAddress ip = WiFi.localIP();
  matrix.textFont(Font_5x7);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println(ip);
  matrix.endText(SCROLL_LEFT);

  matrix.endDraw();
}

void webServer() {
  WiFiClient client = server.available();
  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    currentTime = millis();
    previousTime = currentTime;
    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        header += c;
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            if (header.indexOf("GET /ledon1") >= 0) {
              output1 = "on";
              digitalWrite(LED1, HIGH);
            } else if (header.indexOf("GET /ledoff1") >= 0) {
              output1 = "off";
              digitalWrite(LED1, LOW);
            }
            if (header.indexOf("GET /ledon2") >= 0) {
              output2 = "on";
              digitalWrite(LED2, HIGH);
            } else if (header.indexOf("GET /ledoff2") >= 0) {
              output2 = "off";
              digitalWrite(LED2, LOW);
            }
            if (header.indexOf("GET /ledon3") >= 0) {
              output3 = "on";
              digitalWrite(LED3, HIGH);
            } else if (header.indexOf("GET /ledoff3") >= 0) {
              output3 = "off";
              digitalWrite(LED3, LOW);
            }
            if (header.indexOf("GET /ledon4") >= 0) {
              output4 = "on";
              digitalWrite(LED4, HIGH);
            } else if (header.indexOf("GET /ledoff4") >= 0) {
              output4 = "off";
              digitalWrite(LED4, LOW);
            }

            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".on { background-color: #FF0000; border: 5px; color: white; padding: 16px 40px; border-radius: 20px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".off {background-color: #000000;border: 5px; color: white; padding: 16px 40px; border-radius: 20px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}</style></head>");

            client.println("<body><h1>technolab electronics</h1>");

            client.println("<p>LED1 " + output1 + "</p>");
            if (output1 == "off") {
              client.println("<p><a href=\"/ledon1\"><button class=\"off\">Turn ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/ledoff1\"><button class=\"on\">Turn OFF</button></a></p>");
            }

            client.println("<p>LED2 " + output2 + "</p>");
            if (output2 == "off") {
              client.println("<p><a href=\"/ledon2\"><button class=\"off\">Turn ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/ledoff2\"><button class=\"on\">Turn OFF</button></a></p>");
            }

            client.println("<p>LED3 " + output3 + "</p>");
            if (output3 == "off") {
              client.println("<p><a href=\"/ledon3\"><button class=\"off\">Turn ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/ledoff3\"><button class=\"on\">Turn OFF</button></a></p>");
            }

            client.println("<p>LED4 " + output4 + "</p>");
            if (output4 == "off") {
              client.println("<p><a href=\"/ledon4\"><button class=\"off\">Turn ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/ledoff4\"><button class=\"on\">Turn OFF</button></a></p>");
            }
            client.println("</body></html>");

            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    header = "";
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  IPAddress ip = WiFi.localIP();    //move to global for matrix print
  Serial.print("IP Address: ");
  Serial.println(ip);

  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  Serial.print("Now open this URL on your browser --> http://");
  Serial.println(ip);
}
