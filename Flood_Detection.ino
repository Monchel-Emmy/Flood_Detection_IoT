#define BLYNK_TEMPLATE_ID "TMPL2LKiO5eBz"
#define BLYNK_TEMPLATE_NAME "Flood Detection"
#define BLYNK_AUTH_TOKEN "i1cjWEjvwemP4aLouya_VE-CMLWY_dAA"
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <UrlEncode.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

char ssid[] = "Monchel";
char pass[] = "Monchel1";

// Pins
#define RAIN_SENSOR D2           // Use DO pin from raindrop sensor
#define WATER_SENSOR A0          // AO from water level sensor
#define BUZZER D5

int buzzerStatus = 0;

void setup() {
  Serial.begin(115200);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(BUZZER, LOW);

  pinMode(RAIN_SENSOR, INPUT);  // Digital pin for rain

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
}

String phoneNumber = "+250780904149";
String apiKey = "6737250";
void sendMessage(String message){
  String url = "http://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&apikey=" + apiKey + "&text=" + urlEncode(message);

  WiFiClient client;
  HTTPClient http;
  http.begin(client, url);

  // Use GET instead of POST
  int httpResponseCode = http.GET(); 

  if (httpResponseCode == 200){
    Serial.println("Message sent successfully");
  }
  else{
    Serial.println("Error sending the message");
    Serial.print("HTTP response code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

// Send data to Blynk every second
bool alertSent = false;

void sendSensorData() {
  int rainValue = digitalRead(RAIN_SENSOR);
  int waterValue = analogRead(WATER_SENSOR);

  Blynk.virtualWrite(V0, rainValue == LOW ? 1 : 0);
  Blynk.virtualWrite(V1, waterValue);

  Serial.print("Rain: ");
  Serial.print(rainValue == LOW ? "YES" : "NO");
  Serial.print(" | Water Level: ");
  Serial.println(waterValue);

  if (rainValue == LOW && waterValue > 600 && !alertSent) {
    digitalWrite(BUZZER, HIGH);
    sendMessage("⚠️ Flood alert! Take precautions.");
    alertSent = true; // Prevent spamming
  } else if (rainValue == HIGH || waterValue <= 550) {
    digitalWrite(BUZZER, buzzerStatus);
    alertSent = false; // Reset when situation is normal
  }
}

// Blynk switch to control buzzer
BLYNK_WRITE(V2) {
  buzzerStatus = param.asInt();
  digitalWrite(BUZZER, buzzerStatus);
}


void loop() {
  Blynk.run();
  sendSensorData();
  delay(1000);
}
