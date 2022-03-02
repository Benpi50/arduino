// Import required libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
///////////////////////////////////////////////////////////////////////////////
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
///////////////////////////////////////////////////////////////////////////////
// Replace with your network credentials
const char* ssid = "SFR_A1D0";
const char* password = "Granville";

const char* serverName = "http://192.168.1.97/Meteo/postStation.php";
String apiKeyValue = "tPmAT5Ab3j7F9";

#define DHTPIN 2     // Digital pin connected to the DHT sensor

#define DHTTYPE    DHT22     // DHT 22 (AM2302)
DHT dht(DHTPIN, DHTTYPE);

// current temperature & humidity, updated in loop()
float t = 0.0;
float h = 0.0;

// Replaces placeholder with DHT values
//String processor(const String& var){
//  //Serial.println(var);
//  if(var == "TEMPERATURE"){
//    return String(t);
//  }
//  else if(var == "HUMIDITY"){
//    return String(h);
//  }
//  return String();
//}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  dht.begin();
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  delay(2000);
  if(WiFi.status()== WL_CONNECTED){
    // Print ESP8266 Local IP Address
    Serial.println(WiFi.localIP());
  }
   
   if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 
   { // Address 0x3D for 128x64
    Serial.println(F("SSD1306 allocation failed"));
   }

  delay(2000);
  display.clearDisplay();

}
 
void loop(){  

    float newT = dht.readTemperature();
    // if temperature read failed, don't change t value
    if (isnan(newT)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      t = newT;
      Serial.println(t);
    }
    // Read Humidity
    float newH = dht.readHumidity();
    // if humidity read failed, don't change h value 
    if (isnan(newH)) {
      Serial.println("Failed to read from DHT sensor!");
    }
    else {
      h = newH;
      Serial.println(h);
    }

  displayMeasure(t,h);
  if(WiFi.status()== WL_CONNECTED){
    sendMeasure(t,h);
      }
  else {
    Serial.println("WiFi Disconnected");
  }
  
  //Send an HTTP POST request every 30 seconds
  delay(10000);

}


void displayMeasure(float T, float H){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 10);
  display.print(String(T));
  display.setTextSize(1);
  display.print(" ");
  display.cp437(true);
  display.write(167);
  display.print("C");
  display.setCursor(0, 30);
  display.setTextSize(2);
  display.print(String(H));
  display.setTextSize(1);
  display.print(" %");
  display.display(); 
}

void sendMeasure(float T, float H){
  
//Check WiFi connection status
  
    WiFiClient client;
    HTTPClient http;
    
    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);
    
    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    // Prepare your HTTP POST request data
    String httpRequestData = "api_key=" + apiKeyValue + "&temperature=" + String(T)
                          + "&humidite=" + String(H) + "&piece=" + "Salon"
                          + "";
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);
    
    // Send HTTP POST request
    int httpResponseCode = http.POST(httpRequestData);

    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();

}
