///////////////////////////////////////////////////////////////////////////
//Meteostanice postavená na shieldech ESP8266
//
//Vytvořil Jakub Andrýsek
//Začátek tvorby 28.12.2018
//https://github.com/JakubAndrysek/Meteostanice_ESP8266
///////////////////////////////////////////////////////////////////////////





//#include <Arduino.h>



// připojení knihoven
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
  
  u8g2.clearBuffer();					// clear the internal memory
  u8g2.setFont(u8g2_font_crox2cb_tf );	// choose a suitable font
  u8g2.drawStr(0,10,"Meteostanice!");	// write something to the internal memory
  u8g2.sendBuffer();					// transfer internal memory to the display

  
  delay(2000);
}



void loop ()
{
  u8g2.clearBuffer();	//Smaze displej
  u8g2.setFont(u8g2_font_ncenB08_tr);	//Nastavi font
    
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

  
  
  
  
  //Leve tlacitko zmacknuto////////////////
  BTleft_state = digitalRead(BTleft_pin);
  if (BTleft_state != BTleft_last_state) 
  {
    if (BTleft_state == HIGH) 
    {
      // Zde se projevi zmena
      BTleft_counter++;
      Serial.print("Left");
      Serial.println(BTleft_counter);
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
  }  
  BTleft_last_state = BTleft_state;



  //Prave tlacitko zmacknuto/////////////////
  BTright_state = digitalRead(BTright_pin);
  if (BTright_state != BTright_last_state) 
  {
    if (BTright_state == HIGH) 
    {
      // Zde se projevi zmena
      BTright_counter++;
      Serial.print("Right");
      Serial.println(BTright_counter);
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
  }  
  BTright_last_state = BTright_state;

  
  
  
  switch(zobrazeni)
  {
    case HOME:
      //Vypis casu
      u8g2.setCursor(0, 12); 
      u8g2.print(datumCas.hour());
      u8g2.print(':');
      u8g2.print(datumCas.minute());
      u8g2.print(':');
      u8g2.print(datumCas.second());
    
      //Vypis teploty a vysky
      u8g2.setCursor(0, 25);  // (x,y)
      u8g2.print("T:"); 
      u8g2.print(teplota);
    
      u8g2.print(" V:"); 
      u8g2.print(vyska);
      u8g2.sendBuffer(); //Zobrazi displej

      break;


     case VYSKA:
      display.setCursor(32, 8);  // (x,y) 
      display.print("Vys:"); 
      display.print(vyska);
      
      u8g2.sendBuffer(); //Zobrazi displej
      
      break;

     case TEPLOTA:
      display.setCursor(32, 8);  // (x,y) 
      display.print("Tepl:"); 
      display.print(teplota);
      
      u8g2.sendBuffer(); //Zobrazi displej
      
      break;

     case TLAK:
      display.setCursor(32, 8);  // (x,y) 
      display.print("Tl:"); 
      display.print(tlak);
      
      u8g2.sendBuffer(); //Zobrazi displej
      
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
      u8g2.sendBuffer(); //Zobrazi displej      
      
      break;
      
  }








  //delay(3000);
}
