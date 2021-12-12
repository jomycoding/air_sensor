const int numReadings = 18; //

float readings[numReadings]={0.0};      // the readings from the analog input
int index = 0;                  // the index of the current reading
float total = 0;                  // the running total
float average = 0;                // the average
int first =1;


float mv_avg(float input_value){

       
    // subtract the last reading:
    total = total - readings[index];         
    // read from the sensor:     
    readings[index] = input_value;        
    if(first)  readings[0]=7.0; 
    // add the reading to the total:
    total = total + readings[index];       
    // advance to the next position in the array:  
    index = index + 1;                    
    
    // if we're at the end of the array...
    if (index >= numReadings){              
    // ...wrap around to the beginning: 
    index = 0;    
    first = 0;                       
    }
    
    // calculate the average:
    if(first) {
      average = total / (float)index ;
      //average = total / (float)numReadings; 
      //average = 0;
    }
    else {
      average = total / (float)numReadings;    
    }

    return average;
}
