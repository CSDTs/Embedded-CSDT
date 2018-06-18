/*  Putting together the salinity, turbidity, and temperature sensor code
    such that they will work together and be able to talk back and forth
    with another Arduino with the RF95 module
    * SparkFun Arduino Pro Micro HW Serial is Serial, SW Serial is Serial
    * Pins SCK MOSI MISO, 2 and 10 for RF module
    * *Not in yet* Pins 3 and 4 for AtSci probe (NOT IN YET)
    * Pins 7 9 and A1 for handmade probe
    * Pins A0 for turbidity sensor
    * Pins 6 for DS18B20 temp sensor
    * Pins 15 SCK 16 MOSI 14 MISO 8 for SD shield
*/

// For sleep functionality
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/power.h>

// For the temperature sensor
#include <DallasTemperature.h>
#include <OneWire.h>
#define ONE_WIRE_BUS 6

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// For the SD card
#include <SD.h>
#include <SPI.h>

// Sd card declarations
const int chipSelect = 8;

// temp sensor declarations
int Celsius = 0;
long tempDataPoint = 0;

// turbidity sensor declarations
int NTU = 0;
int turbPin = A0;
long turbDataPoint = 0;

// salinity sensor declarations
int sensorPin = A1;
int switchPin1 = 8;
int switchPin2 = 9;
int salDataPoint = 0;

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

int sensorId = 0;
int waitTime = 16/8;
int fullWaitTime = 904/8;
int nbr_remaining;

// Union declarations to send data back and forth to RF modules
union temp {       // to send the celsius value in 4 bytes to the other RF95
  int number;
  uint8_t b[2];
} intCelsius;

union turb {       // to send the turbidity value in 4 bytes to the other RF95
  int number;
  uint8_t b[2];
} intTurbidity;

union sal {       // to send the salinity value in 4 bytes to the other RF95
  int number;
  uint8_t b[2];
} intSal;


// WDT and sleep functionality
ISR(WDT_vect)
{
        // not hanging, just waiting
        // reset the watchdog
        wdt_reset();
}

void configure_wdt(void)
{
  cli();
  MCUSR = 0;                                      
  WDTCSR |= 0b00011000;
  WDTCSR =  0b01000000 | 0b100001; // 8s interrupt
  sei();                           // re-enable interrupts
}

// Put the Arduino to deep sleep. Only an interrupt can wake it up.
void sleep(int ncycles)
{  
  nbr_remaining = ncycles; // defines how many cycles should sleep
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  
  // Turn off the ADC while asleep.
  power_adc_disable();
  
  while (nbr_remaining > 0){ // while some sleep cycles left
    sleep_mode();
    sleep_disable();
    nbr_remaining = nbr_remaining - 1; 
  }
  power_all_enable();
}

void setup()
{
  //------------------------------------------------------
  // Setup WDT
  configure_wdt();
  //-------------------------------------------------------
  // Setup DS18B20
  sensors.begin();
  //-------------------------------------------------------
  // Setup handmade salinity sensor
  pinMode(switchPin1, OUTPUT);
  pinMode(switchPin2, OUTPUT);
  //-------------------------------------------------------
  // Setup SD card
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
  }
  //-------------------------------------------------------
}

void loop()
{
  sleep(fullWaitTime);
  for( sensorId = 0; sensorId < 3; sensorId++) {
    if(sensorId == 0) {
      intTurbidity.number = readTurbiditySensor(NTU);
      logTurbidityData();
      turbDataPoint++;
      delay(500);
    }
    
    else if(sensorId == 1) {
      intCelsius.number = readTempSensor(Celsius);
      logTempData();
      tempDataPoint++;
      delay(500);
    }
    
    else if(sensorId == 2) { 
      intSal.number = readHMSalinity() * .38;
      if(intSal.number < 0) {
        intSal.number = 0;
      }
      logHMSalData();
      salDataPoint++;
      delay(500);
    }
  }
  sensorId = 0; 
}


// Returns Salinity reading of handmade sensor
double readHMSalinity() {
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

bool logHMSalData() {
  //create and fill data string
  String dataString = "2,";
  dataString += salDataPoint;
  dataString += ',';
  dataString += intSal.number;
  // open the file
  File dataFile = SD.open("salData.txt", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    return true;
  }
  // if the file isn't open, pop up an error:
  else {
    return false;
  }
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
    calLowMeasured = readHMSalinity();
  }
  else if(input.indexOf('mid')>0) {
    calMidTrue = trueValue;
    calMidMeasured = readHMSalinity();
  }
  else if(input.indexOf('high')>0) {
    calHighTrue = trueValue;
    calHighMeasured = readHMSalinity();
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

// Returns Turbidity reading off the analog pin
int readTurbiditySensor(int &NTU) {
  int sensorValue = analogRead(A0);
  float voltage = sensorValue * (5.0 / 1024.0);
  NTU = voltage * -1428.3 + 4778.6;
  return NTU;
}

bool logTurbidityData() {
  //create and fill data string := "sensorid,datapointid,reading"
  String dataString = "0,"; 
  dataString += turbDataPoint;
  dataString += ',';
  dataString += intTurbidity.number;
  // open the file
  File dataFile = SD.open("TurbData.txt", FILE_WRITE); 
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    return true;
  }
  // if the file isn't open, pop up an error:
  else {
    return false;
  }
}

// Takes in celsius and returns the new celsius reading
int readTempSensor(int &Celcius) {
  sensors.requestTemperatures();
  Celcius = sensors.getTempCByIndex(0);
  return Celsius;
}

bool logTempData() {
  //create and fill data string := "sensorid,datapointid,reading"
  String dataString = "1,"; 
  dataString += tempDataPoint;
  dataString += ',';
  dataString += intCelsius.number;
  // open the file
  File dataFile = SD.open("TempData.txt", FILE_WRITE);
  // if the file is available, write to it:
  if (dataFile) {
    dataFile.println(dataString);
    dataFile.close();
    return true;
  }
  // if the file isn't open, pop up an error:
  else {
    return false;
  }
}
