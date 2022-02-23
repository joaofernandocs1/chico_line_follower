/*#include <Arduino.h>
#include <LiquidCrystal.h>
#include <WiFi.h>

// LiquidCrystal lcd(<pino RS>, <pino enable>, <pino D4>, <pino D5>, <pino D6>, <pino D7>)
LiquidCrystal lcd(19, 18, 5, 17, 16, 4);
// NÃO USAR GPIO 0

// CONFIGS WIFI

char ssid[] = "PC CASA"; // "PC CASA" // "VIVO-EC58" // "Roberto" // "REDE JOAO"
char senha[] = "D0C6G01431"; // "D0C6G01431" // "74ceJdRMP4" // "123gio123" // "jo@o1234"

String endIP;
String endBT = "endServBT";

#define ch1 12
#define ch2 13
#define ch3 21

int telacont = 1;

boolean VAI = false;
boolean VOLTA = false;
boolean BT_CON = false;
boolean ACTION = false;
boolean anda = false;

float kp = 237.4;
float ki = 70.5;
float kd = 3009.1;

void setup()
{
   Serial.begin(57600);

   Serial.print("Conectando a: "); Serial.println(ssid);
   WiFi.begin(ssid, senha);

   while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println(".");
   }

   Serial.println(" ");
   Serial.println("WiFi conectada.");
   Serial.println("Endereço IP: ");
   endIP = WiFi.localIP().toString();
   Serial.println(endIP);
   delay(1000);

   lcd.begin(16, 2); // (colunas, linhas)
   lcd.setCursor(0, 0);
   lcd.print("CHIQUINLIFESTYLE");

   delay(3000); // testar outra função pra esperar enquanto node inicializa
}
void loop()
{
   int senNB = analogRead(33);

   VAI = digitalRead(ch1); // ADIANTA TELA
   VOLTA = digitalRead(ch2); // VOLTA TELA
   ACTION = digitalRead(ch3); // AÇÃO

   if (VAI && telacont == 8)
   {
      telacont = 1;
   }
   else if (VAI)
   {
      telacont++;
   }

   if (VOLTA && telacont > 0)
   {
      telacont--;
   }
   else if (VOLTA && telacont == 1)
   {
      telacont = 8;
   }

   if (telacont != 0)
   {
      switch (telacont)
      {
         case 1:
            lcd.clear();
            //lcd.setCursor(0, 0); <<<---- TESTAR PARA TODOS OS MENUS
            lcd.print("Wi-Fi: ");
            lcd.setCursor(0, 1);
            lcd.print(ssid);

            break;

         case 2:
            lcd.clear();
            lcd.print("End. IP: ");
            lcd.setCursor(0, 1);
            lcd.print(endIP);

            break;

         case 3:
            lcd.clear();
            lcd.print("Bluetooth: ");
            lcd.setCursor(0, 1);

            if (BT_CON)
            {
               lcd.print(endBT);
            }

            if (ACTION)
            {
               if (BT_CON)
               {
                  lcd.print("BT OFF");
                  BT_CON = false;
               }
               else
               {
                  lcd.print("BT ON");
                  BT_CON = true;
               }
            }

            break;

         case 4:
            lcd.clear();
            lcd.print("Kp: ");
            lcd.setCursor(0, 1);
            lcd.print(kp);

            break;

         case 5:
            lcd.clear();
            lcd.print("Ki: ");
            lcd.setCursor(0, 1);
            lcd.print(ki);

            break;

         case 6:
            lcd.clear();
            lcd.print("Kd: ");
            lcd.setCursor(0, 1);
            lcd.print(kd);

            break;

         case 7:
            lcd.clear();
            lcd.print("Bateria: ");
            lcd.setCursor(0, 1);
            lcd.print((100*senNB)/4095); lcd.print("% "); lcd.print((8.4*senNB)/4095); lcd.print("V");

            break;

         case 8:
            lcd.clear();
            lcd.print("START / STOP");
            lcd.setCursor(0, 1);

            if (ACTION)
            {
               if (anda)
               {
                  lcd.print("Parado!");
                  anda = false;
               }
               else
               {
                  lcd.print("Andando!");
                  anda = true;
               }
            }

            break;

         default:
            telacont = 0;
      }
   }



}*/
