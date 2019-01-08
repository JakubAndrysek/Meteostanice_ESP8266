/*
#include <Arduino.h>
#include <SD.h>
 
 
// CS pro četčku karet na pinu 4
// (pro Arduino UNO a Ethernet Shield
const int chipSelect = 4;
char vety[200] = "";
int pozice = 0;
char pole[200][200];
 
void setup()
{    
  // nastaví seriovou komunakaci na 9600
  Serial.begin(9600);
 
  Serial.print("Inicializace SD karty...");
  pinMode(4, OUTPUT);
 
  //zkontroluje inicializaci karty
  if (!SD.begin(chipSelect)) {
    Serial.println("\nKarta nenalezena...");    
    return;
  }
 
  Serial.println("\nKarta inicializovana...\n");
 
   // otevře soubor data.txt
   File dataFile = SD.open("Radky.txt", FILE_READ);
 
  // když soubor existuje
  if (dataFile)
  {
     // inicializuje proměnou pro číslo řádku
     int radek = 1;
     int cisla[200];
     
     while (dataFile.available()) // dokud nebudeme na konci souboru tak:
     {
        byte data = dataFile.read(); // přečte jeden bajt (znak) a uloží do proměné data
        
        if (data == 13) // když jsou data znak #13 (Enter, nový řádek)
        {
          radek++;
        }
        vety[pozice] = data;
        pozice ++ ;
     }



     for (int i=0;i<pozice;i++)
     {
       Serial.write(vety[i]);
     }

     for (int i=0;i<pozice;i++)
     {
       if (vety[i]>47 and vety[i]< 58)
       {
         cisla[i]=vety[i];
         
       }
     }





     Serial.println("\nKonec");
     // soubor opět zavřeme
     
     dataFile.close();  
  }
}
 
void loop()
{

}



int recNum = 0; // We have read 0 records so far

 //open a file
 OpenFile("Sample.txt");

 while (myFile.available())
 {
    String list = myFile.readStringUntil('\r');
    Serial.println(list);
    recNum++; // Count the record

    if(recNum == recordNumberOfInterest)
    {
       // Hey, we found the one that we want to do something with
    }
 }
*/



#include <Arduino.h>
#include <SD.h>
 
 
// CS pro četčku karet na pinu 4
// (pro Arduino UNO a Ethernet Shield
const int chipSelect = 4;
int recNum = 0; //Zatím jsme četli 0 záznamů
String pole [20];
String cislo [20];
int radek = 0;

void setup()
{    
  // nastaví seriovou komunakaci na 9600
  Serial.begin(9600);
 
  Serial.print("Inicializace SD karty...");
  pinMode(4, OUTPUT);
 
  //zkontroluje inicializaci karty
  if (!SD.begin(chipSelect)) {
    Serial.println("\nKarta nenalezena...");    
    return;
  }
 
  Serial.println("\nKarta inicializovana...\n");
 
   // otevře soubor data.txt
   File dataFile = SD.open("Text.txt", FILE_READ);
 
  // když soubor existuje
  if (dataFile)
  {
     // inicializuje proměnou pro číslo řádku
     
     
     
    while (dataFile.available())
    {
      pole[radek] = dataFile.readStringUntil('\r');
      
      //Serial.println(pole[radek]);


      if(radek == 3)
      {
        Serial.print(pole[radek]);
        Serial.println("Knec textu");
      }

      radek++;
     }

     Serial.println("\nKonec nacitani");
     // soubor opět zavřeme
     
     dataFile.close(); 
     



     for (int i=0;i<(radek-1);i++)
     {
       cislo[i] = pole.readStringUntil(',');
       Serial.println(cislo[i]);
     }
    Serial.println("\nKonec vypisovani");
  }
}
 
void loop()
{

}