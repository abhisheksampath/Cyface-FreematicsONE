#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <FreematicsONE.h>
#include<SD.h>


#define WIFI_SSID "sam"
#define WIFI_PASSWORD "12345asd"
#define SERVER_URL "192.168.137.1"
#define SERVER_PORT 8080
#define CS_PIN 10

COBDSPI one;

File file;

char buffer[220];
void Openfile();

byte sendCommand(const char* cmd, int timeout = 2000, const char* expected1 = "OK", const char* expected2 = 0)
{
  if (cmd) {
    one.xbWrite(cmd);
    delay(50);
  }
  return one.xbReceive(buffer, sizeof(buffer), timeout, expected1, expected2);
}


void setup() {
  byte ret;
  
  one.begin();
  Serial.begin(115200);
  delay(500);
  // set xBee module baudrate
  Serial.print(F("Setting xBee baudrate..."));
  if (one.xbBegin(9600) ) {
    Serial.println(F("OK"));
  } else {
    Serial.println(F("Failed"));
    for (;;); 
  }
    
  bool success = false;
//  Serial.print("Checking ESP8266...");
//  for (byte n = 0; !(success = sendCommand("AT\r\n")) && n < 10; n++) {
//    delay(100);
//    Serial.print('.');
//  }
//  if (success) {
//    Serial.println("OK"); 
//  } else {
//    Serial.println("failed"); 
//  }
//  delay(100);
//  
/*
  Serial.print("Resetting ESP8266...");
  if (sendCommand("AT+RST\r\n", 5000, "Ready")) {
    Serial.println("OK"); 
  } else {
    Serial.println("dead"); 
  }
*/
//


  Serial.print(F("Entering station mode..."));
  sprintf_P(buffer, PSTR("AT+CWMODE=1\r\n"));
  if (sendCommand(buffer, 2000, "OK", "no change")) {
    Serial.println(F("OK")); 
  } else {
    Serial.println(F("dead")); 
  }
  delay(100);

  Serial.print(F("Set single link..."));
  sprintf_P(buffer, PSTR("AT+CIPMUX=0\r\n"));
  if (sendCommand(buffer))
    Serial.println(F("OK"));
  else
    Serial.println(F("failed"));
  
  Serial.print(F("Connecting AP (SSID:"));
  Serial.print(WIFI_SSID);
  Serial.print(F(")..."));
  sprintf_P(buffer, PSTR("AT+CWJAP=\"%s\",\"%s\"\r\n"), WIFI_SSID, WIFI_PASSWORD);
  ret = sendCommand(buffer, 20000, "OK", "ERROR");
  if (ret == 1) {
    // get IP address
    if (sendCommand("AT+CIFSR\r\n", 2000, "OK")) {
//      char *s = strstr(buffer, "CIFSR");
//      if (s) {
//        s += 8;
//        char *p = strchr(s, '\r');
//        if (p) *p = 0;
        Serial.println(F("OK"));
//      }
    } else {
      Serial.println(F("N/A")); 
    }
  } else {
    Serial.println(F("failed")); 
    for (;;);
  }
  
  Serial.print(F("Connecting to "));
  Serial.print(SERVER_URL);
  Serial.print(F("..."));
  sprintf_P(buffer, PSTR("AT+CIPSTART=\"TCP\",\"%s\",%d\r\n"), SERVER_URL, SERVER_PORT);
  if (sendCommand(buffer, 10000, "Linked")) {
    Serial.println(F("OK"));
  } 
  else {
    Serial.println(F("failed"));
  }

  Openfile();
}

void Openfile()
{


  bool Flag;
  bool Flag2=false;     
  char path[8] = "/tests";
  char charBuff[80];
  unsigned int index=0;
  int counter=0;
  int errFlag=0;
   
  if (!SD.begin(CS_PIN)) 
    Serial.println(F("begin failed"));
  else
    Serial.println(F("SD OK"));

  // Create or open the file.
  file = SD.open(F("datalog.csv"), FILE_WRITE);
  if (!file) 
    Serial.println(F("open failed"));
  else
    Serial.println(F("File opened"));
  
  // Rewind the file for read.
  file.seek(0);   
      
    while (Flag=file.available()) {

//      buffer[0]=0; 
      char header[155];
      char *p = header;
      char someChar = file.read();
      
      
      if(someChar != '\n' && someChar != '\r' && errFlag==0)
      {
       charBuff[index++]=someChar; 
      }
      else
      {
       if(index > 0)
          {
          if(errFlag==0)
            charBuff[index]='\0';
          
          Serial.println(charBuff);
          counter++;
          
  
          p += sprintf_P(p, PSTR("%s %s HTTP/1.1\r\nUser-Agent: ONE\r\nHost: %s\r\nConnection: %s\r\nKeep-Alive: timeout=2000, max=100000\r\n"),"POST", path, SERVER_URL, "keep-alive");
          Serial.println(index);
          p += sprintf_P(p, PSTR("Content-length: %u\r\n"), index);
    
          p += sprintf_P(p, PSTR("\r\n"));
    
          sprintf_P(buffer, PSTR("AT+CIPSEND=%u\r\n"), (unsigned int)(p - header) + index);
          if (sendCommand(buffer, 2000, ">")) {
            // send HTTP header
            one.xbWrite(header);
            delay(50);
            // send POST payload if any
            
            if (charBuff) one.xbWrite(charBuff);
            if (one.xbReceive(buffer, sizeof(buffer), 2000, "SEND OK")) {             
//              Serial.println(F("Data sent"));
            }        
            errFlag=0;
            Flag2=true;
          }
          else
          {
            Serial.println(F("Error"));
            counter=150;  //To reset the tcp conection
          }
          Serial.println(buffer);
          
          if(counter>99){
            Serial.println("RE");
            sprintf_P(buffer, PSTR("AT+CIPCLOSE\r\n"));
            sendCommand(buffer);
            counter=0;
            errFlag=1;

            delay(500);
             
            sprintf_P(buffer, PSTR("AT+CIPSTART=\"TCP\",\"%s\",%d\r\n"), SERVER_URL, SERVER_PORT);
            if(sendCommand(buffer, 10000, "Linked"))
            {
              Serial.println(F("L")); //Linked.
              } 
              else
              {
              Serial.println(F("NL")); //Not linked.
              } 
              
          }
         }

       if(errFlag==0)
       {         
         index = 0;
         charBuff[0]=0;
       }
       buffer[0] = 0;
       delay(100);
  
       }
     }

    if(Flag2 && !Flag)
    {
     SD.remove("datalog.csv");
     Serial.println(F("Done"));
    }
    }
void loop()
{
  }
