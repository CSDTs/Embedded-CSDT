/*
  Analog Input
 Demonstrates analog input by reading an analog sensor on analog pin 0 and
 turning on and off a light emitting diode(LED)  connected to digital pin 13. 
 The amount of time the LED will be on and off depends on
 the value obtained by analogRead(). 
 
 The circuit:
 * Potentiometer attached to analog input 0
 * center pin of the potentiometer to the analog pin
 * one side pin (either one) to ground
 * the other side pin to +5V
 * LED anode (long leg) attached to digital output 13
 * LED cathode (short leg) attached to ground
 
 * Note: because most Arduinos have a built-in LED attached 
 to pin 13 on the board, the LED is optional.
 
 
 Created by David Cuartielles
 modified 30 Aug 2011
 By Tom Igoe
 
 This example code is in the public domain.
 
 http://arduino.cc/en/Tutorial/AnalogInput
 
 */

int sensorPin = A5;    // select the input pin for the potentiometer
// Never set both switch pins high at once, both off is off, switch back and forth for AC
int switchPin1 = 8;
int switchPin2 = 9;
long readingTime = 0;

int samplePeriod = 800;
int drySalinity1 = 0;
int drySalinity2 = 0;

void setup() {
  // declare the ledPin as an OUTPUT:
  Serial.begin(9600);
  pinMode(switchPin1, OUTPUT);
  pinMode(switchPin2, OUTPUT);
  getDryProbeValues();
}

void loop() {
  // read the value from the sensor:
  if (readingTime < millis()) {
    Serial.println(readSalinity());
    readingTime = millis() + samplePeriod;
  }
  
    
}

float readSalinity() {
  float sensorValue = 0;
  boolean isSwitch1 = true;
  float val1 = 0;
  //read one direction
  for(int i = 0; i<10; i++) {
    digitalWrite(switchPin1,isSwitch1);
    digitalWrite(switchPin2,!isSwitch1);
    sensorValue += analogRead(sensorPin) - drySalinity1;
    //read other direction
    isSwitch1 = !isSwitch1;
    digitalWrite(switchPin1,isSwitch1);
    digitalWrite(switchPin2,!isSwitch1);
    sensorValue += drySalinity2 - analogRead(sensorPin);
    isSwitch1 = !isSwitch1;
  }
  //off
  digitalWrite(switchPin1,false);
  digitalWrite(switchPin2,!false);
  return sensorValue / 20;
}

float getDryProbeValues() {
  boolean isSwitch1 = true;
  //read one direction
  digitalWrite(switchPin1,isSwitch1);
  digitalWrite(switchPin2,!isSwitch1);
  drySalinity1 = analogRead(sensorPin);
  isSwitch1 = false;
  digitalWrite(switchPin1,isSwitch1);
  digitalWrite(switchPin2,!isSwitch1);
  drySalinity2 = analogRead(sensorPin);
  digitalWrite(switchPin1,false);
  digitalWrite(switchPin2,!false);
}
