#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// WiFi credentials
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";


// UNIQUE DEVICE IDENTIFIERS (Change these for Device 2 and Device 3)
const String deviceid = "101";  // e.g., 101 for Dev 1, 102 for Dev 2
const String devicepass = "YOUR_DEVICE_PASSWORD";  // Password for this specific device


// Telegram Bot credentials
#define BOTtoken "YOUR_TELEGRAM_BOT_TOKEN"     // bot token after bot creation from @botfather


const int ledPin = 2; // Output pin for this device
int ledState = LOW;

bool isAuthenticated = false;

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);

   

    String text = bot.messages[i].text;

    // Command to select/login to this specific device
    if (text == "/login " + deviceid) {
      bot.sendMessage(chat_id, "Device " + deviceid + " selected. Enter password:", "");
      continue;
    }

    // Password verification for this device
    if (!isAuthenticated && text == devicepass) {
      isAuthenticated = true;
      String msg = "Device " + deviceid + " Authenticated!\n\nCommands:\n";
      msg += "/" + deviceid + "_on : Turn ON\n";
      msg += "/" + deviceid + "_off : Turn OFF\n";
      msg += "/" + deviceid + "_status : Check Status\n";
      msg += "/" + deviceid + "_logout : Logout";
      bot.sendMessage(chat_id, msg, "");
      continue;
    }

    // Commands specific to this device ID
    if (isAuthenticated) {
      if (text == "/" + deviceid + "_on") {
        digitalWrite(ledPin, HIGH);
        ledState = HIGH;
        bot.sendMessage(chat_id, "Device " + deviceid + " LED is ON", "");
      } 
      else if (text == "/" + deviceid + "_off") {
        digitalWrite(ledPin, LOW);
        ledState = LOW;
        bot.sendMessage(chat_id, "Device " + deviceid + " LED is OFF", "");
      } 
      else if (text == "/" + deviceid + "_status") {
        String statusMsg = digitalRead(ledPin) ? "ON" : "OFF";
        bot.sendMessage(chat_id, "Device " + deviceid + " is currently " + statusMsg, "");
      } 
      else if (text == "/" + deviceid + "_logout") {
        isAuthenticated = false;
        bot.sendMessage(chat_id, "Logged out from Device " + deviceid, "");
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  client.setInsecure();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
}

void loop() {
  if (millis() > lastTimeBotRan + botRequestDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}