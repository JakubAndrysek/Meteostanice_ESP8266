//#include "main.cpp"


void wake ()
{
    u8g2.clearBuffer();	//Smaze displej
    DateTime datumCas = DS1307.now(); //Nacteni casu
    //Nastaveni promenych hodnotami z teplomeru
    vyska_raw=bmp.readAltitude();
    teplota_raw=bmp.readTemperature();
    tlak_raw=bmp.readPressure();

    //Rastr////////////////
    u8g2.drawRFrame(0,0,64,48,5);
    u8g2.drawRFrame(12,0,40,14,3);

    //Kalibrace///////////
    vyska = vyska_raw + 120;
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






}


sleep();
{
    
}





