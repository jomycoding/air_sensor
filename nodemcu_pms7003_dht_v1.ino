void setup() {

  Serial.begin(115200);
 
  Wire.begin(); 
  oled_init();  

  pms7003_nodemcu_init();

  dht.begin();
  dht_init();
  
}


void loop() {
  pms7003_nodemcu_PM25();
  dht_loop();
  
}
