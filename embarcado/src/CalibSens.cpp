/*#include <Arduino.h>

// PINAGEM

#define pin_sen_2E 36 // sensor 2E
#define pin_sen_1E 39 // sensor 1E

#define pin_sen_CT 34 // sensor 0

#define pin_sen_1D 35 // sensor 1D
#define pin_sen_2D 32 // sensor 2D

#define pin_sen_NB 33 // sensor nível de bateria

int sen2E = 0; // sensor 2E  ****
int sen1E = 0; // sensor 1E  ****
int senCT = 0; // sensor CT  ---->>>>>> (Direção na vista superior)
int sen1D = 0; // sensor 1D  ****
int sen2D = 0; // sensor 2D  ****

int senNB; // sensor nível de bateria |||

float soma_sen2E = 0;
float soma_sen1E = 0;
float soma_senCT = 0;
float soma_sen1D = 0;
float soma_sen2D = 0;

int medidas = 0;

void setup()
{
   Serial.begin(38400);
}

void loop()
{
   if (medidas <= 100)
   {
      sen2E = analogRead(pin2E);
      sen1E = analogRead(pin1E);
      senCT = analogRead(pinCT);
      sen1D = analogRead(pin1D);
      sen2D = analogRead(pin2D);

      Serial.print(sen2E); Serial.print(" ");
      Serial.print(sen1E); Serial.print(" ");
      Serial.print(senCT); Serial.print(" ");
      Serial.print(sen1D); Serial.print(" ");
      Serial.print(sen2D); Serial.print(" ");
      Serial.println(senNB);

      soma_sen2E += sen2E;
      soma_sen1E += sen1E;
      soma_senCT += senCT;
      soma_sen1D += sen1D;
      soma_sen2D += sen2D;

      medidas++;
   }

   if (medidas == 101)
   {
      soma_sen2E = abs(soma_sen2E/100);
      soma_sen1E = abs(soma_sen1E/100);
      soma_senCT = abs(soma_senCT/100);
      soma_sen1D = abs(soma_sen1D/100);
      soma_sen2D = abs(soma_sen2D/100);

      Serial.println("C A L I B R A G E N S");
      Serial.println(" ");
      Serial.print("sen2E com: "); Serial.print(soma_sen2E); Serial.print(" ");
      Serial.print("sen1E com: "); Serial.print(soma_sen1E); Serial.print(" ");
      Serial.print("senCT com: "); Serial.print(soma_senCT); Serial.print(" ");
      Serial.print("sen1D com: "); Serial.print(soma_sen1D); Serial.print(" ");
      Serial.print("sen2D com: "); Serial.println(soma_sen2D);

      delay(20000); // anotar as medidas

   }
   else if (medidas == 101) {
      medidas = 0;
   }

   senNB = analogRead(senNB);
   Serial.print(senNB);

}*/
