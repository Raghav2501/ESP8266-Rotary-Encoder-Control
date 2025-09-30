#include <ESP8266WiFi.h>  // Include WiFi library for ESP8266

// 🛠️ Wi-Fi credentials (replace with your own)
const char* ssid = "yourwifiname";       // Your Wi-Fi network name
const char* password = "wifipassword";   // Your Wi-Fi password

// 🌐 Server details (ESP8266 server IP and port)
const char* serverIP = "192.168.1.xx"; // IP address of the server ESP8266
const int serverPort = 8080;             // Port number used for TCP communication

WiFiClient client;  // Create a TCP client object

// 🎛️ Rotary encoder pin assignments
const int clkPin = D5;  // Clock pin
const int dtPin  = D6;  // Data pin
const int swPin  = D7;  // Switch (push button) pin

// 🔧 State variables
int brightness = 128;   // Initial brightness (0–255)
bool ledState = true;   // LED toggle state (ON/OFF)
int lastClk = HIGH;     // Previous CLK state for edge detection

// ⏱️ Timing variables for debounce and smoothing
unsigned long lastRotateTime = 0;
const unsigned long rotateDelay = 50;     // Minimum time between rotations

unsigned long lastButtonTime = 0;
const unsigned long debounceDelay = 300;  // Minimum time between button presses

void setup() {
  Serial.begin(115200);  // Start serial communication for debugging

  // 🧭 Set rotary encoder pins as inputs with pull-up resistors
  pinMode(clkPin, INPUT_PULLUP);
  pinMode(dtPin, INPUT_PULLUP);
  pinMode(swPin, INPUT_PULLUP);

  // 📶 Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());  // Print local IP for reference

  // 🔌 Connect to the server
  client.connect(serverIP, serverPort);
}

void loop() {
  // 🔄 Rotary encoder rotation detection
  int clk = digitalRead(clkPin);
  int dt  = digitalRead(dtPin);

  // Detect falling edge on CLK to determine rotation
  if (clk != lastClk && clk == LOW) {
    if (millis() - lastRotateTime > rotateDelay) {
      // Determine direction based on DT pin
      if (dt == HIGH) {
        brightness = min(brightness + 10, 255);  // Clockwise → increase brightness
      } else {
        brightness = max(brightness - 10, 0);    // Counterclockwise → decrease brightness
      }
      sendMessage("BRIGHTNESS:" + String(brightness));  // Send brightness to server
      Serial.println("Brightness: " + String(brightness));
      lastRotateTime = millis();  // Update last rotation time
    }
  }
  lastClk = clk;  // Store current CLK state

  // 🔘 Button press detection for toggle
  if (digitalRead(swPin) == LOW) {
    if (millis() - lastButtonTime > debounceDelay) {
      ledState = !ledState;  // Toggle LED state
      sendMessage("TOGGLE:" + String(ledState ? "ON" : "OFF"));  // Send toggle command
      Serial.println("Toggle: " + String(ledState ? "ON" : "OFF"));
      lastButtonTime = millis();  // Update last button press time
    }
  }

  delay(5);  // Small delay to reduce CPU load
}

// 📤 Send message to server via TCP
void sendMessage(String msg) {
  if (!client.connected()) {
    client.connect(serverIP, serverPort);  // Reconnect if disconnected
  }
  client.println(msg);  // Send message to server
}
