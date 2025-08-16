#include <WiFi.h>

const char *ssid = "ESP32-Flame";
const char *password = "12345678";

const int ledPin = 22;       // GPIO pin connected to LED
const int buzzerPin = 23;    // GPIO pin connected to buzzer
const int pushButtonPin = 25; // GPIO pin connected to push button
const int serverPort = 80;    // Server port

WiFiClient client;

void setup() {
  Serial.begin(115200);

  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  pinMode(pushButtonPin, INPUT_PULLUP);

  // Connect to Wi-Fi
  connectToWiFi();
}

void loop() {
  // Check if Wi-Fi connection is lost and reconnect if necessary
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connection lost. Reconnecting...");
    connectToWiFi();
  }

  // Check for client connection
  if (client.connected()) {
    while (client.available() > 0) {
      // Read the received data as a string
      String receivedData = client.readStringUntil('\n');
      Serial.println(receivedData);

      // Split the data into individual values
      int flameValue = receivedData.substring(0, receivedData.indexOf(',')).toInt();
      receivedData.remove(0, receivedData.indexOf(',') + 1);
      float temperatureValue = receivedData.substring(0, receivedData.indexOf(',')).toFloat();
      receivedData.remove(0, receivedData.indexOf(',') + 1);
      int gasValue = receivedData.toInt();

      // Check conditions and control LED and buzzer
      if (flameValue == 0 || temperatureValue > 40.0 || gasValue > 700) {
        digitalWrite(ledPin, HIGH);    // Turn on LED
        digitalWrite(buzzerPin, HIGH); // Turn on buzzer
      } else {
        digitalWrite(ledPin, LOW);     // Turn off LED
        digitalWrite(buzzerPin, LOW);  // Turn off buzzer
      }
    }
  } else {
    Serial.println("Connection lost. Reconnecting...");
    
    // Attempt to reconnect to the sender ESP32
    IPAddress senderIP(192, 168, 4, 1); // Change to the IP address of the sender ESP32
    client.connect(senderIP, serverPort);
  }

  // Check the state of the push button
  if (digitalRead(pushButtonPin) == LOW) {
    // Reset the circuit or perform an action
    resetActions();
  }

  delay(1000); // Adjust delay according to your requirements
}

// Function to connect to Wi-Fi
void connectToWiFi() {
  WiFi.begin(ssid, password);
  int attempts = 0;

  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("Connected to WiFi. IP Address: ");
    Serial.println(WiFi.localIP());

    // Set up the client to connect to the sender ESP32
    IPAddress senderIP(192, 168, 4, 1); // Change to the IP address of the sender ESP32
    client.connect(senderIP, serverPort);
  } else {
    Serial.println("Failed to connect to WiFi. Please check your credentials and connection.");
  }
}

// Function to reset LED and buzzer
void resetActions() {
  digitalWrite(ledPin, LOW);    // Turn off LED
  digitalWrite(buzzerPin, LOW); // Turn off buzzer
}
