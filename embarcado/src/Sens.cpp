/*#include <Arduino.h>

#define pin2E 36 // sensor -3
#define pin1E 39 // sensor -2

#define pinCT 34 // sensor -1

#define pin1D 35 // sensor 1
#define pin2D 32 // sensor 2

#define pinNB 33 // sensor de nível de bateria

int sen2E; // sensor -2  ****
int sen1E; // sensor -1  ****

int senCT; // sensor 0   ----

int sen1D; // sensor 1   ****
int sen2D; // sensor 2   ****

int senNB; // sensor nível de bateria |||

void setup()
{
  Serial.begin(19200);
}

void loop()
{
  sen2E = analogRead(pin2E);
  sen1E = analogRead(pin1E);
  senCT = analogRead(pinCT);
  sen1D = analogRead(pin1D);
  sen2D = analogRead(pin2D);
  senNB = analogRead(pinNB);

  Serial.print(sen2E); Serial.print(" ");
  Serial.print(sen1E); Serial.print(" ");
  Serial.print(senCT); Serial.print(" ");
  Serial.print(sen1D); Serial.print(" ");
  Serial.print(sen2D); Serial.print(" ");
  Serial.println(senNB);

  //SAÍDA: sen2E sen1E senCT sen1D sen2D senNB
  //delay(200);
}*/
