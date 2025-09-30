#include <ESP8266WiFi.h>        // Wi-Fi library for ESP8266
#include <LiquidCrystal_I2C.h>  // I2C LCD library

// 🛠️ Wi-Fi credentials (replace with your own)
const char* ssid = "yourwifiname";       // Your Wi-Fi network name
const char* password = "wifipassword";   // Your Wi-Fi password

// 💡 LED control pin
const int ledpin = D4;  // GPIO2 (PWM-capable pin)

bool ledstate = true;   // LED toggle state
int brightness = 128;   // Initial brightness (0–255)

// 📟 LCD setup: I2C address 0x27, 16 columns x 2 rows
LiquidCrystal_I2C lcd(0x27, 16, 2);

// 🌐 TCP server setup
WiFiServer server(8080);  // Listen on port 8080
WiFiClient client;        // Incoming client connection

void setup() {
  lcd.begin();            // Initialize LCD
  lcd.backlight();        // Turn on LCD backlight
  pinMode(ledpin, OUTPUT);  // Set LED pin as output
  Serial.begin(115200);     // Start serial monitor for debugging

  // 📶 Connect to Wi-Fi
  WiFi.begin(ssid, password);
  int dotcount = 0;
  while (WiFi.status() != WL_CONNECTED) {
    lcd.setCursor(0, 0);
    lcd.print("Connecting..");
    lcd.setCursor(dotcount % 16, 1);
    lcd.print(".");
    dotcount++;
    delay(500);
  }

  // ✅ Wi-Fi connected
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WiFi Connected");
  lcd.setCursor(0, 1);
  lcd.print(WiFi.localIP());  // Show IP address
  delay(3000);

  server.begin();  // Start TCP server
}

void loop() {
  // 🔌 Accept new client if none connected
  if (!client || !client.connected()) {
    client = server.available();
  }

  // 📥 Read incoming message from client
  if (client.available()) {
    String msg = client.readStringUntil('\n');  // Read until newline
    Serial.println("Message Received: " + msg);

    // 🔆 Handle brightness command
    if (msg.startsWith("BRIGHTNESS:")) {
      String value = msg.substring(11);         // Extract value
      brightness = value.toInt();               // Convert to integer
      brightness = constrain(brightness, 0, 255);  // Clamp between 0–255
    }

    // 🔘 Handle toggle command
    if (msg.startsWith("TOGGLE:")) {
      String toggle = msg.substring(7);
      toggle.trim();                            // Remove whitespace
      ledstate = (toggle == "ON");              // Set LED state
    }

    // 💡 Apply brightness to LED
    analogWrite(ledpin, ledstate ? brightness : 0);

    // 📟 Update LCD display
    updatelcd();
  }
}

// 📟 LCD update function
void updatelcd() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("BRIGHTNESS:" + String(brightness));
  lcd.setCursor(0, 1);
  lcd.print("TOGGLE:" + String(ledstate ? "ON" : "OFF"));
}
