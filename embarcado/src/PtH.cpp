/*#include <Arduino.h>

const int in1 = 25;
const int in2 = 26;

const int in3 = 27;
const int in4 = 14;

const int ena_pin = 22; // direita
const int enb_pin = 23; // esquerda

// FREQUÊNCIA

const int en_freq = 1000;

// CANAL

const int ena_chn = 0;
const int enb_chn = 1;

// RESOLUÇÃO

const int en_res = 8;

// VALOR PWM

int duty_ena_cyc = 110;
int duty_enb_cyc = 110;

void setup() {

  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  ledcSetup(ena_chn, en_freq, en_res); // EN A
  ledcSetup(enb_chn, en_freq, en_res); // EN B

  ledcAttachPin(ena_pin, ena_chn); // EN A
  ledcAttachPin(enb_pin, enb_chn); // EN B
}

void loop() {

  // MOTOR A DA DIREITA

  // sentido inicial (FRENTE)

  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);

  ledcWrite(ena_chn, duty_ena_cyc);

  // muda o sentido (RÉ)

  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);

  ledcWrite(ena_chn, duty_ena_cyc);

  // MOTOR B DA ESQUERDA

  // sentido inicial (RÉ)

  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);

  ledcWrite(enb_chn, duty_enb_cyc);

  // muda o sentido (FRENTE)

  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);

  ledcWrite(enb_chn, duty_enb_cyc);

}*/
