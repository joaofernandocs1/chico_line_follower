/*#include <Arduino.h>
#include <WiFi.h>

char ssid[] = "Roberto"; // "PC CASA" //"VIVO-EC58" // "Roberto" // "REDE JOAO"
char senha[] = "123gio123"; // "D0C6G01431" //"74ceJdRMP4" // "123gio123" // "jo@o1234"

WiFiServer server(80);

void setup() {
   Serial.begin(115200);

   Serial.print("Conectando a: "); Serial.println(ssid);
   WiFi.begin(ssid, senha);

   while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.println(".");
   }

   Serial.println(" ");
   Serial.println("WiFi conectada.");
   Serial.println("Endereço IP: ");
   Serial.println(WiFi.localIP());
   delay(3000);

   server.begin();
}

void loop() {

}*/
