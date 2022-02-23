#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wifi.h>
#include <LiquidCrystal.h>
#include <WebSocketServer.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>



// SERVIDOR E SERVIÇO BLUETOOTH //

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

//

// PWMs E PONTE H

#define pin_pwm_dir 22 // EN A = PWM motor Dir
#define pin_pwm_esq 23 // EN B = PWM motor Esq

#define mot_dir_fr 25 // direita frente
#define mot_dir_re 26 // direita ré

#define mot_esq_fr 14 // esquerda frente
#define mot_esq_re 27 // esquerda ré

#define pwm_freq 25000 // 1000 Hz

#define mot_dir_chn 0 // canal 0 -> motor direita -> motor A
#define mot_esq_chn 1 // canal 1 -> motor esquerda -> motor B

#define pwm_res 12 // 0 - 16 bits de resolução para o sinal. 12 bits = 0 <-> 4095


// PINAGEM

#define pin_sen_2E 36 // sensor 2E
#define pin_sen_1E 39 // sensor 1E

#define pin_sen_CT 34 // sensor 0

#define pin_sen_1D 35 // sensor 1D
#define pin_sen_2D 32 // sensor 2D

#define pin_sen_NB 33 // sensor nível de bateria


// CALIBRAGENS (setpoints)

#define preto_2E 2600
#define branco_2E 350

#define preto_1E 3220
#define branco_1E 200

#define preto_CT 3450
#define branco_CT 100

#define preto_1D 3150
#define branco_1D 320

#define preto_2D 3250
#define branco_2D 280


// LEITURA DO SENSOR (Vista superior)

int sen2E = 0; // sensor 2E  ****
int sen1E = 0; // sensor 1E  ****

int senCT = 0; // sensor CT  ---- PISTA

int sen1D = 0; // sensor 1D  ****
int sen2D = 0; // sensor 2D  ****

int sen_NB; // sensor Nível de Bateria


// DISPLAY

LiquidCrystal lcd(19, 18, 5, 17, 16, 4);

int menu_index = 1;
int const_index = 1;

bool troca_index = false;
bool trocou;

// false: roda index de informações/configurações
// true: roda index de constantes PID

// BOTÕES

int bot_1;
int bot_2;
int bot_3;

bool stat_bot_1 = false;
bool stat_bot_2 = false;
bool stat_bot_3 = false;

bool stat_bot_1_e_3 = false;

const int ch1 = 12;
const int ch2 = 13;
const int ch3 = 21;


// CONFIGURAÇÕES

// Rede = ssid
// IP = WiFi.localIP().toString())
String endBT = "MAC BLE";


// SINAIS

float u_diff;
//float erro;


// ERROS

float erro_pos_dir;
float erro_neg_esq;

float erro_ant_neg_esq = 0;
float erro_ant_pos_dir = 0;

int sign = -1;


// VELOCIDADES E ESTADO

int pwm_val_dir = 100;
int pwm_val_esq = 100;

int pwm_max_val = 3700;

bool anda = true;

String ordem = "";

// PID

float Kp = 0.0007; // 0,0007// 0.00823 estoura
float Ki = 2.6847; // 2,6847
float Kd = 0.8887; // 0,8887

float bloco_p;
float bloco_i;
float bloco_d;

unsigned long chk_point = 0;
unsigned long delta; // testar float e double

boolean Dir;


// WI FI

char ssid[] = "myssid"
char senha[] = "mypassword"

WiFiServer servidor(80);
WiFiClient cliente;

WebSocketServer webSocketServidor;

bool openCon = false;


// BLUETOOTH

String buff_to_parse;
String buff_to_send = "";

String metadados;
String dados;

char aux[8];

int ind;

