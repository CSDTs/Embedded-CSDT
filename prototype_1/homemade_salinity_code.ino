#include <SoftwareSerial.h>
#define rxpin 2
#define txpin 3


SoftwareSerial myserial(rxpin, txpin);


String inputstring = "";
String sensorstring = ""; 
boolean input_stringcomplete = false;
boolean sensor_stringcomplete = false;

int sensorPin = A5;
int switchPin1 = 8;
int switchPin2 = 9;

long readingTime = 0;
int samplePeriod = 1000; //ms

double calLowTrue = 10;
double calLowMeasured = 10;
double calMidTrue = 100;
double calMidMeasured = 100;
double calHighTrue = 1000;
double calHighMeasured = 1000;

double calOffset = 0;
double calSlope = 1;

void setup() {
  // declare the ledPin as an OUTPUT:
  Serial.begin(9600);
  pinMode(switchPin1, OUTPUT);
  pinMode(switchPin2, OUTPUT);
  myserial.begin(38400);
  inputstring.reserve(5);
  sensorstring.reserve(30);
}

void loop() {
  // read the value from the sensor:
  if (readingTime < millis()) {
    double val = readSalinity();
    Serial.print((val-calOffset)/calSlope);Serial.println(" ohms");
    delay(10);
    readingTime = millis() + samplePeriod;
  }
  if (input_stringcomplete){
    //if you get a string with the word cal in it
    if(inputstring.indexOf("cal")>-1) {
      getCalibrationValues(inputstring);
      calculateLeastSquares();
    }
    else {
      myserial.print(inputstring);
    }
    inputstring = "";
    input_stringcomplete = false;
  }
  if (sensor_stringcomplete){
   Serial.print(sensorstring);
   sensorstring = "";
   sensor_stringcomplete = false;
  }
  while (myserial.available()) {
   char inchar = (char)myserial.read();                                  //get the new char
   sensorstring += inchar;                                               //add it to the sensorString
    Serial.print(inchar);    //while a char is holding in the serial buffer
   if (inchar == '\r') {sensor_stringcomplete = true;}                   //if the incoming character is a <CR>, set the flag
  }
  while (Serial.available()) {
    Serial.print("4");                                                         //if the hardware serial port receives a char
   char inchar = (char)Serial.read();                               //get the char we just received
   inputstring += inchar;                                           //add it to the inputString
   if(inchar == '\r') {input_stringcomplete = true;}                //if the incoming character is a <CR>, set the flag
  }
}

double readSalinity() {
  double sensorValue = 0;
  boolean isSwitch1 = true;
  double measuredVoltage = 0;
  double appliedVoltage = 5.0;
  double digitalToVoltage = appliedVoltage / 1024;
  long resistor = 670;
  //read one direction
  for(int i = 0; i<5; i++) {
    digitalWrite(switchPin1,isSwitch1);
    digitalWrite(switchPin2,!isSwitch1);
    measuredVoltage = digitalToVoltage*analogRead(sensorPin);
    sensorValue += (measuredVoltage*resistor)/(appliedVoltage - measuredVoltage);
    //read other direction
    isSwitch1 = !isSwitch1;
    digitalWrite(switchPin1,isSwitch1);
    digitalWrite(switchPin2,!isSwitch1);
    measuredVoltage = digitalToVoltage*analogRead(sensorPin);
    sensorValue += (appliedVoltage*resistor)/measuredVoltage - resistor;
    isSwitch1 = !isSwitch1;
  }
  //off
  digitalWrite(switchPin1,false);
  digitalWrite(switchPin2,!false);
  return sensorValue / 10;
}
//select low, mid, or high points
void getCalibrationValues(String input){
  char buffer[10];
  input.substring(input.indexOf(',')+1).toCharArray(buffer, 10);
  float trueValue = atof(buffer);
  input.toLowerCase();
  if (trueValue==0) {
    Serial.println("failed, bad input");
    return;
  }
  if(input.indexOf('low')>0) {
    calLowTrue = trueValue;
    calLowMeasured = readSalinity();
  }
  else if(input.indexOf('mid')>0) {
    calMidTrue = trueValue;
    calMidMeasured = readSalinity();
  }
  else if(input.indexOf('high')>0) {
    calHighTrue = trueValue;
    calHighMeasured = readSalinity();
  }
}

void calculateLeastSquares() {
  double Xave = (calLowTrue + calMidTrue + calHighTrue)/3;
  double SSxx = sq(calLowTrue-Xave) + sq(calMidTrue-Xave) + sq(calHighTrue-Xave);
  double Yave = (calLowMeasured + calMidMeasured + calHighMeasured)/3;
  double SSxy = (calLowMeasured*calLowTrue) + (calMidMeasured*calMidTrue) + (calHighMeasured*calHighTrue) - (3*Xave*Yave);
  calSlope = SSxy/SSxx;
  calOffset = Yave - calSlope*Xave;
}
