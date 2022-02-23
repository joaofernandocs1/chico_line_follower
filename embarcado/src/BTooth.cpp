/*#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Arduino.h>

//  F U N C I O N A N D O  //


String buff_to_parse;
String buff_to_send = "";

String metadados;
String dados;

char aux[8];

int ind;

int sen_2E = 3467;
int sen_1E = 704;
int sen_CT = 20;
int sen_1D = 3901;
int sen_2D = 1113;
int sen_NB = 2340;

int vmax;

String ordem;
bool anda = false;

float Kp;
float Ki;
float Kd;


#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

void setOrd(String ord) {

   ordem = ord;

   if (ordem == "start" && anda == false) {
      anda = true;
      Serial.println("Chiquindo está andando!");
   }
   else if (ordem == "stop" && anda == true) {
      anda = false;
      Serial.println("Chiquinho parou!");
   }

}

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string inc_Data = pCharacteristic->getValue();

      if (inc_Data.length() > 0) {
        buff_to_parse = "";
        for (int i = 0; i < inc_Data.length(); i++){
          buff_to_parse = buff_to_parse + inc_Data[i];
        }

        //Serial.print("buff_to_parse = ");
        //Serial.println(buff_to_parse);

        ind = buff_to_parse.indexOf(",");
        metadados = buff_to_parse.substring(0, ind);

        dados = buff_to_parse.substring(ind + 1);


        //Serial.print("Metadados: ");
        //Serial.print(metadados);
        //Serial.print(" ");
        //Serial.print("Dados: ");
        //Serial.print(dados);
        //Serial.println(" ");

        if (metadados == "Kp") {

           Kp = dados.toFloat();

           //Serial.print("Kp: ");
           //Serial.println(Kp);

        }

        if (metadados == "Ki") {

           Ki = dados.toFloat();

           //Serial.print("Ki: ");
           //Serial.println(Ki);

        }

        if (metadados == "Kd") {

           dados.toCharArray(aux, dados.length() + 1);
           Kd = atof(aux);

           //Serial.print("Kd: ");
           //Serial.println(Kd);

        }

        if (metadados == "vmax") {

           vmax = dados.toInt();

           //Serial.print("V. Máx.: ");
           //Serial.println(vmax);

        }

        if (metadados == "ordem") {

           ordem = dados;

           //Serial.print("Ordem: ");
           //Serial.println(ordem);

           setOrd(ordem);

        }

        if (metadados == "sens") {

           String S_sen_2E = String(sen_2E);
           String S_sen_1E = String(sen_1E);
           String S_sen_CT = String(sen_CT);
           String S_sen_1D = String(sen_1D);
           String S_sen_2D = String(sen_2D);
           String S_sen_NB = String(sen_NB);

           buff_to_send += S_sen_2E;
           //Serial.println(buff_to_send);
           buff_to_send += "," + S_sen_1E;
           //Serial.println(buff_to_send);
           buff_to_send += "," + S_sen_CT;
           //Serial.println(buff_to_send);
           buff_to_send += "," + S_sen_1D;
           //Serial.println(buff_to_send);
           buff_to_send += "," + S_sen_2D;
           //Serial.println(buff_to_send);
           buff_to_send += "," + S_sen_NB;
           //Serial.println(buff_to_send);

           pCharacteristic->setValue(buff_to_send.c_str()); // Devuelve el buff_to_parse.
           //Serial.println(" ");
           //Serial.println(buff_to_send);
           buff_to_send = "";

        }

      }
    }
};

void setup() {

  Serial.begin(38400);

  BLEDevice::init("Chiquinho Lifestyle"); //
  BLEServer *pServer = BLEDevice::createServer();

  BLEService *pService = pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());
  pCharacteristic->setValue("Iniciado.");
  pService->start();

  BLEAdvertising *pAdvertising = pServer->getAdvertising();
  pAdvertising->start();
}

void loop() {
//
}*/
