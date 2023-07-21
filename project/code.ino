#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h> // GPS library

#define FIREBASE_HOST "https://absd-a02a7-default-rtdb.firebaseio.com/"                     //Your Firebase Project URL goes here without "http:" , "\" and "/"
#define FIREBASE_AUTH "Re**********************************A9FTiV" //Your Firebase Database Secret goes here
#define WIFI_SSID " WIFIName"                                               //WiFi SSID to which you want NodeMCU to connect
#define WIFI_PASSWORD "WIFIPassword"
// #define WIFI_SSID "InishkaIP"                                               //WiFi SSID to which you want NodeMCU to connect
// #define WIFI_PASSWORD "12345678"

// GPS module pins
#define GPS_RX_PIN D1
#define GPS_TX_PIN D2

TinyGPSPlus gps; // GPS object
SoftwareSerial mygps(D2,D1);

// Declare the Firebase Data object in the global scope
FirebaseData firebaseData;
const int trigPin = D6;
const int echoPin = D7;

void setup() {
  Serial.begin(9600);
  Serial.println("Serial communication started\n\n");

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);


  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println();
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP Address is: ");
  Serial.println(WiFi.localIP());

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
  delay(1000);

  Serial.println("Initializing GPS...");
  mygps.begin(9600); // Initialize GPS module
}

void loop() {
  while (mygps.available() > 0) {
    if (gps.encode(mygps.read())) {
      if (gps.location.isUpdated()) {
        long duration ;
        float distance ;
        float depth;
        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin, LOW);
        duration = pulseIn(echoPin, HIGH);
        distance = duration * 0.034 / 2;
        Serial.println("detecting pothole......");
        
        // Serial.println(distance);
        // Retrieve latitude and longitude values from the GPS module
        if(distance < 20 && distance > 10){
          depth = distance - 10;
          Serial.println("pothole found press the brakes , sending data to municipality!!");
          // Serial.println("depth of pothole is");
          // Serial.println(depth);
          delay(500);
          double latitude = gps.location.lat();
          double longitude = gps.location.lng();
          
        // Convert latitude and longitude to string
          String latitudeStr = String(latitude, 6);
          String longitudeStr = String(longitude, 6);
          String depthStr = String(depth, 2);

        // Update Firebase with latitude and longitude data
          if (Firebase.setString(firebaseData, "/GPS/latitude", latitudeStr) &&
              Firebase.setString(firebaseData, "/GPS/longitude", longitudeStr)&&
              Firebase.setString(firebaseData, "/GPS/depth", depthStr)) {
            Serial.println("Latitude, Longitude and Depth Uploaded Successfully");
            Serial.print("Latitude: ");
            Serial.println(latitudeStr);
            Serial.print("Longitude: ");
            Serial.println(longitudeStr);
            Serial.println("Depth of pothole is :");
            Serial.println(depthStr);
            Serial.println();
          } else {
            Serial.println(firebaseData.errorReason());
          }
          delay(5000);
        }else{
          Serial.println("pothole not found !, keep driving :) ");
          delay(5000);
        }

      }
    }
  }
}