void setOrd(String ord) {

   ordem = ord;

   if (ordem == "start" && anda == false) {
      anda = true;
      //Serial.println("Chiquindo está andando!");
   }
   else if (ordem == "stop" && anda == true) {
      anda = false;
      //Serial.println("Chiquinho parou!");
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

            pwm_max_val = dados.toInt();

            //Serial.print("V. Máx.: ");
            //Serial.println(pwm_max_val);

         }

         if (metadados == "ordem") {

            ordem = dados;

            //Serial.print("Ordem: ");
            //Serial.println(ordem);

            setOrd(ordem);

         }

         if (metadados == "sens") {

            String S_sen_2E = String(sen2E);
            String S_sen_1E = String(sen1E);
            String S_sen_CT = String(senCT);
            String S_sen_1D = String(sen1D);
            String S_sen_2D = String(sen2D);
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

            pCharacteristic->setValue(buff_to_send.c_str());
            //Serial.println(" ");
            //Serial.println(buff_to_send);
            buff_to_send = "";

         }

      }
   }
};

//

void setup() {

   //Serial.begin(38400);

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


   WiFi.begin(ssid, senha);
   servidor.begin();


   lcd.begin(16, 2); // (colunas, linhas)
   lcd.clear();
   lcd.setCursor(0, 0);
   lcd.print("Iniciando...");
   delay(1000);
   lcd.clear();

   pinMode(ch1, INPUT);
   pinMode(ch2, INPUT);
   pinMode(ch3, INPUT);

   pinMode(mot_dir_fr, OUTPUT);
   pinMode(mot_dir_re, OUTPUT);
   pinMode(mot_esq_re, OUTPUT);
   pinMode(mot_esq_fr, OUTPUT);

   ledcSetup(mot_dir_chn, pwm_freq, pwm_res);
   ledcSetup(mot_esq_chn, pwm_freq, pwm_res);

   ledcAttachPin(pin_pwm_dir, mot_dir_chn); // canal 0 -> motor direita -> motor A
   ledcAttachPin(pin_pwm_esq, mot_esq_chn); // canal 1 -> motor esquerda -> motor B

}



void ler() {

  sen2E = analogRead(pin_sen_2E);
  sen1E = analogRead(pin_sen_1E);

  senCT = analogRead(pin_sen_CT);

  sen1D = analogRead(pin_sen_1D);
  sen2D = analogRead(pin_sen_2D);

  sen_NB = analogRead(pin_sen_NB);

}

void calc_erros ()
{
   sen2E = sen2E - branco_2E;
   sen1E = sen1E - branco_1E;

   senCT = preto_CT - senCT;

   sen1D = sen1D - branco_1D;
   sen2D = sen2D - branco_2D;

   // ERRO = REFERÊNCIA - SETPOINT
   // valor leitura branca <<<<<< valor leitura preta

   erro_neg_esq = ((senCT * 1) + (sen1E * 2) + (sen2E * 3))/6;
   erro_pos_dir = ((senCT * 1) + (sen1D * 2) + (sen2D * 3))/6;
}

boolean chk_dir (float erro_neg_esq, float erro_pos_dir)
{
   if (erro_neg_esq > erro_pos_dir)
   {
      erro_neg_esq = sign*erro_neg_esq; // sign para termo integral convergir pora zero

      bloco_p = Kp*erro_neg_esq;

      bloco_i += Ki*(erro_neg_esq)*delta*0.001;

      bloco_d = Kd*(erro_neg_esq - erro_ant_neg_esq)/(delta*0.001);

      u_diff = bloco_p + bloco_i + bloco_d;
      erro_ant_neg_esq = erro_neg_esq;


      Dir = false; // virar a esquerda
   }
   else if (erro_neg_esq < erro_pos_dir)
   {
      bloco_p = Kp*erro_pos_dir;

      bloco_i += Ki*(erro_pos_dir)*delta*0.001; // 0.001 para passar delta para milissegundos

      bloco_d = Kd*(erro_pos_dir - erro_ant_pos_dir)/(delta*0.001);

      u_diff = bloco_p + bloco_i + bloco_d;
      erro_ant_pos_dir = erro_pos_dir;

      Dir = true; // virar a direita
   }

   return Dir;
}

