/* 
 *  전역변수 사용: dht_te_modi 를 DHT로부터 온도값을 받아 옴.
 *  전역변수 사용: dht_hu_modi 를 DHT로부터 습도값을 받아 옴.
 *  PM2.5가 50보다 큰 경우에는 경고문구가 나옴

*/

#define REF_PM25_WARN 50
#include <SoftwareSerial.h> 


// #define DEBUG    // DEBUG 가 무슨 역할을 하는지 잘 모르겠습니다. 
                        // 아래 코드에 보면 #ifdef 문등으로 분기 되어있는데
                        // #define DEBUG 선언을 안해도 수치가 나오기는 합니다.
                        // 고수님들이 손을 좀 봐주세요


#define MEAN_NUMBER 15 // 평균을 잡기 위해 읽어들이는 데이타 수.. 10 ~ 30 이 적당함
#define MAX_PM 0
#define MIN_PM 32767


SoftwareSerial mySerial(D7, D8);


int incomingByte = 0; // for incoming serial data
const int MAX_FRAME_LEN = 64;
char frameBuf[MAX_FRAME_LEN];
int detectOff = 0;
int frameLen = MAX_FRAME_LEN;
bool inFrame = false;
char printbuf[256];


unsigned int calcChecksum = 0;
unsigned int pm1_0=0, pm2_5=0, pm10_0=0;
unsigned int tmp_max_pm1_0, tmp_max_pm2_5, tmp_max_pm10_0; 
unsigned int tmp_min_pm1_0, tmp_min_pm2_5, tmp_min_pm10_0; 
byte i=0;


struct PMS7003_framestruct {
byte frameHeader[2];
unsigned int frameLen = MAX_FRAME_LEN;
unsigned int concPM1_0_CF1;
unsigned int concPM2_5_CF1;
unsigned int concPM10_0_CF1;
unsigned int checksum;
} thisFrame;


bool pms7003_read() {
  #ifdef DEBUG 
    Serial.println("----- Reading PMS7003 -----");
  #endif
  
  mySerial.begin(9600);
  //mySerial.begin(115200);
  
  bool packetReceived = false;
  calcChecksum = 0;
  
  while (!packetReceived) {
    if (mySerial.available() > 32) {
      int drain = mySerial.available();

      #ifdef DEBUG
        Serial.print("----- Draining buffer: -----");
        Serial.println(mySerial.available(), DEC);
      #endif
   
      for (int i = drain; i > 0; i--) {
        mySerial.read();
      }
    }

    if (mySerial.available() > 0) {
      #ifdef DEBUG
       Serial.print("----- Available: -----");
       Serial.println(mySerial.available(), DEC);
      #endif
    
      incomingByte = mySerial.read();
      #ifdef DEBUG
       Serial.print("----- READ: -----");
       Serial.println(incomingByte, HEX);
      #endif
    
      if (!inFrame) {
        if (incomingByte == 0x42 && detectOff == 0) {
           frameBuf[detectOff] = incomingByte;
           thisFrame.frameHeader[0] = incomingByte;
           calcChecksum = incomingByte; // Checksum init!
           detectOff++;
        }
    
        else if (incomingByte == 0x4D && detectOff == 1) {
          frameBuf[detectOff] = incomingByte;
          thisFrame.frameHeader[1] = incomingByte;
          calcChecksum += incomingByte;
          inFrame = true;
          detectOff++;
        }
  
        else {
          Serial.print("----- Frame syncing... -----");
          Serial.print(incomingByte, HEX);
          Serial.println();
        }
     }
  
    else {
      frameBuf[detectOff] = incomingByte;
      calcChecksum += incomingByte;
      detectOff++;
      unsigned int val = (frameBuf[detectOff-1]&0xff)+(frameBuf[detectOff-2]<<8);
    
      switch (detectOff) {
      case 4:
        thisFrame.frameLen = val;
        frameLen = val + detectOff;
      break;
     case 6:
       thisFrame.concPM1_0_CF1 = val;
      break;
      case 8:
        thisFrame.concPM2_5_CF1 = val;
      break;
      case 10:
        thisFrame.concPM10_0_CF1 = val;
      break;
      case 32:
        thisFrame.checksum = val;
        calcChecksum -= ((val>>8)+(val&0xFF));
      break;
      default:
      break;
    }

    
      if (detectOff >= frameLen) {
        #ifdef DEBUG 
          sprintf(printbuf, "PMS7003 ");
          sprintf(printbuf, "%s[%02x %02x] (%04x) ", printbuf,
          thisFrame.frameHeader[0], thisFrame.frameHeader[1], thisFrame.frameLen);
          sprintf(printbuf, "%sCF1=[%04x %04x %04x] ", printbuf,
          thisFrame.concPM1_0_CF1, thisFrame.concPM2_5_CF1, thisFrame.concPM10_0_CF1);
          sprintf(printbuf, "%scsum=%04x %s xsum=%04x", printbuf,
          thisFrame.checksum, (calcChecksum == thisFrame.checksum ? "==" : "!="), calcChecksum);
          Serial.println(printbuf);
        #endif 
    
        packetReceived = true;
        detectOff = 0;
        inFrame = false;
      }
    }
    } 
  }  //end of while
  
  mySerial.end();
  return (calcChecksum == thisFrame.checksum);
  
}




