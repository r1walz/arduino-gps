#include <SoftwareSerial.h>
#define DEBUG true

String data[5];
SoftwareSerial sim808(7,8);
char phone_no[] = "+917340605885";
String state, timegps, latitude, longitude;

String sendData(String cmd, const int timeout, boolean debug) {
  String res = "";

  sim808.println(cmd);

  long int time = millis();
  int i = 0;

  while ((time + timeout) > millis()) {
    while (sim808.available()) {
      char c = sim808.read();
      res += c;
    }
  }

  if (debug) {
    Serial.print(res);
  }

  return res;
}

void sendTabData(String cmd, const int timeout, boolean debug) {
  sim808.println(cmd);

  long int time = millis();
  int i = 0;

  while ((time + timeout) > millis()) {
    while (sim808.available()) {
      char c = sim808.read();

      if (c != ',') {
        data[i] += c;
        delay(100);
      } else {
        ++i;
      }

      if (i == 5) {
        delay(100);
        goto leave;
      }
    }
  }

  leave:
    if (debug) {
      state = data[1];
      timegps = data[2];
      latitude = data[3];
      longitude = data[4];
    }
}

void setup() {
  sim808.begin(9600);
  Serial.begin(9600);
  delay(50);

  sim808.println("AT+CSMP=17,167,0,0");
  delay(100);

  sim808.println("AT+CMGF=1");
  delay(400);

  sendData("AT+CGNSPWR=1", 1000, DEBUG);
  delay(50);

  sendData("AT+CGNSSEQ=RMC", 1000, DEBUG);
  delay(150);
}

void loop() {
  sendTabData("AT+CGNSINF", 1000, DEBUG);

  if (longitude == "") {
    Serial.println("GPS Initializing...");
    return;
  }
  
  Serial.println("State: " + state);
  Serial.println("Time: " + timegps);
  Serial.println("Latitude: " + latitude);
  Serial.println("Longitude: " + longitude);

  sim808.print("AT+CMGS=\"");
  sim808.print(phone_no);
  sim808.println("\"");
  delay(300);

  sim808.print("Here is my location: ");
  sim808.print("https://www.google.com/maps/place/");
  sim808.print(latitude);
  sim808.print(",");
  sim808.print(longitude);
  delay(200);

  sim808.print((char) 26);
  delay(200);

  sim808.println();
  delay(20000);

  sim808.flush();
}
