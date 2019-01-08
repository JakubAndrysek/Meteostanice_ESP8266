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


bool wake = true;
bool DB = false;

//Enumy////////////////
enum tZobrazeni
{
  HOME,
  VYSKA,
  USLO,
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
  vyska_last=(bmp.readAltitude()+120);


  //OLED/////////////////////
  u8g2.begin();
  
  u8g2.clearBuffer(); //Smaze displej
  u8g2.drawRFrame(0,0,64,48,5);
  u8g2.setFont(u8g2_font_profont12_mf );	// Nastavi fontu
  u8g2.drawStr(10,23,"OD KUBY"); //Vypise na displej
  u8g2.setFont(u8g2_font_profont10_mf );	// Nastavi fontu
  u8g2.drawStr(22,36,"V1.1");  //Vypise na displej
  u8g2.sendBuffer();  //Zobrazi
  
  
  delay(100);
}

void loop()
{

  ///////////////////////////
  //Kontrola VZHURU / SPIM///
  ///////////////////////////

  if (wake==true)
  {
    u8g2.clearBuffer();	//Smaze displej
    u8g2.setFont(u8g2_font_profont10_mf );	// Nastavi fontu
    DateTime datumCas = DS1307.now(); //Nacteni casu
    //Nastaveni promenych hodnotami z teplomeru
    vyska_raw=bmp.readAltitude();
    teplota_raw=bmp.readTemperature();
    tlak_raw=bmp.readPressure();

    //Rastr////////////////
    u8g2.drawRFrame(0,0,64,48,5);
    u8g2.drawRFrame(12,0,40,12,3);

    //Kalibrace///////////
    vyska = vyska_raw + 120; //Nutno zmenit i nahore
    teplota = teplota_raw - 4;
    tlak = (tlak_raw/100) + 50;


    //Vyska//////////////////
    if(!DB)
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
    if (vyska<vyska_min)
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
    if (teplota>teplota_max)
    {
        teplota_max=teplota;
        if(DB)
        {
        Serial.print("T-Max ");
        Serial.println(teplota_max);
        }
    }
    if (teplota<teplota_min)
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
    if (tlak<tlak_min)
    {
        tlak_min=tlak;
        if(DB)
        {
        Serial.print("P-Min ");
        Serial.println(tlak_min);
        }   
    }


    //Nachozene metry nahory
    if ((vyska+2)>vyska_last)
    {
        vyska_up_all+=(vyska-vyska_last);
        vyska_last=vyska;
        
    }
    if(DB)
    {
        Serial.print("Nahoru");
        Serial.println(vyska_up_all);
    }


    //Nachozene metry dolu
    if ((vyska-2)<vyska_last)
    {
        vyska_down_all+=(vyska_last-vyska);
        vyska_last=vyska;
        
    }
    if(DB)
    {
        Serial.print("Dolu");
        Serial.println(vyska_down_all);
    }



    //Leve tlacitko zmacknuto////////////////
    BTleft_state = digitalRead(BTleft_pin);
    if (BTleft_state != BTleft_last_state) 
    {
        if (BTleft_state == HIGH) 
        {
        switch(zobrazeni)
        {
            case HOME:
            zobrazeni=CAS;
            break;
            
            case VYSKA:
            zobrazeni=HOME;
            break;

            case USLO:
            zobrazeni=VYSKA;
            break;

            case TEPLOTA:
            zobrazeni=USLO;
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
        switch(zobrazeni)
        {
            case HOME:
            zobrazeni=VYSKA;
            break;
            
            case VYSKA:
            zobrazeni=USLO;
            break;

            case USLO:
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
        u8g2.drawStr(21,9,"HOME");  //Vypise na displej 

        u8g2.setCursor(3, 20); 
        u8g2.print(datumCas.hour());
        u8g2.print(':');
        u8g2.print(datumCas.minute());
        u8g2.print(':');
        u8g2.print(datumCas.second());
    
        //Vypis teploty a vysky
        u8g2.setCursor(3, 28);  // (x,y)
        u8g2.print("T:"); 
        u8g2.print(teplota);
    
        u8g2.print(" V:"); 
        u8g2.print(vyska);
        u8g2.sendBuffer(); //Zobrazi displej

        break;


    case VYSKA:
        u8g2.drawStr(19,9,"Vyska");  //Vypise na displej 

        u8g2.setCursor(3, 20);  // (x,y) 
        u8g2.print("Now:"); 
        u8g2.print(vyska);
        u8g2.print(" m");

        u8g2.setCursor(3, 28);  // (x,y) 
        u8g2.print("Min:"); 
        u8g2.print(vyska_min);
        u8g2.print(" m");

        u8g2.setCursor(3, 36);  // (x,y) 
        u8g2.print("Max:"); 
        u8g2.print(vyska_max);
        u8g2.print(" m");
        
        u8g2.sendBuffer(); //Zobrazi displej
        
        break;

    case USLO:
        u8g2.drawStr(21,9,"Uslo");  //Vypise na displej
        
        u8g2.setCursor(3, 20);  // (x,y) 
        u8g2.print("Nahoru:"); 
        u8g2.print(vyska_up_all);
        u8g2.print(" m");

        u8g2.setCursor(3, 28);  // (x,y) 
        u8g2.print("Dolu:"); 
        u8g2.print(vyska_down_all);
        u8g2.print(" m");

        u8g2.sendBuffer(); //Zobrazi displej 

        break;

    case TEPLOTA:
        u8g2.drawStr(14,9,"Teplota");  //Vypise na displej  

        u8g2.setCursor(3, 20);  // (x,y) 
        u8g2.print("Now:"); 
        u8g2.print(teplota);
        u8g2.print(" C");

        u8g2.setCursor(3, 28);  // (x,y) 
        u8g2.print("Min:"); 
        u8g2.print(teplota_min);
        u8g2.print(" C");

        u8g2.setCursor(3, 36);  // (x,y) 
        u8g2.print("Max:"); 
        u8g2.print(teplota_max);
        u8g2.print(" C");


        u8g2.sendBuffer(); //Zobrazi displej
        
        break;

    case TLAK:
        u8g2.drawStr(23,9,"Tlak");  //Vypise na displej 
        u8g2.setCursor(3, 20);  // (x,y) 
        u8g2.print("Now:"); 
        u8g2.print(tlak);
        u8g2.print(" kPa");

        u8g2.setCursor(3, 28);  // (x,y) 
        u8g2.print("Min:"); 
        u8g2.print(tlak_min);
        u8g2.print(" kPa");

        u8g2.setCursor(3, 36);  // (x,y) 
        u8g2.print("Max:"); 
        u8g2.print(tlak_max);
        u8g2.print(" kPa");


        u8g2.sendBuffer(); //Zobrazi displej
        
        break;

    case CAS:
        u8g2.drawStr(25,9,"Cas");  //Vypise na displej 

        u8g2.setCursor(3, 20);  // (x,y)
        
        u8g2.print(datumCas.hour());
        u8g2.print(':');
        u8g2.print(datumCas.minute());
        u8g2.print(':');
        u8g2.print(datumCas.second());
    
        u8g2.setCursor(3, 28);  // (x,y)
        u8g2.print(datumCas.day());
        u8g2.print('.');
        u8g2.print(datumCas.month());
        u8g2.print('.');
        u8g2.print(datumCas.year());
        u8g2.sendBuffer(); //Zobrazi displej      
        
        break;
    }

  }
  else
  {
    Serial.println("Spim");
  }



}

