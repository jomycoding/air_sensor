
#include "jm_bitmap.h"
//#include "moving_average.h"
#include "jm_func.h"
#include "jm_oled.h" 
#include "jm_dht.h"
//#include "jm_ppd42ns.h"
//#include "jm_pms7003.h"
#include "jm_pms7003_nodemcu.h"



void setup() {

  //Serial.begin(9600);
  Serial.begin(115200);
 
  Wire.begin(); 
  oled_init();  

  pms7003_nodemcu_init();
  //ppd42ns_init();

  dht.begin();
  dht_init();
  
}


void loop() {

  //ppd42ns_loop();
  //pms7003_loop();
  //pms7003_nodemcu_loop();
  pms7003_nodemcu_PM25();
  dht_loop();
  
}
