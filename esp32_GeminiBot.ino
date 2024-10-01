#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

String apiKey;
String endpoint;
String userName;

void connectToWiFi(const char* ssid, const char* password) {
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(1000);

  WiFi.begin(ssid, password);

  int retries = 0;
  const int maxRetries = 30;
  while (WiFi.status() != WL_CONNECTED && retries < maxRetries) {
    delay(1000);
    Serial.print(".");
    retries++;
    delay(500);
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("");
    Serial.println("Error: Could not connect to WiFi network.");
  }
}

void loadAPIKey() {
  Serial.println("Please send the API key file (api.txt) over Serial:");
  
  while (!Serial.available());
  apiKey = Serial.readStringUntil('\n');
  apiKey.trim();
  endpoint = "https://generativelanguage.googleapis.com/v1beta/models/gemini-pro:generateContent?key=" + apiKey;
  Serial.println("API key loaded successfully.");
}

void saveAP(const char* ssid, const char* password) {
  Serial.println("Saving Access Point information...");
  String apData = String(ssid) + "//" + String(password) + "\n";
  Serial.print("Writing to SavedAPs.txt: ");
  Serial.println(apData);
}

bool loadSavedAP(String &ssid, String &password) {
  Serial.println("Loading saved Access Points from SavedAPs.txt...");
  
  while (!Serial.available());
  String apData = Serial.readStringUntil('\n');
  
  int separatorIndex = apData.indexOf("//");
  if (separatorIndex == -1) return false;

  ssid = apData.substring(0, separatorIndex);
  password = apData.substring(separatorIndex + 2);
  
  ssid.trim();
  password.trim();
  
  return true;
}

void setup() {
  Serial.begin(115200);
  delay(10);

  loadAPIKey();

  Serial.println("Welcome to the Gemini ESP32!");
  Serial.println("Please enter your name:");
  while (!Serial.available());
  userName = Serial.readStringUntil('\n');
  userName.trim();

  Serial.println("Hello, " + userName + "!");

  Serial.println("Scanning WiFi networks...");
  int n = WiFi.scanNetworks();
  Serial.println("Available WiFi networks:");
  for (int i = 0; i < n; ++i) {
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(WiFi.SSID(i));
    Serial.print(" (");
    Serial.print(WiFi.RSSI(i));
    Serial.println(")");
  }

  while (true) {
    Serial.println("Do you want to use a saved Access Point? (yes/no):");
    while (!Serial.available());
    String choice = Serial.readStringUntil('\n');
    choice.trim();

    if (choice.equalsIgnoreCase("yes")) {
      Serial.println("Please enter the SSID and password in the format 'SSID//password':");
      
      while (!Serial.available());
      String input = Serial.readStringUntil('\n');
      input.trim();

      int separatorIndex = input.indexOf("//");
      if (separatorIndex == -1) {
        Serial.println("Invalid format. Use 'SSID//password'. Try again.");
        continue;
      }

      String ssid = input.substring(0, separatorIndex);
      String password = input.substring(separatorIndex + 2);
      ssid.trim(); 
      password.trim(); 

      connectToWiFi(ssid.c_str(), password.c_str());
      break; 
    } else if (choice.equalsIgnoreCase("no")) {
      Serial.println("Select a WiFi network by entering the corresponding number or SSID followed by the password separated by '//':");

      while (!Serial.available());
      String input = Serial.readStringUntil('\n');
      input.trim();

      int separatorIndex = input.indexOf("//");
      if (separatorIndex == -1) {
        Serial.println("Invalid format. Use 'number//password' or 'SSID//password'. Try again.");
        continue;
      }

      String password = input.substring(separatorIndex + 2);
      password.trim();

      String ssidInput = input.substring(0, separatorIndex);

      int networkIndex = ssidInput.toInt() - 1;
      if (networkIndex >= 0 && networkIndex < n) {
        String ssid = WiFi.SSID(networkIndex);
        Serial.print("You selected: ");
        Serial.println(ssid);
        Serial.print("Connecting to ");
        Serial.println(ssid);
        connectToWiFi(ssid.c_str(), password.c_str());
      } else {
        Serial.print("Connecting to SSID: ");
        Serial.println(ssidInput);
        connectToWiFi(ssidInput.c_str(), password.c_str());
      }

      if (WiFi.status() == WL_CONNECTED) {
        saveAP(ssidInput.c_str(), password.c_str());
        break;
      } else {
        Serial.println("Failed to connect. Please try again.");
      }
    }
  }
}

void loop() {
  if (Serial.available() > 0) {
    String userQuery = Serial.readStringUntil('\n');
    userQuery.trim();

    if (userQuery.length() > 0) {
      if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;

        http.setTimeout(15000);
        http.begin(endpoint);
        http.addHeader("Content-Type", "application/json");

        String payload = "{\"contents\":[{\"parts\":[{\"text\":\"" + userQuery + "\"}]}]}";

        int httpResponseCode = http.POST(payload);

        if (httpResponseCode > 0) {
          String response = http.getString();

          DynamicJsonDocument doc(4096);
          DeserializationError error = deserializeJson(doc, response);

          if (!error) {
            const char* text = doc["candidates"][0]["content"]["parts"][0]["text"];
            Serial.println(userName + ": \"" + userQuery + "\""); 
            Serial.println("Gemini: \"" + String(text) + "\"");
          } else {
            Serial.print("Error parsing JSON: ");
            Serial.println(error.c_str());
          }
        } else {
          Serial.print("Request error: ");
          Serial.println(httpResponseCode);
          Serial.print("HTTP error: ");
          Serial.println(http.errorToString(httpResponseCode).c_str());
        }

        http.end();
      } else {
        Serial.println("WiFi connection error");
      }
    }
  }
}
