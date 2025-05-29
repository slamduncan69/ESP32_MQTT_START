#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

// ——— Wi-Fi & MQTT ———
const char* ssid          = "TP-Link_E689";
const char* pass          = "40913506";
const char* mqtt_server   = "192.168.0.236";
const uint16_t mqtt_port  = 1883;
const char* state_topic   = "M3/Stories/TE/State";
const char* mqtt_message  = "Start";

// ——— Pins ———
constexpr uint8_t BTN_PIN = 4;  // your button input (INPUT_PULLUP)
constexpr uint8_t LED_PIN = 2;  // button’s built-in LED (active-HIGH)

WiFiClient   wifiClient;
PubSubClient mqtt(wifiClient);

// Blink helper for active-HIGH LED
void blink(uint8_t times, uint16_t onTime=100, uint16_t offTime=100) {
  for (uint8_t i = 0; i < times; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(onTime);
    digitalWrite(LED_PIN, LOW);
    if (i < times - 1) delay(offTime);
  }
}

// MQTT reconnect & subscribe (no idle publish!)
void reconnectMQTT() {
  while (!mqtt.connected()) {
    Serial.print("MQTT connecting...");
    if (mqtt.connect("ESP32Button")) {
      Serial.println("connected");
      mqtt.subscribe(state_topic);
    } else {
      Serial.print(" failed, rc=");
      Serial.print(mqtt.state());
      Serial.println(" – retry in 1s");
      delay(1000);
    }
  }
}

// Handle incoming state messages
void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (unsigned int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }
  Serial.print("→ Msg ["); Serial.print(topic); Serial.print("]: ");
  Serial.println(msg);

  if (msg == "Start") {
    digitalWrite(LED_PIN, HIGH);    // turn ON
  }
  else if (msg == "Idle") {
    digitalWrite(LED_PIN, LOW);     // turn OFF
  }
  else {
    blink(5, 50, 50);               // error blink
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(BTN_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);       // assume broker’s Idle at start

  // 1) Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected");

  // 2) MQTT setup
  mqtt.setServer(mqtt_server, mqtt_port);
  mqtt.setCallback(callback);
  reconnectMQTT();
}

void loop() {
  if (!mqtt.connected()) reconnectMQTT();
  mqtt.loop();

  // Button press with debounce
  static uint32_t lastDebounce = 0;
  if (digitalRead(BTN_PIN) == LOW && millis() - lastDebounce > 250) {
    lastDebounce = millis();
    if (mqtt.publish(state_topic, mqtt_message, true)) {
      Serial.println("→ Start published");
      blink(2, 200, 200);         // flash twice
      digitalWrite(LED_PIN, HIGH);// leave ON
    } else {
      Serial.println("! Publish failed");
      blink(5, 50, 50);           // rapid error blinks
    }
  }
}
