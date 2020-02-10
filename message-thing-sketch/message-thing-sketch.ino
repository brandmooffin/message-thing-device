#include <LiquidCrystal_I2C.h> // https://www.losant.com/blog/how-to-connect-lcd-esp8266-nodemcu
#include <Wire.h>
#include <ESP8266WiFi.h> // http://arduino.esp8266.com/stable/package_esp8266com_index.json https://www.python.org/downloads/
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h> //https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/examples/HTTPSRequest/HTTPSRequest.ino
#include <ArduinoJson.h> //https://www.instructables.com/id/ESP8266-Parsing-JSON/ https://arduinojson.org/v6/assistant/ https://arduinojson.org/
// https://microcontrollerslab.com/esp8266-pinout-reference-gpio-pins/
// Construct an LCD object and pass it the
// I2C address, width (in characters) and
// height (in characters). Depending on the
// Actual device, the IC2 address may change.
LiquidCrystal_I2C lcd(0x27, 16, 2);

ESP8266WiFiMulti WiFiMulti;

const int ledPin = 12;
const int selectLeftButtonPin = 14;
const int selectRightButtonPin = 15;
const int confirmButtonPin = 13;

boolean hasMessage = false;

String messages[] = {"Sample message 1", "Sample message 2", "Longer sample message 3"};
int messagesCount = 3;
int selectedMessage = 0;
String messageToSend = "";
const char* receivedMessage = "";
String receivedMessageId = "";


String host = "<api-base-url>";
const int httpsPort = 443;
String messagingEndpoint = "/api/messaging";
String deviceId = "TestDevice1";
String toDeviceId = "TestDevice2";

void setup() {

  Serial.begin(9600);

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("<SSID>", "<pass>");

  pinMode(confirmButtonPin, INPUT);
  pinMode(selectLeftButtonPin, INPUT);
  pinMode(selectRightButtonPin, INPUT);
  pinMode(ledPin, OUTPUT);

  Wire.begin();
  // The begin call takes the width and height. This
  // Should match the number provided to the constructor.
  lcd.begin(16, 2);
  lcd.init();

  // Turn on the backlight.
  lcd.backlight();

  // Move the cursor characters to the right and
  // zero characters down (line 1).
  lcd.setCursor(5, 0);

  // Print HELLO to the screen, starting at 5,0.
  lcd.print("It's");

  // Move the cursor to the next line and print
  // WORLD.
  lcd.setCursor(5, 1);
  lcd.print("Alive!");
}

void loop() {
  int confirmButtonPinVal = digitalRead(confirmButtonPin);
  int selectLeftButtonPinVal = digitalRead(selectLeftButtonPin);
  int selectRightButtonPinVal = digitalRead(selectRightButtonPin);
  Serial.println(confirmButtonPinVal);

  if (selectLeftButtonPinVal == HIGH) {
    selectedMessage -= 1;
    if (selectedMessage < 0) {
      selectedMessage = messagesCount - 1;
    }
    Serial.println("Selection left");
    lcd.clear();
    lcd.setCursor(0, 0);
    messageToSend = messages[selectedMessage];
    setMessage(messageToSend);
  }

  if (selectRightButtonPinVal == HIGH) {
    selectedMessage += 1;
    if (selectedMessage >= messagesCount) {
      selectedMessage = 0;
    }
    Serial.println("Selection right");
    lcd.clear();
    lcd.setCursor(0, 0);
    messageToSend = messages[selectedMessage];
    setMessage(messageToSend);
  }

  if (confirmButtonPinVal == HIGH) {
    if (hasMessage) {
      hasMessage = false;
      clearMessages();
      lcd.clear();

      // Move the cursor characters to the right and
      // zero characters down (line 1).
      lcd.setCursor(5, 0);

      // Print HELLO to the screen, starting at 5,0.
      lcd.print("No");

      // Move the cursor to the next line and print
      // WORLD.
      lcd.setCursor(2, 1);
      lcd.print("Messages");
      Serial.println("Clear messages");
    } else {
      if (!messageToSend.equals("")) {
        //sendMessage
        sendMessage();
      }
    }
  }

  if (!hasMessage) { //checks if object is there or not
    digitalWrite(ledPin, LOW);
  } else {
    digitalWrite(ledPin, HIGH);
  }

  checkMessages();

  delay(5000);
}

