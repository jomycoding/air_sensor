/* Grove - Dust Sensor v2.0
 added LED that responds to moving average
 Interface to Shinyei Model PPD42NS Particle Sensor
 
 http://www.seeedstudio.com/depot/grove-dust-sensor-p-1050.html
 http://www.sca-shinyei.com/pdf/PPD42NS.pdf
 
 JST Pin 1 (Black Wire)  => Arduino GND
 JST Pin 2 (Brown) => Arduino Pin 7 (P2, variable output)
 JST Pin 3 (Red wire)    => Arduino 5VDC
 JST Pin 4 (Yellow wire) => Arduino Digital Pin 8 (P1, fixed 1mu output)
 JST Pin 5 (Orange) => Arduino Pin 13 (T1, threshold for P2)
 */


int P1pin = 8;
unsigned long P1duration;
unsigned long startTime_ms;

unsigned long sampleTime_ms = 10000;  //sample time 10s;
float dust_sensitivity = 12.0;  //

unsigned long P1lowPulseOccupancy = 0;
float ledSignal = 0;

float P1ratio = 0;
float P1concentration = 0;

void ppd42ns_init(){
  pinMode(P1pin,INPUT);
 // startTime_ms = millis();//get the current time;
}



void ppd42ns_loop(){
  P1lowPulseOccupancy = 0;
  startTime_ms = millis();//get the current time;
  
  while( (millis()-startTime_ms) <= sampleTime_ms) {      //my modification
    P1duration = pulseIn(P1pin, LOW);
    P1lowPulseOccupancy = P1lowPulseOccupancy+P1duration;
  }

   
  P1ratio = P1lowPulseOccupancy/(sampleTime_ms*10.0);  // Integer percentage 0=>100
  P1concentration = 1.1*pow(P1ratio,3)-3.8*pow(P1ratio,2)+520*P1ratio+0.62; // using spec sheet curve
   
  float dustDensity = P1concentration/130.0 * dust_sensitivity;  //130
   
  String dst1 = digits3(dustDensity);
  const char* cdst1=(const char*) dst1.c_str();   

  if(dustDensity>299.0) dustDensity=299.0; // limit maximum value      
  float m_dustDensity = mv_avg(dustDensity);

  String dst2 = digits3(m_dustDensity);
  const char* cdst2=(const char*) dst2.c_str();

  char cdst3[10]; 
  dtostrf(dustDensity , 3, 0, cdst3);  // 3 : width, 0 : precision
  char cdst4[10]; 
  dtostrf(m_dustDensity , 3, 1, cdst4);  // 3 : width, 0 : precision
  
  if(m_dustDensity > 75.0){
    u8g2.firstPage(); 
    do{ 
        //u8g2.drawBitmap(12,0,13,64,high); 
        u8g2.setFont(u8g2_font_fub20_tr);
        u8g2.drawStr(20,48,"Dust");
    }while(u8g2.nextPage()); 
    delay(3000);
  } 

    
  u8g2.firstPage(); 
  do{ 
    u8g2.setFont(u8g2_font_fur11_tr);
    u8g2.drawStr(80,12,cdst1); // for 2 color oled top
    u8g2.drawStr(112,57,"ug");
      
     u8g2.setFont(u8g2_font_fub42_tn);
    u8g2.drawStr(11,62,cdst2); // for 2 color oled body
      
   
    if(dustDensity>75.0) {
      u8g2.setFont(u8g2_font_fur11_tr);
      u8g2.drawStr(20,12,"*****");
     }    
  }while(u8g2.nextPage()); 
  delay(3000);   
 
}
