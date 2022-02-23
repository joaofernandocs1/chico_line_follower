/*#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WebSocketServer.h>

// CONFIGS WIFI

char ssid[] = "VIVO-EC58"; // "PC CASA" //"VIVO-EC58" // "Roberto" // "REDE JOAO"
char senha[] = "74ceJdRMP4"; // "D0C6G01431" //"74ceJdRMP4" // "123gio123" // "jo@o1234"

bool anda = false;

WiFiServer servidor(80);
WiFiClient cliente;

WebSocketServer webSocketServidor;

bool openCon = false;

// PARAMS
//float bat = 1000.1;
//float erro = 2000.2;
//float diff = 3000.3;

//TESTE BAT
long bat;

// TESTE SIGNS
long erro;
long diff;

// CONS
float kp = 4000.4;
float ki = 5000.5;
float kd = 6000.6;

// CONFS

String rede = ssid;
String endBT = "endMacBT";

// VMÁX
int vmax = 1024;

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
   Serial.println(WiFi.localIP().toString());
   delay(1000);

   servidor.begin();
   delay(100);

}

// RECEBER


void procRecCons (String message) { // processar JSON chegando como String

   const size_t capacity = JSON_OBJECT_SIZE(3) + 80;
   DynamicJsonDocument doc(capacity);
   DeserializationError error = deserializeJson(doc, message);

   if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
   }

   float kp = doc["kp"];
   float ki = doc["ki"];
   float kd = doc["kd"];

   Serial.println(kp);
   Serial.println(ki);
   Serial.println(kd);

}

void procRecOrd (String message) {

   const size_t capacity = JSON_OBJECT_SIZE(1) + 30;
   DynamicJsonDocument doc(capacity);
   DeserializationError error = deserializeJson(doc, message);

   if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
   }

   String ordem = doc["ordem"];

   if (ordem == "start" && anda == false) {
      anda = true;
      Serial.println("Chiquindo está andando!");
   }
   else if (ordem == "stop" && anda == true) {
      anda = false;
      Serial.println("Chiquinho parou!");
   }
   else {
      anda = false;
      Serial.println("Chiquinho parou!");
   }
}

void procSenConf () {

   String message;
   const size_t capacity = JSON_OBJECT_SIZE(4) + 130;
   DynamicJsonDocument doc(capacity);

   doc["Rede"] = ssid;
   doc["IP"] = WiFi.localIP().toString();
   doc["BT"] = endBT;
   serializeJson(doc, message);

   Serial.print("Json enviado: "); Serial.println(message);
   webSocketServidor.sendData(message);

   // message = {"rede": "nomeDaRede", "ip": "123.456.789.000", "BT": "nomeUUID nomeUUID nomeUUID"}
}

void procSenCons() {

   String message;
   const size_t capacity = JSON_OBJECT_SIZE(3) + 80;
   DynamicJsonDocument doc(capacity);

   doc["kp"] = kp;
   doc["ki"] = ki;
   doc["kd"] = kd;
   serializeJson(doc, message);

   Serial.print("Json enviado: "); Serial.println(message);
   webSocketServidor.sendData(message);

   // message = {"kp": 4000.4, "ki": 5000.5, "kd": 6000.6}
}

void procSenSigns() {

   String message;
   const size_t capacity = JSON_OBJECT_SIZE(3) + 70;
   DynamicJsonDocument doc(capacity);

   doc["erro"] = erro;
   doc["diff"] = diff;
   serializeJson(doc, message);

   Serial.print("Json enviado: "); Serial.println(message);
   webSocketServidor.sendData(message);

   // message = {"erro": 2000.2, "diff": 3000.3}
}

void procRecVel(String message) {

   const size_t capacity = JSON_OBJECT_SIZE(1) + 30;
   DynamicJsonDocument doc(capacity);
   DeserializationError error = deserializeJson(doc, message);

   if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
   }

   vmax = doc["vmax"];

   Serial.println("Vmax: ");
   Serial.println(vmax);

   // message = {"vmax": 999}
}

void procSenBat() {

   String message;
   const size_t capacity = JSON_OBJECT_SIZE(1) + 30;
   DynamicJsonDocument doc(capacity);

   doc["bat"] = bat;
   serializeJson(doc, message);

   Serial.print("Json enviado: "); Serial.println(message);
   webSocketServidor.sendData(message);

   // message = {"bat": 9999}
}

bool chkConnection() {

   if (!openCon) {
      cliente = servidor.available(); // se há clientes disponíveis ou não

      if (cliente) {
         Serial.println("Novo cliente.");

         if (cliente.connected()) {
            Serial.println("Cliente WiFi se conectou.");

            if (webSocketServidor.handshake(cliente)) {
               Serial.println("Websocket estabelecido.");
               openCon = true;
            }
         }
         else {
            //Serial.println("Cliente não se conectou.");
         }
      }
      else {
         //Serial.println("Não há cliente.");
      }
   }
   else if (openCon) {
      //Serial.println("Websocket já estabelecido.");
   }

   return openCon;
}

void chkData () {

   String recData;

   if (cliente.available()) { // verifica se há dados chegando do cliente

      recData = webSocketServidor.getData();
      delay(100); // delay NECESSÁRIO para receber dados corretamente

      if (recData.length() > 0) {

         Serial.print("Chegou mensagem: ");
         Serial.println(recData);

         if (recData == "rec") {

            Serial.print("A mensagem é de tráfego: ");
            Serial.println(recData);

            recData = webSocketServidor.getData();
            delay(100);

            if (recData.length() > 0) {
               Serial.print("Chegaram metadados: ");
               Serial.println(recData);

               if (recData == "cons") {
                  recData = webSocketServidor.getData(); // getData usa String args
                  delay(100);

                  if (recData.length() > 0) {
                     Serial.print("Chegaram dados Json: ");
                     Serial.println(recData);

                     procRecCons(recData);
                  }
               }
               else if (recData == "ord") {
                  recData = webSocketServidor.getData();
                  delay(100);

                  if (recData.length() > 0) {
                     Serial.print("Chegaram dados: ");
                     Serial.println(recData);

                     procRecOrd(recData);
                  }
               }
               else if (recData == "vel") {
                  recData = webSocketServidor.getData();
                  delay(100);

                  if (recData.length() > 0) {
                     Serial.print("Chegaram dados: ");
                     Serial.println(recData);

                     procRecVel(recData);
                  }
               }
            }
         }
         else if (recData == "send") {
            Serial.print("A mensagem é de tráfego: ");
            Serial.println(recData);

            recData = webSocketServidor.getData();
            delay(100);

            if (recData.length() > 0) {
               Serial.print("Chegaram metadados: ");
               Serial.println(recData);

               if (recData == "cons") {

                  procSenCons();
               }
               else if (recData == "conf") {

                  procSenConf();
               }
               else if (recData == "signs") {

                  erro = random(1000, 2000); // random só aceita variáveis tipo long
                  diff = random(0, 1000);

                  procSenSigns();
               }
               else if (recData == "bat") {

                  bat = random(0, 9);

                  procSenBat();
               }
            }
         }
      }

      //delay(100); // verificar necessidade
   }
   else {
      //Serial.println("Não há dados do cliente.");
   }

}

void loop () {
   if (chkConnection()) {
      chkData();
   }
}
*/