void pms7003_loop() {

 while(i<MEAN_NUMBER){  
 
  if(i==0) { 
    tmp_max_pm1_0 = MAX_PM;
    tmp_max_pm2_5 = MAX_PM;
    tmp_max_pm10_0 = MAX_PM;
    tmp_min_pm1_0 = MIN_PM;
    tmp_min_pm2_5 = MIN_PM;
    tmp_min_pm10_0 = MIN_PM;
  }

  if (pms7003_read()) {
    tmp_max_pm1_0 = max(thisFrame.concPM1_0_CF1, tmp_max_pm1_0);
    tmp_max_pm2_5 = max(thisFrame.concPM2_5_CF1, tmp_max_pm2_5);
    tmp_max_pm10_0 = max(thisFrame.concPM10_0_CF1, tmp_max_pm10_0);
    tmp_min_pm1_0 = min(thisFrame.concPM1_0_CF1, tmp_min_pm1_0);
    tmp_min_pm2_5 = min(thisFrame.concPM2_5_CF1, tmp_min_pm2_5);
    tmp_min_pm10_0 = min(thisFrame.concPM10_0_CF1, tmp_min_pm10_0);
    pm1_0 += thisFrame.concPM1_0_CF1;
    pm2_5 += thisFrame.concPM2_5_CF1;
    pm10_0 += thisFrame.concPM10_0_CF1;
    i++;
    Serial.print("O:");
    Serial.print(i);
    Serial.print(":");
    Serial.print(thisFrame.concPM2_5_CF1);
    Serial.print(", sum=");
    Serial.println(pm2_5);
  }
  else {
    Serial.print("*");
  }
  
 } // end of "while(i<=MEAN_NUMBER)"


 
  Serial.print("after while(i)");
  Serial.print(pm2_5-tmp_max_pm2_5-tmp_min_pm2_5);
  Serial.print(" / ");
  Serial.print(MEAN_NUMBER-2);
 


  unsigned int pm1_0_mean = (pm1_0-tmp_max_pm1_0-tmp_min_pm1_0)/(MEAN_NUMBER-2);
  unsigned int pm2_5_mean = (pm2_5-tmp_max_pm2_5-tmp_min_pm2_5)/(MEAN_NUMBER-2);
  unsigned int pm10_0_mean = (pm10_0-tmp_max_pm10_0-tmp_min_pm10_0)/(MEAN_NUMBER-2);

    
  sprintf(printbuf, "[Checksum OK]");
  sprintf(printbuf, "%s PM1.0 = %02x, PM2.5 = %02x, PM10 = %02x", printbuf, 
  pm1_0_mean,    pm2_5_mean,   pm10_0_mean);


  sprintf(printbuf, "%s [max =%02x,%02x,%02x, min = %02x,%02x,%02x]", printbuf,
  tmp_max_pm1_0, tmp_max_pm2_5, tmp_max_pm10_0,
  tmp_min_pm1_0, tmp_min_pm2_5, tmp_min_pm10_0); 

  Serial.println();
  Serial.println(printbuf);


  // Hex 가 아닌 10 진수로 보기 쉽게 다시 표시함
  Serial.print("PM 1.0 Original = ");
  Serial.println(pm1_0_mean);

  Serial.print("PM 2.5 Original = ");
  Serial.println(pm2_5_mean);

  Serial.print("PM 10 Original = ");
  Serial.println(pm10_0_mean);

  Serial.print("dht temperature modified = ");
  Serial.println(dht_te_modi);
  Serial.print("dht humidity modified = ");
  Serial.println(dht_hu_modi);

  if(pm2_5_mean > REF_PM25_WARN){
   for(int k=1; k<6; k++){
    u8g2.firstPage(); 
    do{ 
        //u8g2.drawBitmap(19,13,8,44,dust_64x44); 
        u8g2.setFont(u8g2_font_fub20_tr);
        u8g2.drawStr(20,30,"Bad");
        u8g2.drawStr(40,60,"Air!");
    }while(u8g2.nextPage());     
    delay(2000);
    
    u8g2.firstPage(); 
    do{ 
        //u8g2.drawBitmap(19,13,8,44,dust_64x44); 
        u8g2.setFont(u8g2_font_fub20_tr);
        u8g2.drawStr(25,30,"Bad");
        u8g2.drawStr(60,60,"Air!");
    }while(u8g2.nextPage());     
    delay(2000);    
   }
  }
  
  u8g2.firstPage();
  do{
        /*
        u8g2.setFont(u8g2_font_fur11_tr);  
        u8g2.drawStr(14,12,"PM:"); 
        u8g2.drawStr(42,12,"(");       
        u8g2.setPrintPos(48,12); // top line
        u8g2.print(pm1_0_mean); 
        u8g2.drawStr(65,12,",");
             
        u8g2.setPrintPos(73,12); // top line
        u8g2.print(pm2_5_mean);
        u8g2.drawStr(90,12,","); 
        
        u8g2.setPrintPos(98,12); // top line
        u8g2.print(pm10_0_mean); 
        u8g2.drawStr(115,12,")"); */

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


        
      

        //-----------
        /*

        if((pm2_5_mean < 16) && (pm10_0_mean<31)) {        
          u8g2.drawStr(90,60,"Good"); 
           
        }else if((pm2_5_mean < 36) && (pm10_0_mean<61)){           
          u8g2.drawStr(80,60,"Normal");

        }else if( (pm2_5_mean < 76) && (pm10_0_mean<151) ){          
          u8g2.drawStr(92,60,"Bad");

        }else{          
          u8g2.drawStr(88,60,"Death");
        }  */
       
   }while (u8g2.nextPage());
   delay(15000);  
   
   pm1_0=pm2_5=pm10_0=i=0; // initialize()
   delay(10);
 
}
