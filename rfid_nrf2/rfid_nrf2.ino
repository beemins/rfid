#include <stdio.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include "printf.h"
#include <SPI.h>
#include <RFID.h>

RF24 radio(8,7); // (CE,CSN)
uint64_t writing_pipe = 0xF0F0F0F0E6LL;

//rc522
#define SS_PIN 10
#define RST_PIN 9

RFID rfid(SS_PIN, RST_PIN);

int type; //type zone place or event
unsigned char zone1[5] = {52, 161, 158, 221, 214};
unsigned char zone2[5] = {224, 251, 107, 2, 114};
unsigned char place1[5] = {173, 147, 214, 121, 145};
unsigned char place2[5] = {77, 71, 211, 121, 160};
unsigned char place3[5] = {93, 254, 203, 200, 160};
unsigned char place4[5] = {77, 153, 212, 121, 121};
unsigned char place5[5] = {61, 246, 203, 200, 200};
unsigned char place6[5] = {141, 134, 212, 121, 166};
unsigned char event1[5] = {109, 158, 212, 121, 94};
unsigned char event2[5] = {77, 164, 214, 121, 70};
int bZone1 = 1;
int bZone2 = 1;
int bPlace1 = 1;
int bPlace2 = 1;
int bPlace3 = 1;
int bPlace4 = 1;
int bPlace5 = 1;
int bPlace6 = 1;
int bEvent1 = 1;
int bEvent2 = 1;
int bEvent3 = 1;
int bEvent4 = 1;
int endZone1 = 0;
int endZone2 = 0;
int endPlace3 = 0;
int endPlace6 = 0;
int led1 = 5; // orange
int led2 = 6; // red

// Setup variables:
int serNum0;
int serNum1;
int serNum2;
int serNum3;
int serNum4;   

