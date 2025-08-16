#include <WiFi.h>
#include <OneWire.h>
#include <DallasTemperature.h>

int FlameSensor = 22;
int GasSensor = 34;
int TemperatureSensor = 21;

const char *ssid = "ESP32-Flame";
const char *password = "12345678";

WiFiServer server(80);
WiFiClient client;

// Data wire is connected to the Arduino digital pin 4
#define ONE_WIRE_BUS 21
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

int gasThreshold = 700;

void setup() {
  Serial.begin(9600);
  sensors.begin();
  pinMode(FlameSensor, INPUT);
  pinMode(TemperatureSensor, INPUT);
  pinMode(GasSensor, INPUT); // Ensure this is set for analog readings

  // Connect to Wi-Fi
  WiFi.softAP(ssid, password);
  delay(100);
  server.begin();

  // Baseline calibration for the gas sensor
  long baselineGasValue = 0;
  for (int i = 0; i < 20; i++) {
    baselineGasValue += analogRead(GasSensor);
    delay(50); // Short delay between readings
  }
  baselineGasValue /= 20; // Average
  Serial.print("Baseline Gas Value: ");
  Serial.println(baselineGasValue);

  // Adjust the gas threshold based on baseline
  // This is a starting point; you may need to adjust this based on further testing
  gasThreshold = baselineGasValue + 200; // Adjust value based on environment and testing
}

void loop() {
  client = server.available();

  if (client) {
    while (client.connected()) {
      int flameValue = digitalRead(FlameSensor);
      sensors.requestTemperatures();
      float tempC = sensors.getTempCByIndex(0);
      int gasValue = analogRead(GasSensor);

      
      // Define the threshold for gas detection
      // Adjust based on calibration and testing
      // const int gasThreshold = 700; // This line is commented out because gasThreshold is already declared globally

      // Checking the gas value against the threshold
      bool gasDetected = gasValue > gasThreshold;

      // Send data to the receiver
      client.print(flameValue);
      client.print(",");
      client.print(tempC);
      client.print(",");
      client.println(gasValue);

      if (gasDetected) {
        Serial.println("Gas anomaly detected!");
      }

      delay(10); // Small delay for stability
    }

    client.stop();
  }

  delay(1000); // Delay between checks
}
