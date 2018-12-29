///////////////////////////////////////////////////////////////////////////
//Meteostanice postavená na shieldech ESP8266
//
//Vytvořil Jakub Andrýsek
//Začátek tvorby 28.12.2018
//https://github.com/JakubAndrysek/Meteostanice_ESP8266
///////////////////////////////////////////////////////////////////////////

// připojení knihoven
#include "Void.cpp"
#include <Arduino.h>
#include <SPI.h> //Seriova linka
#include <Wire.h>
#include <SD.h> //SD karta
#include <Adafruit_BMP085.h> //Teplomer
#include <Adafruit_GFX.h> //Fonty textu
#include <RTClib.h> //RTC-hodiny
//OLED
#include <U8g2lib.h>
U8G2_SSD1306_64X48_ER_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);



RTC_DS1307 DS1307; // instance RTC
File myFile; //instance SD karty
Adafruit_BMP085 bmp; //instance teplomeru

//Promene/////////////
const int  BTleft_pin = D3;    
const int  BTright_pin = D4;

float vyska_raw = 555;
int vyska = 555;
int vyska_max = -11000;
int vyska_min  = 11000;
int vyska_last = 0;
int vyska_up_all = 0;
int vyska_down_all = 0;

float teplota_raw = 55;
int teplota = 55;
int teplota_max = -55;
int teplota_min  = 100;
int teplota_last = 0;

float tlak_raw = 100000;
int tlak = 1000;
int tlak_max = 100;
int tlak_min  = 10000;
int tlak_last = 0;

int BTleft_counter = 0;   // counter for the number of button presses
int BTleft_state = 0;         // current state of the button
int BTleft_last_state = 0;     // previous state of the button

int BTright_counter = 0;   // counter for the number of button presses
int BTright_state = 0;         // current state of the button
int BTright_last_state = 0;     // previous state of the button



bool DB = false;

//Enumy////////////////
enum tZobrazeni
{
  HOME,
  VYSKA,
  TEPLOTA,
  TLAK,
  CAS
};

tZobrazeni zobrazeni = HOME;


enum tStav
{
  VZHURU,
  SPIM
};

tStav stav = VZHURU;

void setup () 
{
  //SERIAL/////////////
  Serial.begin(9600);
  Serial.println("Serial-Run");

  //Button///////////
  pinMode(BTleft_pin, INPUT);
  pinMode(BTright_pin, INPUT);

  
  //RTC///////////////
  // kontrola připojeného obvodu reálného času
  if (! DS1307.begin())
  {
    Serial.println("Hodiny nejsou pripojeny!");
  }
  // kontrolu spuštění obvodu reálného času
  if (! DS1307.isrunning())
  {
    Serial.println("Hodiny nejsou spusteny! Spoustim nyni..");
  }
  // pro nastavení času v obvodu reálného času použijte jednorázově(!)
  // následující příkaz v pořadí rok, měsíc, den, hodina, minuta, vteřina
  // příklad: 28.12.2018 15:15:50
  //DS1307.adjust(DateTime(2018, 12, 28, 15, 41, 50));

  
  //BMP085////////////////
  if (!bmp.begin()) 
  {
    Serial.println("Could not find BMP180 or BMP085 sensor at 0x77");
  }
  vyska_last=bmp.readAltitude();


  //OLED/////////////////////
  u8g2.begin();
  /*
  u8g2.clearBuffer(); //Smaze displej
  u8g2.drawRFrame(0,0,64,48,5);
  u8g2.setFont(u8g2_font_profont12_mf );	// Nastavi 
  u8g2.drawStr(10,23,"OD KUBY"); //Vypise na displej
  u8g2.setFont(u8g2_font_profont10_mf );	// Nastavi 
  u8g2.drawStr(22,36,"V1.0");  //Vypise na displej
  u8g2.sendBuffer();  //Zobrazi
  */
  
  delay(2000);
}
 



void loop ()
{
   ///////////////////////////
  //Kontrola VZHURU / SPIM///
  ///////////////////////////
  
  
  switch (stav)
  {
    case VZHURU:
      wake();
    case SPIM:
      sleep();
  }















 
  
  
  
  
 
  
  
  
  switch(zobrazeni)
  {
    case HOME:
      //Vypis casu
      u8g2.setCursor(0, 11); 
      u8g2.print(datumCas.hour());
      u8g2.print(':');
      u8g2.print(datumCas.minute());
      u8g2.print(':');
      u8g2.print(datumCas.second());
    
      //Vypis teploty a vysky
      u8g2.setCursor(0, 21);  // (x,y)
      u8g2.print("T:"); 
      u8g2.print(teplota);
    
      u8g2.print(" V:"); 
      u8g2.print(vyska);
      u8g2.sendBuffer(); //Zobrazi displej

      break;


     case VYSKA:
      u8g2.setCursor(0, 11);  // (x,y) 
      u8g2.print("Vys:"); 
      u8g2.print(vyska);
      
      u8g2.sendBuffer(); //Zobrazi displej
      
      break;

     case TEPLOTA:
      u8g2.setFont(u8g2_font_profont10_mf );	// Nastavi 
      u8g2.drawStr(14,10,"Teplota");  //Vypise na displej  

      u8g2.setCursor(0, 20);  // (x,y) 
      u8g2.print("Tepl:"); 
      u8g2.print(teplota);
      
      u8g2.sendBuffer(); //Zobrazi displej
      
      break;

     case TLAK:
      u8g2.setCursor(0, 11);  // (x,y) 
      u8g2.print("Tl:"); 
      u8g2.print(tlak);
      
      u8g2.sendBuffer(); //Zobrazi displej
      
      break;

    case CAS:
      u8g2.setCursor(0, 11);  // (x,y)
       
      u8g2.print(datumCas.hour());
      u8g2.print(':');
      u8g2.print(datumCas.minute());
      u8g2.print(':');
      u8g2.print(datumCas.second());
    
      u8g2.setCursor(0, 21);  // (x,y)
      u8g2.print(datumCas.day());
      u8g2.print('.');
      u8g2.print(datumCas.month());
      u8g2.print('.');
      u8g2.print(datumCas.year());
      u8g2.sendBuffer(); //Zobrazi displej      
      
      break;
      
  }







*/
  //delay(3000);
}