void setup()
{  
  Serial.begin(9600);
  SPI.begin(); 
  rfid.init();
  
  printf_begin();
  radio.begin();
  radio.setPayloadSize(32);
  radio.setDataRate(RF24_1MBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setChannel(66);
  radio.setRetries(15,15);
  radio.openWritingPipe(writing_pipe);
  radio.startListening();
  radio.printDetails();
  
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  Serial.print("\nStart connection");
}

void loop()
{
  while(!rfid.isCard());
  if(rfid.readCardSerial()) 
  {
    if(!(rfid.serNum[0] == serNum0
       && rfid.serNum[1] == serNum1
       && rfid.serNum[2] == serNum2
       && rfid.serNum[3] == serNum3
       && rfid.serNum[4] == serNum4))
      {
        /* With a new cardnumber, show it. */
        Serial.println(" ");
        Serial.println("\nCard found!");
        serNum0 = rfid.serNum[0];
        serNum1 = rfid.serNum[1];
        serNum2 = rfid.serNum[2];
        serNum3 = rfid.serNum[3];
        serNum4 = rfid.serNum[4];
                
        /*Serial.println(" ");
        Serial.println("Cardnumber:");
        Serial.print("Dec: ");
        Serial.print(rfid.serNum[0],DEC);
        Serial.print(", ");
        Serial.print(rfid.serNum[1],DEC);
        Serial.print(", ");
        Serial.print(rfid.serNum[2],DEC);
        Serial.print(", ");
        Serial.print(rfid.serNum[3],DEC);
        Serial.print(", ");
        Serial.print(rfid.serNum[4],DEC);
        Serial.println(" ");*/
        
        //check card type          
        type = cardType(serNum1);
        
        //zone card        
        if(type == 1)
        {
          if(serNum1 == 161 && endPlace3 == 0)
          {
            Serial.println("Zone 1");
            bZone1 = sendStatus(bZone1,"zone1");
          }
          else if(serNum1 == 251 && bPlace3 == 2 && endPlace6 == 0)
          {
            Serial.println("Zone 2");
            bZone2 = sendStatus(bZone2,"zone2");
          }
          else if(serNum1 == 161 && endPlace3 == 1 && endZone1 == 0)
          {
            Serial.println("End zone1");
            endZone1 = 1;
            endZone1 = sendStatus(endZone1,"endzone1");
          }
          else if(serNum1 == 251 && endPlace6 == 1 && endZone2 == 0)
          {
            Serial.println("End zone2");
            endZone2 = 1;
            endZone2 = sendStatus(endZone2,"endzone2"); 
          }
          else
          {
            Serial.println("Zone 1 first!!");
            digitalWrite(led2, HIGH);
            delay(300);
            digitalWrite(led2, LOW);
          }
        }
        
        //start place in zone 1      
        else if(type == 2 && bZone1 == 2 && endZone1 == 0)
        {
          if(serNum1 == 147)
          {
            Serial.println("Place 1");
            bPlace1 = sendStatus(bPlace1,"place1");
          }
          else if(serNum1 == 71 && bPlace1 == 2)
          {
            Serial.println("Place 2");
            bPlace2 = sendStatus(bPlace2,"place2");
          }
          else if(serNum1 == 254 && bPlace2 == 2)
          {
            Serial.println("Place 3");
            bZone1 = 0;
            endPlace3 = 1;
            bPlace3 = sendStatus(bPlace3,"place3");
          }
          else
          {
            Serial.println("[Zone1]Pleae tag place in order!!");
            digitalWrite(led2, HIGH);
            delay(300);
            digitalWrite(led2, LOW);
          }
        }
                
        //start place in zone2
        else if(type == 2 && bZone2 == 2 && endZone1 == 2)
        {
          if(serNum1 == 153)
          {
            Serial.println("Place 4");
            bPlace4 = sendStatus(bPlace4,"place4");
          }
          else if(serNum1 == 246 && bPlace4 == 2)
          {
            Serial.println("Place 5");
            bPlace5 = sendStatus(bPlace5,"place5");
          }
          else if(serNum1 == 134 && bPlace5 == 2)
          {
            Serial.println("Place 6");
            endPlace6 = 1;
            bPlace6 = sendStatus(bPlace6,"place6");
          }
          else
          {
            Serial.println("[Zone2]Please tag place in order!!");
            digitalWrite(led2, HIGH);
            delay(300);
            digitalWrite(led2, LOW);
          }
        }
        
        //event card 
        else if(type == 3)
        {
          if((bPlace1 == 2 || bPlace2 == 2 || bPlace3 == 2) && endZone1 == 0)
          {
            if(serNum1 == 158)
            {
              Serial.println("[Zone1]Event 1");
              bEvent1 = sendStatus(bEvent1,"event1");
            }
            else if(serNum1 == 164)
            {
              Serial.println("[Zone1]Event 2");
              bEvent2 = sendStatus(bEvent2,"event2");
            }
          }
          else if((bPlace4 == 2 || bPlace5 == 2 || bPlace6 == 2) && bZone2 == 2)
          {
            {
              if(serNum1 == 158)
              {
                Serial.println("[Zone2]Event 1");
                bEvent3 = sendStatus(bEvent3,"event3");
              }
              else if(serNum1 == 164)
              {
                Serial.println("[Zone2]Event 2");
                bEvent4 = sendStatus(bEvent4,"event4");
              }
            }
          }
          else
          {
            Serial.print("Please tag zone/place first!!\n");
            digitalWrite(led2, HIGH);
            delay(300);
            digitalWrite(led2, LOW);
          }
        }
          
        //not zone card first  
        else
        {
          digitalWrite(led2, HIGH);
          delay(300);
          digitalWrite(led2, LOW);
          Serial.print("Please tag zone first!!\n");
        }
      }
      
      /* If we have the same ID, just write a dot. */
      else 
      {
        Serial.print(".");
      }
  }
  rfid.halt();
}

//function to check card type
int cardType(int serNum)
{
  if(serNum == 161 || serNum == 251) //zone
    return 1;
  else if(serNum == 147 || serNum == 71 || serNum == 254 || 
          serNum == 153 || serNum == 246 || serNum == 134) // place
    return 2;
  else if(serNum == 158 || serNum == 164)//event
    return 3;
}

//send status to server
int sendStatus(int value, char where[])
{
    while(value == 1)
    {
      radio.stopListening();
      Serial.print("Sent ");
      Serial.print(where);
      Serial.print(" to server");
      radio.write((void*)where, 32);
      radio.startListening();
      digitalWrite(led1, HIGH);
      delay(300);
      digitalWrite(led1, LOW); 
      return 2;
    }
}