void checkMessages() {
  hasMessage = false;
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    // Use WiFiClientSecure class to create TLS connection
    WiFiClientSecure client;
    client.setInsecure();
    Serial.print("connecting to : '");
    Serial.print(host);
    Serial.println("'");

    if (!client.connect(host, httpsPort)) {
      Serial.println("connection failed");
      return;
    }

    Serial.print("requesting URL: '");
    Serial.print(messagingEndpoint);
    Serial.println("'");

    client.print(String("GET ") + messagingEndpoint + "/" + deviceId + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");

    Serial.println("request sent");
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
    }
    Serial.println("reply was:");
    Serial.println("==========");
    String line;
    int responseLine = 1;

    while (client.available()) {
      line = client.readStringUntil('\n');  //Read Line by Line
      Serial.println(line); //Print response
      String tempLine(line);
      if (tempLine.indexOf("text") > 0) {
        const size_t capacity = JSON_OBJECT_SIZE(4) + 100;
        DynamicJsonDocument doc(capacity);
        deserializeJson(doc, line);
        const char* potentialMessageId = doc["id"];
        String tempMessageId(receivedMessageId);
        String tempPotentialMessageId(potentialMessageId);
        Serial.println(tempPotentialMessageId);
        if (!tempMessageId.equals(tempPotentialMessageId)) {
          Serial.println(tempMessageId);
          receivedMessageId = tempPotentialMessageId;
          receivedMessage = doc["text"];
          hasMessage = true;
          setMessage(receivedMessage);
          Serial.println(receivedMessage);
        }
      }
      responseLine++;
    }
    Serial.println("==========");
    Serial.println("closing connection");
  }
  delay(5000);
}

void setMessage(String message) {
  int max_lines = 2;
  int max_per_line = 16;
  String line1 = "";
  String line2 = "";
  for (int i = 0; i < message.length(); i++) {
    if (i < 16) {
      line1 += message[i];
    } else if (i < 32) {
      line2 += message[i];
    }
  }
  lcd.clear();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
}

void sendMessage() {
  Serial.print("Send message'");
  // Use WiFiClientSecure class to create TLS connection
  WiFiClientSecure client;
  client.setInsecure();
  Serial.print("connecting to : '");
  Serial.print(host);
  Serial.println("'");

  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }

  Serial.print("requesting URL: '");
  Serial.print(messagingEndpoint);
  Serial.println("'");

  const size_t capacity = JSON_OBJECT_SIZE(3);
  DynamicJsonDocument doc(capacity);

  String json;
  doc["toDevice"] = toDeviceId.c_str();
  doc["fromDevice"] = deviceId.c_str();
  doc["text"] = messageToSend.c_str();

  serializeJson(doc, json);

  Serial.println(json);

  HTTPClient http;

  // Send request
  http.begin(client, "https://"+ host + messagingEndpoint);
  http.addHeader("Content-Type", "application/json");
  http.POST(json);
  
  // Read response
  Serial.print(http.getString());
  
  // Disconnect
  http.end();

  Serial.println("request sent");
  delay(5000);
}

void clearMessages() {
  // wait for WiFi connection
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    // Use WiFiClientSecure class to create TLS connection
    WiFiClientSecure client;
    client.setInsecure();
    Serial.print("connecting to : '");
    Serial.print(host);
    Serial.println("'");

    if (!client.connect(host, httpsPort)) {
      Serial.println("connection failed");
      return;
    }

    Serial.print("requesting URL: '");
    Serial.print(messagingEndpoint);
    Serial.println("'");

    Serial.print(receivedMessageId);
    Serial.print(receivedMessageId.length());

    client.print(String("DELETE ") + messagingEndpoint + "/" + receivedMessageId + " HTTP/1.1\r\n" +
                 "Host: " + host + "\r\n" +
                 "Connection: close\r\n\r\n");

    Serial.println("request sent");
    while (client.connected()) {
      String line = client.readStringUntil('\n');
      if (line == "\r") {
        Serial.println("headers received");
        break;
      }
    }
    Serial.println("reply was:");
    Serial.println("==========");
    String line;
    while (client.available()) {
      line = client.readStringUntil('\n');  //Read Line by Line
      Serial.println(line); //Print response
    }
    Serial.println("==========");
    Serial.println("closing connection");
  }
  delay(5000);
}
