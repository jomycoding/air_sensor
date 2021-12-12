/*
 * 아래의 경우를 제외하고 미세먼지 수치(PM2.5)만 큰 글씨로 화면에 출력되고,
 * 이 때 상단에 온도,습도 작은글씨로 표기됨.
 * 
 * 습도가 낮은 경우 100% 이하에서 수치가 oled에 다음화면에 출력되며, 습도가 36%보다 작은 경우에는 경고문구가 나옴.
 * 온도가 높은 경우 -100도 이상에서 수치가 oled에 다음화면에 출력되며, 온도가 33도보다 큰 경우에는 경고문구가 나옴.
 * DHT 온도보정  : -1.0도
*/


#define REF_HU 42.0 // for mom 42.0% 이하만 출력(case1), (case2)모두출력 100.0,  (case3) 계절따라 50.0% 이하만
#define REF_HU_wet 80.0 // for 여름 80% 이상일 때 출력
#define REF_HU_WARN 36.0 //36%
#define REF_TE 28.0 // for mom 28.0도 이상에서 출력(case1), (case2)모두출력 -100.0, (case3) 계절따라 28.0도 이상만
#define REF_TE_WARN 33.0 //33.0도

#define Delta_HU -3.0 // 습도보정 defalut -3.0
#define Delta_TE -1.5 // 온도보정 defalut -1.5  




#include <DHT.h>

#define DHTPIN D5     // Digital pin connected to the DHT sensor
//#define dht_GND D4     // Digital pin connected to the DHT sensor GND
//#define dht_POWER D6     // Digital pin connected to the DHT sensor POWER



// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

float dht_te_modi, dht_hu_modi;
DHT dht(DHTPIN, DHTTYPE);



void dht_init(){

//  pinMode(dht_GND,OUTPUT);
//  digitalWrite(dht_GND,LOW);
  
//  pinMode(dht_POWER,OUTPUT);
//  digitalWrite(dht_POWER,HIGH);

  // Wait a few seconds between measurements.
  delay(2000);
}

void dht_loop() {
  // Wait a few seconds between measurements.
  //delay(2000);

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.println("***********************");
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));
  Serial.println("***********************");

    String dst1 = digits3(t);
    const char* cdst1=(const char*) dst1.c_str();
    String dst2 = digits3(h);
    const char* cdst2=(const char*) dst2.c_str();

    //temperature modification 
    dht_te_modi = t + Delta_TE;
    dht_hu_modi = h + Delta_HU;
    
    
    char cdst3[10]; 
    dtostrf(dht_te_modi , 3, 1, cdst3);  // 5 : width, 2 : precision
    char cdst4[10]; 
    dtostrf(dht_hu_modi , 3, 0, cdst4);  // 5 : width, 2 : precision


    if(dht_hu_modi <= REF_HU_WARN){   
     for(int k=1; k<6; k++){   
      u8g2.firstPage(); 
      do{ 
        //void U8G2::drawBitmap(u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t cnt, u8g2_uint_t h, const uint8_t *bitmap) 
        // cnt=width/8
        u8g2.drawBitmap(12,2,13,64,dry_104x64); 
        //u8g2.setFont(u8g2_font_fub20_tr);
        //u8g2.drawStr(20,30,"Very");
        //u8g2.drawStr(40,60,"Dry~!");  
      }while(u8g2.nextPage());
      delay(2000);
      
      u8g2.firstPage(); 
      do{  
        u8g2.drawBitmap(17,2,13,64,dry_104x64);
        //u8g2.setFont(u8g2_font_fub20_tr);
        //u8g2.drawStr(25,30,"Very");
        //u8g2.drawStr(50,60,"Dry~!");  
      }while(u8g2.nextPage());
      delay(2000);
     } 
      
    } //end of if(very dry)

    if((dht_hu_modi<= REF_HU) || (dht_hu_modi>= REF_HU_wet) ){   
     u8g2.firstPage(); 
     do{ 
      u8g2.setFont(u8g2_font_fur11_tr);
      u8g2.drawStr(70,12,cdst3); // top
      u8g2.drawStr(102,12,"`C");

      u8g2.setFont(u8g2_font_fub42_tn);
      u8g2.drawStr(17,62,cdst4); // body - humidity
      
      u8g2.setFont(u8g2_font_fub20_tr);
      u8g2.drawStr(102,62,"%");
       
     }while(u8g2.nextPage()); 
     delay(2000);
     
     if(dht_te_modi >= REF_TE) delay(13000); 
     //  temperature change, check below "if-sentence"
    }
   
        
  
   if(dht_te_modi >= REF_TE_WARN){      
     for(int k=1; k<6; k++){   
      u8g2.firstPage(); 
      do{ 
        //void U8G2::drawBitmap(u8g2_uint_t x, u8g2_uint_t y, u8g2_uint_t cnt, u8g2_uint_t h, const uint8_t *bitmap) 
        // cnt=width/8
        u8g2.drawBitmap(12,2,13,64,hot_104x64); 
        //u8g2.setFont(u8g2_font_fub20_tr);
        //u8g2.drawStr(20,30,"Very");
        //u8g2.drawStr(40,60,"Hot~!");  
      }while(u8g2.nextPage());
      delay(2000);
      
      u8g2.firstPage(); 
      do{  
        u8g2.drawBitmap(17,2,13,64,hot_104x64); 
        //u8g2.setFont(u8g2_font_fub20_tr);
        //u8g2.drawStr(25,30,"Very");
        //u8g2.drawStr(50,60,"Hot~!");  
      }while(u8g2.nextPage());
      delay(2000);
     }   
   }  //end of if("very hot")

  if(dht_te_modi >= REF_TE){ 
     u8g2.firstPage(); 
     do{ 
      u8g2.setFont(u8g2_font_fur11_tr);
      u8g2.drawStr(8,12,cdst4); //  top
      u8g2.drawStr(35,12,"%");

      u8g2.setFont(u8g2_font_fub35_tn);
      u8g2.drawStr(6,60,cdst3); //  body - temperature
      
      //u8g2.drawBitmap(50,40,3,40,percent_24x40);  
      u8g2.setFont(u8g2_font_fur11_tr);
      u8g2.drawStr(110,40,"`C");
       
     }while(u8g2.nextPage()); 
     delay(2000);
     //delay(13000); delay time of PMS7003
  }

  
}
