/*
 * Example code for DFRobot Turbidity Sensor to test
*/

void setup() {
  Serial.begin(9600); //Baud rate: 9600
}
void loop() {
  float average;
  float readings[10];
  float NTU;
  
  for(int i = 0; i<10; i++) {
    readings[i] = readTurbidity();
    delay(200);
  }
  
  //Following is to take an average of the 10 readings just taken to get an average for this Turbidity
  for(int i = 0; i<10; i++) {
    average += readings[i];
  }
  average = average / 10;
  Serial.print("  Average Voltage: "); Serial.println(average);
  NTU = average * -1428.3 + 6528.6;
  
  Serial.print("  Average NTU: "); Serial.println(NTU); Serial.println();
}

float readTurbidity() {
  int sensorValue = analogRead(A0);// read the input on analog pin 0:
  float voltage = sensorValue * (5.0 / 1024.0); // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  return voltage;
}
