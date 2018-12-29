///////////////////////////////////////////////////////////////////////////
//Meteostanice postavená na shieldech ESP8266
//
//Vytvořil Jakub Andrýsek
//Začátek tvorby 28.12.2018
//https://github.com/JakubAndrysek/Meteostanice_ESP8266
///////////////////////////////////////////////////////////////////////////

// připojení knihoven
#include <Arduino.h>
#include <SPI.h> //Seriova linka
#include <Wire.h>
#include <SD.h> //SD karta
#include <Adafruit_BMP085.h> //Teplomer
#include <Adafruit_GFX.h> //Fonty textu
#include <RTClib.h> //RTC-hodiny
//OLED
#include <Adafruit_SSD1306.h> 
#define OLED_RESET 0  // GPIO0
Adafruit_SSD1306 display(OLED_RESET);


RTC_DS1307 DS1307; // instance RTC
File myFile; //instance SD karty
Adafruit_BMP085 bmp; //instance teplomeru

//Promene/////////////
const int BTright = D3;
const int BTleft = D4;


int vyska = 555;
int vyska_max = -11000;
int vyska_min  = 11000;
int vyska_last = 0;
int vyska_up_all = 0;
int vyska_down_all = 0;

int teplota = 55;
int teplota_max = -55;
int teplota_min  = 100;
int teplota_last = 0;

int tlak = 1000;
int tlak_max = 100;
int tlak_min  = 10000;
int tlak_last = 0;



bool DB = false;

//Enumy////////////////
enum tZobrazeni
{
  HOME,
  VYSKA,
  TEPLOTA,
  TLAK,
  CAS
}zobrazeni=HOME;

void setup () 
{
  //SERIAL/////////////
  Serial.begin(9600);
  Serial.println("Serial-Run");

  //Button///////////
  pinMode(BTright, INPUT);
  pinMode(BTleft, INPUT);

  
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


  //OLED//////////////////
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 64x48)
    
  display.clearDisplay(); //Smaze displej
  display.setTextSize(1); //Nastavi velikost pisma
  display.setTextColor(WHITE);
  display.setCursor(32, 8);  // (x,y)
  
  display.print("OLED-Run"); 
  display.display();
  
  delay(500);
}

void loop ()
{
  display.clearDisplay(); //vycisteni displeje
  display.setTextSize(1); //Nastaveni fontu
  display.setTextColor(WHITE); //Barva textu
  
  DateTime datumCas = DS1307.now(); //Nacteni casu

  vyska=bmp.readAltitude();
  teplota=bmp.readTemperature();
  tlak=(bmp.readPressure()/100);
  
  //Vyska//////////////////
  if(DB)
  {
    Serial.print("Vyska ");
    Serial.println(vyska);
  }
 
  if (vyska>vyska_max)
  {
    vyska_max=vyska;
    if(DB)
    {  
      Serial.print("V-Max ");
      Serial.println(vyska_max);
    }
  }
  else if (vyska<vyska_min)
  {
    vyska_min=vyska;
    if(DB)
    {  
      Serial.print("V-Min ");
      Serial.println(vyska_min);
    }
  }


  //Teplota/////////////////
  if(DB)
  {
    Serial.print("Teplota ");
    Serial.println(teplota);  
  }
  if (vyska>teplota_max)
  {
    teplota_max=teplota;
    if(DB)
    {
      Serial.print("T-Max ");
      Serial.println(teplota_max);
    }
  }
  else if (teplota<teplota_min)
  {
    teplota_min=teplota;
    if(DB)
    {
      Serial.print("T-Min ");
      Serial.println(teplota_min);
    }  
  }
  
  
  //Tlak//////////////////
  if(DB)
  {
    Serial.print("Tlak ");
    Serial.println(tlak);  
  }
  if (tlak>tlak_max)
  {
    tlak_max=tlak;
    if(DB)
    {
      Serial.print("P-Max ");
      Serial.println(tlak_max);
    }
  }
  else if (tlak<tlak_min)
  {
    tlak_min=tlak;
    if(DB)
    {
      Serial.print("P-Min ");
      Serial.println(tlak_min);
    }
  }

  //Nachozene metry nahory
  if (vyska>vyska_last)
  {
    vyska_up_all+=(vyska-vyska_last);
    vyska_last=vyska;
    
  }
  if(DB)
  {
    Serial.println(vyska_up_all);
  }
  
  //Nachozene metry dolu
  if (vyska<vyska_last)
  {
    vyska_down_all+=(vyska_last-vyska);
    vyska_last=vyska;
    
  }
  if(DB)
  {
    Serial.println(vyska_down_all);
  }

  
  Serial.println(zobrazeni);
  if (digitalRead(BTleft)==HIGH)
  {
    Serial.println("left");
    switch(zobrazeni)
    {
      case HOME:
        zobrazeni=CAS;
        break;
        
      case VYSKA:
        zobrazeni=HOME;
        break;

      case TEPLOTA:
        zobrazeni=VYSKA;
        break;
        
      case TLAK:
        zobrazeni=TEPLOTA;
        break;
        
      case CAS:
        zobrazeni=TLAK;
        break;
    }
  }
  else if (digitalRead(BTright)==HIGH)
  {
    Serial.println("right");   
    switch(zobrazeni)
    {
      case HOME:
        zobrazeni=VYSKA;
        break;

      case VYSKA:
        zobrazeni=TEPLOTA;
        break;

      case TEPLOTA:
        zobrazeni=TLAK;
        break;
        
      case TLAK:
        zobrazeni=CAS;
        break;
        
      case CAS:
        zobrazeni=HOME;
        break;
    }
  }
  
  
  
  switch(zobrazeni)
  {
    case HOME:
      //Vypis casu
      display.setCursor(32, 8);  // (x,y)  
      display.print(datumCas.hour());
      display.print(':');
      display.print(datumCas.minute());
      display.print(':');
      display.print(datumCas.second());
    
      //Vypis teploty a vysky
      display.setCursor(32, 16);  // (x,y)
      display.print("T:"); 
      display.print(teplota);
    
      display.print(" V:"); 
      display.print(vyska);
      display.display();

      break;


     case VYSKA:
      display.setCursor(32, 8);  // (x,y) 
      display.print("Vys:"); 
      display.print(vyska);
      
      display.display();
      
      break;

     case TEPLOTA:
      display.setCursor(32, 8);  // (x,y) 
      display.print("Tepl:"); 
      display.print(teplota);
      
      display.display();
      
      break;

     case TLAK:
      display.setCursor(32, 8);  // (x,y) 
      display.print("Tl:"); 
      display.print(tlak);
      
      display.display();
      
      break;

    case CAS:
      display.setCursor(32, 8);  // (x,y)
       
      display.print(datumCas.hour());
      display.print(':');
      display.print(datumCas.minute());
      display.print(':');
      display.print(datumCas.second());
    
      display.setCursor(32, 17);  // (x,y)
      display.print(datumCas.day());
      display.print('.');
      display.print(datumCas.month());
      display.print('.');
      display.print(datumCas.year());
      display.display();      
      
      break;
      
  }








  delay(3000);
}