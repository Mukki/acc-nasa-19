#include <Wire.h>
#include <WiFi.h>
#include "MPU6050.h"

MPU6050 MPU;

WiFiServer server(80);

const char* ssid     = "Accelerometer";
const char* password = "123456789";

const int MPU_addr = 0x68;  // I2C address of the MPU-6050

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";

// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;

String header;

int16_t AcX, AcY, AcZ, Tmp, GyX, GyY, GyZ;

void setup(){
  MPU.initialize();
  Serial.println(MPU.getFullScaleAccelRange());
  MPU.setFullScaleAccelRange(MPU6050_ACCEL_FS_16);
  Serial.println(MPU.getFullScaleAccelRange());
  MPU.initialize();
  Serial.println(MPU.getFullScaleAccelRange());

  Serial.begin(115200);
  // Initialize the output variables as outputs
  pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output26, LOW);
  digitalWrite(output27, LOW);

  Serial.print("Setting Access Point");
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();cd
  Serial.print("IP address is ");
  Serial.println(IP);
  
  server.begin();
}

void loop(){
  MPU.getAcceleration(&AcX, &AcY, &AcZ);

  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();
            
            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons 
            // Feel free to change the background-color and font-size attributes to fit your preferences
            client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: #555555;}</style></head>");
            
            // Web Page Heading
            client.println("<body><h1>Data from accelerometer</h1>");
            
            // Display current state, and ON/OFF buttons for GPIO 26  
            client.println("<p>Ax - " + String(Ax) + "</p>");
            client.println("<p>Ay - " + String(Ay) + "</p>");
            client.println("<p>Az - " + String(Az) + "</p>");

            client.println("</body></html>");
            
            // The HTTP response ends with another blank line
            client.println();
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
