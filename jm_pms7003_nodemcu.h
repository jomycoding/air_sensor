#include <Arduino.h>
#include <pms.h>

#define REF_PM25_WARN 50 //jm
#define NUM_SAMPLE 15 //jm

////////////////////////////////////////

//
// Please uncomment #define PMS_DYNAMIC in pmsConfig.h file
//

#if defined PMS_DYNAMIC
Pmsx003 *pms_ = nullptr;
#define pms (*pms_)
#else
Pmsx003 pms(D7, D8); 
#endif

////////////////////////////////////////

void pms7003_nodemcu_init(void) {
  Serial.begin(115200);
  while (!Serial) {};
  Serial.println("Pmsx003");

#if defined PMS_DYNAMIC
  pms_ = new Pmsx003(D7, D8);
#else
  pms.begin();
#endif 

  pms.waitForData(Pmsx003::wakeupTime);
  pms.write(Pmsx003::cmdModeActive);
}

////////////////////////////////////////

auto lastRead = millis();

void oled_print(int pm2_5_mean){
   if(pm2_5_mean > REF_PM25_WARN){
   for(int k=1; k<6; k++){
    u8g2.firstPage(); 
    do{ 
        u8g2.drawBitmap(12,2,13,64,dust_104x64); 
        //u8g2.setFont(u8g2_font_fub20_tr);
        //u8g2.drawStr(20,30,"Bad");
        //u8g2.drawStr(40,60,"Air!");
    }while(u8g2.nextPage());     
    delay(2000);
    
    u8g2.firstPage(); 
    do{ 
        u8g2.drawBitmap(17,2,13,64,dust_104x64); 
        //u8g2.setFont(u8g2_font_fub20_tr);
        //u8g2.drawStr(25,30,"Bad");
        //u8g2.drawStr(60,60,"Air!");
    }while(u8g2.nextPage());     
    delay(2000);    
   }
  }
  
  u8g2.firstPage();
  do{
        // temperature & humidity from DHT----------------------------
        char cdst11[10]; 
        dtostrf(dht_te_modi , 3, 1, cdst11);  // 5 : width, 2 : precision
        char cdst12[10]; 
        dtostrf(dht_hu_modi , 3, 0, cdst12);  // 5 : width, 2 : precision

        u8g2.setFont(u8g2_font_fur11_tr);
        u8g2.drawStr(70,12,cdst11); 
        //u8g2.setPrintPos(70,12);
        //u8g2.print(dht_te_modi);        
        u8g2.drawStr(102,12,"`C");
                
        u8g2.drawStr(8,12,cdst12); 
        //u8g2.setPrintPos(8,12);
        //u8g2.print(dht_hu_modi);
        u8g2.drawStr(35,12,"%");
        // end of DHT ---------------------------------------------
  
        
        String dst2 = digits3(pm2_5_mean);
        const char* cdst2=(const char*) dst2.c_str();
        u8g2.setFont(u8g2_font_fur11_tr);
        u8g2.drawStr(112,57,"ug");      
        u8g2.setFont(u8g2_font_fub42_tn);
        u8g2.drawStr(11,62,cdst2); //  body    
             
   }while (u8g2.nextPage());
   delay(15000); 
}


int pms7003_nodemcu_loop(void) {
  
  const Pmsx003::pmsIdx n = Pmsx003::nValues_PmsDataNames;
  Pmsx003::pmsData data[n];

  auto t0Read = millis();
  Pmsx003::PmsStatus status = pms.read(data, n);
  auto t1Read = millis();

  switch (status) {
    case Pmsx003::OK:
    {
      Serial.print("_________________ time of read(): ");
      Serial.print(t1Read - t0Read);
      Serial.println(" msec");
      auto newRead = millis();
      Serial.print("Wait time ");
      Serial.println(newRead - lastRead);
      lastRead = newRead;

      for (Pmsx003::pmsIdx i = 0; i < n; ++i) {

        //Serial.println("_________________");
        //Serial.print(status);
        //Serial.print(" status : ");
        //Serial.println(Pmsx003::errorMsg[status]);
        
        Serial.print(i);
        Serial.print(": ");
        Serial.print(data[i]);
        Serial.print("\t");
        Serial.print(Pmsx003::getDataNames(i));
        Serial.print(" [");
        Serial.print(Pmsx003::getMetrics(i));
        Serial.print("]");
        Serial.println();
        
      }
      
      return data[1]; //jm    
      break;
    }
    case Pmsx003::noData:
      return 0;
      break;
      
    default:
      Serial.println("_________________");
      Serial.print(status);
      Serial.print(" status : ");
      Serial.println(Pmsx003::errorMsg[status]);

      return 0;
  }

}

void pms7003_nodemcu_PM25(void) {
  
  int PM25_now=0.0;
  int PM25_sum=0.0;
  int PM25_mean=0.0;  
  int n_eff=0;
  
  for(int i=0; i<NUM_SAMPLE; i++){ 
    
    PM25_now = pms7003_nodemcu_loop(); 
    Serial.println("***************");
    Serial.print(i);
    Serial.print("(i): PM25_now =");
    Serial.println(PM25_now);  
    
    PM25_sum= PM25_sum + PM25_now;
    if(PM25_now == 0) i=i-1 ;
    n_eff = i+1;
    delay(1000);
  }
  
  PM25_mean = PM25_sum/n_eff;

  
  Serial.print(n_eff);
  Serial.print("(neff): PM25_sum =");
  Serial.print(PM25_sum); 
  Serial.print(": PM25_mean =");
  Serial.println(PM25_mean); 

  oled_print(PM25_mean); //jm

}
