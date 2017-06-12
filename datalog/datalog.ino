
#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include <FreematicsONE.h>
#include <TinyGPS.h>
#include "config.h"
#include<string.h>

TinyGPS gps;
COBDSPI one;

File dataFiles;
bool firstFlag=true;

void setup() {
  byte ret;
  
  one.begin();
  Serial.begin(115200);
  delay(500);

  //GPS
//  Serial.print("Firm Vers ");
//  Serial.println(ver);
  Serial.print(F("GPS"));
  if (one.initGPS(GPS_SERIAL_BAUDRATE)) {
    Serial.println(F("OK"));
  }
  else 
  Serial.println(F("NO"));
    

  //SD Card
  pinMode(SS, OUTPUT);
  // see if the SD card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println(F("C Fail")); // C - Card
    // don't do anything more:
    while (1) ;
  }
  
  Serial.println(F("C OK"));   // C - Card

//  if(!SD.exists("/DataLog"))
//    SD.mkdir("/DataLog");

 // Open up the file we're going to log to!
  dataFiles = SD.open("datalog3.csv", FILE_WRITE);
  if (! dataFiles) {
    Serial.println(F("File error"));
    // Wait forever since we cant write data
    while (1) ;
  }
  else
    Serial.println(F("file OK"));
    
 


    Wire.begin();
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x6B);  // PWR_MGMT_1 register
    Wire.write(0);     // set to zero (wakes up the MPU-6050)
    Wire.endTransmission(true);

       // Open up the file we're going to log to!

 
  
}
void vin()
{
  String datas="";
  char vin[10];
      if (one.getVIN(vin, sizeof(vin))) {
          datas = datas + '#' + String(vin);
      }
      else
      { 
//        Serial.println(F("VIN not retrieved"));
        datas = datas + '#' + String("12345");
      }
      dataFiles.println(datas);
      Serial.println(datas);
  }
void showGPS()
{
    // parsed GPS data is ready
    uint32_t time;
    uint32_t date;
    String data = "";
    float AcX,AcY,AcZ;

    
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x3B);  // starting with register 0x3B (ACCEL_XOUT_H)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_addr,6,true); // request a total of 14 registers
    AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)    
    AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
    AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)

    AcX=AcX/acc4g;
    AcY=AcY/acc4g;
    AcZ=AcZ/acc4g;

    gps.get_datetime(&date, &time, 0);
    
    if(date==0)
    {
      Serial.print(F("Waiting for signal\n"));
    }
    else
    {
//      Serial.print(F("Date:"));
//      Serial.print(date);
//      Serial.print(F(" Time:"));
//      Serial.print(time);
  
      int32_t lat, lon;
      gps.get_position(&lat, &lon, 0);


//      Serial.print(F(" LAT:"));
//      Serial.print((float)lat / 100000,5);
//   
//      Serial.print(F(" LNG:"));
//      Serial.print((float)lon / 100000,5);
//      
//      Serial.print(F(" ALT:"));
//      Serial.print(gps.altitude() / 100);
//      Serial.print("m");
//  
//      Serial.print(F(" Speed:"));
//      Serial.print(gps.speed() * 1852 / 100000);
//      Serial.print(F("km/h"));
//  
//      Serial.print(F(" SAT:"));
//      Serial.print(gps.satellites());
//  
//      Serial.println();
//
//      Serial.print(F("AcX = ")); 
//      Serial.print(AcX,8);
//      Serial.print(F(" | AcY = ")); 
//      Serial.print(AcY,8);
//      Serial.print(F(" | AcZ = ")); 
//      Serial.print(AcZ,8);
      
      Serial.println();
  
      data = data + String(date) + "," + String(time) + "," + String((float)lat / 100000,5) + "," + String((float)lon / 100000,5) + "," + String(gps.altitude()/ 100) + "," + String(gps.speed()* 1852 / 100000) + "," + String(gps.satellites()) + "," + String(AcX,8) + "," + String(AcY,8) + "," + String(AcZ,8) + ",0";

      dataFiles.println(data);
      Serial.println(data);

    }
}

void loop() {
  
    char buf[130];
    //delay(10000);
    byte n = one.getGPSRawData(buf, sizeof(buf));
    if (n > 0) {
        bool updated = false;
        // need to skip heading ($GPS) and ending (>)
        for (byte m = 4; m < n - 1; m++) {
          if (!ready) Serial.write(buf[m]);
          if (gps.encode(buf[m])) {
            updated = true;
          }
        }
      if (!updated || updated) {
        ready = true;
        if(firstFlag)
        {
          
          vin();
          dataFiles.flush();
          firstFlag=false;
        }
        showGPS(); 
        dataFiles.flush();
      }
    }
  
}