void set_motores(boolean Dir)
{
   u_diff = abs(u_diff);
   u_diff = (u_diff)/2;


   if (Dir == false)
   {
      pwm_val_dir = pwm_val_dir + u_diff;
      pwm_val_esq = pwm_val_esq - u_diff;
      pwm_val_dir = int(pwm_val_dir);
      pwm_val_esq = int(pwm_val_esq);
   }
   else if (Dir == true)
   {
      pwm_val_dir = pwm_val_dir - u_diff;
      pwm_val_esq = pwm_val_esq + u_diff;
      pwm_val_dir = int(pwm_val_dir);
      pwm_val_esq = int(pwm_val_esq);
   }

   if (pwm_val_dir > 4000) {pwm_val_dir = pwm_max_val;}
   if (pwm_val_esq > 4000) {pwm_val_esq = pwm_max_val;}
   if (pwm_val_dir < 0) {pwm_val_dir = 0;}
   if (pwm_val_esq < 0) {pwm_val_esq = 0;}

// APLICAR TENSÃO NO MOTOR DIREITO

   digitalWrite(mot_dir_fr, HIGH);
   digitalWrite(mot_dir_re, LOW);

   ledcWrite(mot_dir_chn, pwm_val_dir);  // SETA O MOTOR DA DIREITA

   //delayMicroseconds(5);

// APLICAR TENSÃO NO MOTOR ESQUERDO

   digitalWrite(mot_esq_fr, HIGH);
   digitalWrite(mot_esq_re, LOW);

   ledcWrite(mot_esq_chn, pwm_val_esq);  // SETA O MOTOR DA ESQUERDA

   //delayMicroseconds(5);
   //delay(10);
}


// ENVIAR

void procSenConf () {

   String message;
   const size_t capacity = JSON_OBJECT_SIZE(4) + 130;
   DynamicJsonDocument doc(capacity);

   doc["Rede"] = ssid;
   //doc["IP"] = WiFi.localIP().toString();
   doc["BT"] = endBT;
   serializeJson(doc, message);

   //Serial.print("Json enviado: "); Serial.println(message);
   webSocketServidor.sendData(message);

   // message = {"rede": "nomeDaRede", "ip": "123.456.789.000", "BT": "nomeUUID nomeUUID nomeUUID"}
}

void procSenCons() {

   String message;
   const size_t capacity = JSON_OBJECT_SIZE(3) + 80;
   DynamicJsonDocument doc(capacity);

   doc["kp"] = Kp;
   doc["ki"] = Ki;
   doc["kd"] = Kd;
   serializeJson(doc, message);

   //Serial.print("Json enviado: "); Serial.println(message);
   webSocketServidor.sendData(message);

   // message = {"kp": 4000.4, "ki": 5000.5, "kd": 6000.6}
}

void procSenSigns() {

   String message;
   const size_t capacity = JSON_OBJECT_SIZE(3) + 70;
   DynamicJsonDocument doc(capacity);

   if (erro_neg_esq >= erro_pos_dir) {

      doc["erro"] = erro_neg_esq;

   }
   else {

      doc["erro"] = erro_pos_dir;

   }

   doc["diff"] = u_diff;
   serializeJson(doc, message);

   //Serial.print("Json enviado: "); Serial.println(message);
   webSocketServidor.sendData(message);

   // message = {"erro": 2000.2, "diff": 3000.3}
}

void procSenBat() {

   String message;
   const size_t capacity = JSON_OBJECT_SIZE(1) + 30;
   DynamicJsonDocument doc(capacity);

   doc["bat"] = sen_NB;
   serializeJson(doc, message);

   //Serial.print("Json enviado: "); Serial.println(message);
   webSocketServidor.sendData(message);

   // message = {"bat": 9999} - ENVIAR DE 5 EM 5 SEGUNDOS
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

   Kp = doc["kp"];
   Ki = doc["ki"];
   Kd = doc["kd"];

   //Serial.println(Kp);
   //Serial.println(Ki);
   //Serial.println(Kd);

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

   ordem = doc["ordem"].as<String>();

   setOrd(ordem);
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

   pwm_max_val = doc["vmax"];

   //Serial.println("Vmax: ");
   //Serial.println(pwm_max_val);

   // message = {"vmax": 999}
}


// CONEXÃO

