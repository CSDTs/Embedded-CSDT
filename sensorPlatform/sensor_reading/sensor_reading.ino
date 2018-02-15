int pin1 = A0;    // led voltage
int pin2 = A1;
int led1 = 0;  // variable to store the value coming from the sensor
int led2 = 0;  

void setup()
{
  Serial.begin(9600);              //  setup serial
}

void loop() {
  Serial.println("-------------------");
  Serial.flush();
  // read the value from the sensor:
  led1 += analogRead(pin1);
  led2 += analogRead(pin2);
  delay(led1);
  delay(led2);
  led1 = (float)led1*5.01/1024.0;
  led2 = (float)led2*5.01/1024.0;
  Serial.println(led1*11.0);
  Serial.flush();
  Serial.println(led2*11.0);
  Serial.flush();
}
