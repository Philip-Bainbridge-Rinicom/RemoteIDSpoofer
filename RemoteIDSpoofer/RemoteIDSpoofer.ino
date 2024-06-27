// ESP8266 RemoteID spoofer

#include "frontend.h"
#include "spoofer.h"
#include <string.h>
#include <stdio.h>

// for some reason I can't get vector to work so... this hacky thing for now...
int num_spoofers = 0;
Spoofer spoofers[16];

String ReadPos; 
double Lat;
double Lon;

char incomingByte = 0;
String str;
const byte numChars = 100;
char receivedChars[numChars];
char transmitChars[numChars];
boolean newData = false;
static boolean recvInProgress = false;
static byte ndx = 0;
char startMarker = '#';
char endMarker = '\r';
char rc;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9699, SERIAL_8N1, 17, 18);
  pinMode(2, INPUT);
  // start the frontend and don't exit until either timer elapse or user ends it
  // the timer here is set at 2 minutes
  Frontend frontend(120000);
  while (!frontend.do_spoof) {
    frontend.handleClient();
  }

  // instantiate the spoofers and change locations
  Serial.println("Starting Spoofers");
  num_spoofers = frontend.num_drones;
  for (int i = 0; i < num_spoofers; i++) {
    spoofers[i].init();
    spoofers[i].updateLocation(frontend.latitude, frontend.longitude);
  }
}

void loop() {
  // do the spoofing
  /*if (Serial.available() > 0) {
    ReadPos = Serial.readString();
    ReadPos.trim();
      if(ReadPos.startsWith("LAT")){
        Lat = ReadPos.toCharArray;
        //Lat == ReadPos;
        //Serial.print(Lat);
        //Lat.remove(0, 3);
        //Serial.print(Lat.toDouble());
      }
      else if(ReadPos.startsWith("LON")){
        //Lon == ReadPos;
        //Serial.print(Lon);
        //Serial.print(Lon.toDouble());
      }
  }*/

  while (Serial1.available() > 0 ) {
                rc = Serial1.read();
                //Serial.print(rc);
                if (recvInProgress == true) {
                        if (rc != endMarker) {
                                receivedChars[ndx] = rc;
                                ndx++;
                                if (ndx >= numChars) {
                                        ndx = numChars - 1;
                                }
                        } else {
                                receivedChars[ndx] = '\0'; // terminate the string
                                recvInProgress = false;
                                ndx = 0;
                                newData = true;
                                //Serial.print('.');
                                //break;
                        }
                } else if (rc == startMarker) {
                        recvInProgress = true;
                }

                if (newData == true) {
                  if(receivedChars[1] == 'A'){
                      Lat = atof(receivedChars + 3);
                      //Serial.print(Lat);
                  }
                  else if(receivedChars[1] == 'O'){
                      Lon = atof(receivedChars + 3);
                      //Serial.print(Lon);
                  }
                }
                newData = false;
                //strcpy(transmitChars, receivedChars);
                for (int i = 0; i < num_spoofers; i++) {
                spoofers[i].updateLocation(Lat, Lon);
  }
  }
 Serial1.print(Lat);
 Serial1.print(Lon);
                
  if(digitalRead(2) == 1){              
  
  
  
  for (int i = 0; i < num_spoofers; i++) {
    spoofers[i].update();
    delay(200 / num_spoofers);
  }
  }
}