bool chkConnection() {

   if (!openCon) {
      cliente = servidor.available(); // se há clientes disponíveis ou não

      if (cliente) {
         //Serial.println("Novo cliente.");

         if (cliente.connected()) {
            //Serial.println("Cliente WiFi se conectou.");

            if (webSocketServidor.handshake(cliente)) {
               //Serial.println("Websocket estabelecido.");
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


// DADOS

void chkData () {

   String recData;

   if (cliente.available()) { // verifica se há dados chegando do cliente

      recData = webSocketServidor.getData();
      delay(100); // delay NECESSÁRIO para receber dados corretamente

      if (recData.length() > 0) {

         //Serial.print("Chegou mensagem: ");
         //Serial.println(recData);

         if (recData == "rec") {

            //Serial.print("A mensagem é de tráfego: ");
            //Serial.println(recData);

            recData = webSocketServidor.getData();
            delay(100);

            if (recData.length() > 0) {
               //Serial.print("Chegaram metadados: ");
               //Serial.println(recData);

               if (recData == "cons") {
                  recData = webSocketServidor.getData(); // getData usa String args
                  delay(100);

                  if (recData.length() > 0) {
                     //Serial.print("Chegaram dados Json: ");
                     //Serial.println(recData);

                     procRecCons(recData);
                  }
               }
               else if (recData == "ord") {
                  recData = webSocketServidor.getData();
                  delay(100);

                  if (recData.length() > 0) {
                     //Serial.print("Chegaram dados: ");
                     //Serial.println(recData);

                     procRecOrd(recData);
                  }
               }
               else if (recData == "vel") {
                  recData = webSocketServidor.getData();
                  delay(100);

                  if (recData.length() > 0) {
                     //Serial.print("Chegaram dados: ");
                     //Serial.println(recData);

                     procRecVel(recData);
                  }
               }
            }
         }
         else if (recData == "send") {
            //Serial.print("A mensagem é de tráfego: ");
            //Serial.println(recData);

            recData = webSocketServidor.getData();
            delay(100);

            if (recData.length() > 0) {
               //Serial.print("Chegaram metadados: ");
               //Serial.println(recData);

               if (recData == "cons") {

                  procSenCons();
               }
               else if (recData == "conf") {

                  procSenConf();
               }
               else if (recData == "signs") {

                  procSenSigns();
               }
               else if (recData == "bat") {

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


void f5_param_disp() {

   bot_1 = digitalRead(ch1);
   bot_2 = digitalRead(ch2);
   bot_3 = digitalRead(ch3);


   if (!troca_index) {

      switch (menu_index) {


         case 1: // TELA GERAL

            lcd.setCursor(0, 0);
            lcd.print("BAT ");
            lcd.print(float((100*sen_NB)/4095), 2);
            lcd.print("% ");
            lcd.print(float((3.3*sen_NB)/4095), 2);
            lcd.print("V");

            lcd.setCursor(0, 1);
            lcd.print("WIFI ");

            if (WiFi.status() != WL_CONNECTED) {

               lcd.print("Desconect");
               WiFi.begin(ssid, senha);

            }
            else if (WiFi.status() == WL_CONNECTED) {

               lcd.print(ssid);

            }

            break;


         case 2: // TELA CONFS

            lcd.setCursor(0, 0);
            lcd.print("IP ");

            if (WiFi.status() != WL_CONNECTED) {

               WiFi.begin(ssid, senha);

               while (WiFi.status() != WL_CONNECTED) {

                  Serial.println(".");
                  delay(500);

               }

            }
            else if (WiFi.status() == WL_CONNECTED) {

               lcd.setCursor(3, 0);
               lcd.print(WiFi.localIP().toString());

            }

            lcd.setCursor(0, 1);
            lcd.print("BT ");
            lcd.print(endBT);

            break;


         case 3: // CHECA CLIENTE WIFI E CLIENTE WEBSOCKET

            lcd.setCursor(0, 0);
            lcd.print("WIFI CLNT: ");

            if (cliente) {

               lcd.print("ON ");

            }
            else {

               lcd.print("OFF");

            }

            lcd.setCursor(0, 1);
            lcd.print("WEBS CLNT: ");

            if (openCon) {

               lcd.print("ON ");

            }
            else {

               lcd.print("OFF");

            }


            break;


      }
   }
   else if (troca_index) {


      bot_1 = digitalRead(ch1);
      bot_2 = digitalRead(ch2);
      bot_3 = digitalRead(ch3);

      // TESTAR LEITURA AQUI

      switch (const_index) {

         case 1: // TELA KP

            lcd.setCursor(0, 0);
            lcd.print("Kp ");
            lcd.setCursor(0, 1);
            lcd.print(Kp, 8);

            // diminui

            while (bot_1 == 1) {

               bot_1 = digitalRead(ch1);

               Kp -= 0.00001;

               if (Kp < 0) {
                  Kp = 0.00001;
               }

               lcd.setCursor(0, 1);
               lcd.print(Kp, 8);

            }

            // aumenta

            while (bot_2 == 1) {

               bot_2 = digitalRead(ch2);

               Kp += 0.00001;

               if (Kp >= 1000000000) {
                  Kp = 0.00001;
               }

               lcd.setCursor(0, 1);
               //lcd.print("Kp ");
               //lcd.setCursor(0, 3);
               lcd.print(Kp, 8);

            }

            break;


         case 2: // TELA KI

            lcd.setCursor(0, 0);
            lcd.print("Ki ");
            lcd.setCursor(0, 1);
            lcd.print(Ki, 3);

            while (bot_1 == 1) {

               bot_1 = digitalRead(ch1);

               Ki -= 0.001;

               if (Ki < 0) {
                  Ki = 0.001;
               }

               lcd.setCursor(0, 1);
               lcd.print(Ki, 3);

            }


            while (bot_2 == 1) {

               bot_2 = digitalRead(ch2);

               Ki += 0.001;

               if (Ki >= 1000000000) {
                  Ki = 0.001;
               }

               lcd.setCursor(0, 1);
               lcd.print(Ki, 3);

            }

            break;


         case 3: //TELA KD

            lcd.setCursor(0, 0);
            lcd.print("Kd ");
            lcd.setCursor(0, 1);
            lcd.print(Kd, 3);

            while (bot_1 == 1) {

               bot_1 = digitalRead(ch1);

               Kd -= 0.001;

               if (Kd < 0) {
                  Kd = 0.001;
               }

               lcd.setCursor(0, 1);
               lcd.print(Kd, 3);

            }


            while (bot_2 == 1) {

               bot_2 = digitalRead(ch2);

               Kd += 0.001;

               if (Kd >= 1000000000) {
                  Kd = 0.001;
               }

               lcd.setCursor(0, 1);
               lcd.print(Kd, 3);

            }

            break;

      }

   }


   //*****//
   if (bot_1 == 1 && bot_3 == 1 && !stat_bot_1_e_3) {

      trocou = false;

      if (!troca_index) {

         troca_index = true;
         lcd.clear();
         trocou = true;

      }

      if (!trocou) {

         troca_index = false;
         lcd.clear();

      }

      stat_bot_1_e_3 = true;

   }
   else if (bot_1 == 0 && bot_3 == 0 && stat_bot_1_e_3) {

      stat_bot_1_e_3 = false;

   }

   // troca a tela

   if (bot_3 == 1 && !stat_bot_3) {

      stat_bot_3 = true;

      if (troca_index) {

         const_index++;
         lcd.clear();

      }
      else if (!troca_index) {

         menu_index++;
         lcd.clear();

      }


      if (const_index == 4) {
         const_index = 1;
      }

      if (menu_index == 4) {
         menu_index = 1;
      }

   }
   else if (bot_3 == 0 && stat_bot_3) {

      stat_bot_3 = false;

   }

}

void loop () {

   ler();

   if (chkConnection()) {
      chkData();
   }

   f5_param_disp();

   delta = millis() - chk_point;
   chk_point = millis();

   calc_erros();

   //print_sens();
   //print_cons();
   //print_pid();
   //print_signs();

   if (anda) {

      set_motores(chk_dir(erro_neg_esq, erro_pos_dir));

   }

}
