// Include required libraries
//#include <TinyGPS++.h>
#include <TinyGPSPlus.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <SoftwareSerial.h>



SoftwareSerial mygps(21,19); //Tx, Rx
// Define WiFi credentials
#define WIFI_SSID "JESUS IS LORD"
#define WIFI_PASSWORD "jesusislord123"

// Define Firebase API Key, Project ID, and user credentials
#define API_KEY "AIzaSyBezWOG8Lp2wRU8Odfos23aj5X9WXNB1qY"
#define FIREBASE_PROJECT_ID "gpstracking-130b8"
#define USER_EMAIL "tomypop24@gmail.com"
#define USER_PASSWORD "childline"

// Define Firebase Data object, Firebase authentication, and configuration
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
TinyGPSPlus gps; // The TinyGPS++ object

void setup() {
  Serial.begin(9600);
  mygps.begin(9600);
  //Serial1.begin(9600, SERIAL_8N1, RX1PIN, TX1PIN);
  Serial.println(F("Arduino - GPS module"));
 
  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Print Firebase client version
  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  // Assign the API key
  config.api_key = API_KEY;

  // Assign the user sign-in credentials
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  // Assign the callback function for the long-running token generation task
  config.token_status_callback = tokenStatusCallback;  // see addons/TokenHelper.h

  // Begin Firebase with configuration and authentication
  Firebase.begin(&config, &auth);

  // Reconnect to Wi-Fi if necessary
  Firebase.reconnectWiFi(true);
}

void loop() {
  // Define the path to the Firestore document
  String documentPath = "GPSdata/NEO6M";

  // Create a FirebaseJson object for storing data
  FirebaseJson content;

  // Read Longitude,Latitude and Date from the DHT sensor
  float Latitude = gps.location.lat();
  float Longitude = gps.location.lng();
  float date_time = gps.date.month() + gps.date.year() + gps.date.day() ;
  float speed = gps.speed.kmph();
  float course_deg = gps.course.deg();


  // Print Latitude and Longitude values
  Serial.print("latitude:");
  Serial.println(Latitude);
  Serial.print("Longitude");
  Serial.println(Longitude);
  Serial.print("date_time");
  //Serial.println(date_time);
  //Serial.print("date_time");
  //Serial.println(date_time);

  

  // Check if the values are valid (not NaN)
  if (!isnan(Latitude) && !isnan(Longitude)) {
    // Set the 'Latitude' and 'Longitude' fields in the FirebaseJson object
    content.set("fields/Latitude/stringValue", String(Latitude, 2));
    content.set("fields/Longitude/stringValue", String(Longitude, 2));
    content.set("fields/date_time/stringValue", String(date_time, 2));
    content.set("fields/speed/stringValue", String(speed, 2));
    content.set("fields/course_deg/stringValue", String(course_deg, 2));

    Serial.print("Update/Add GPS Data... ");

    // Use the patchDocument method to update the Latitude and Longitude Firestore document
    if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "Latitude") 
    && Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "Longitude") 
    && Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "date_time")
    && Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "speed")
    && Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "course_deg")) {
      
      Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
      } else {
            Serial.println(fbdo.errorReason());
          }
        } else {
          Serial.println("Failed to read GPS data.");
        }

  // Delay before the next reading
  delay(5000);


if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
}
