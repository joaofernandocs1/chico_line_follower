/*#include <Arduino.h>
#include <LiquidCrystal.h>

// PID CHIQUINHO LIFESTYLE //


// --- PINAGENS ---

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

// ALIMENTAÇÃO

// #define Vsens 12 // alimentação sensores // alimentar os sensores com fonte externa (corrigir esse erro!!)

// PINAGEM

#define pin_sen_2E 36 // sensor -2
#define pin_sen_1E 39 // sensor -1

#define pin_sen_CT 34 // sensor 0

#define pin_sen_1D 35 // sensor 1
#define pin_sen_2D 32 // sensor 2

// --- CALIBRAGENS (setpoints) ---

#define preto_2E 3589 // calibragens das leituras dos sensores para preto e branco (atualizada 02/02/20)
#define preto_1E 3661

#define branco_2E 498
#define branco_1E 452

#define branco_CT 81
#define preto_CT 3773

#define branco_1D 537
#define branco_2D 232 // 232 forçar simetria de calibragem com 2E para erro de um lado não tender a ser maior que o outro devido a sensibilidade

#define preto_1D 3639
#define preto_2D 3670

// --- VARIÁVEIS ---

// LEITURA DO SENSOR (Vista superior)

int sen2E = 0; // sensor 2E  ****
int sen1E = 0; // sensor 1E  ****

int senCT = 0; // sensor CT  ---- PISTA

int sen1D = 0; // sensor 1D  ****
int sen2D = 0; // sensor 2D  ****

// PID

//float Kp = 0.2; // não usar "define", pois Kp e Kd tem de ser float no código
//float Ki = 0.000001;
//float Kd = 30;

float Kp = 0.001; // 0,0007// 0.00823 estoura
float Ki = 0.001; // 2,6847
float Kd = 0.001; // 0,8887

float bloco_p;
float bloco_i;
float bloco_d;

unsigned long chk_point = 0;
unsigned long delta; // testar float e double

boolean Dir;

// DISPLAY

LiquidCrystal lcd(19, 18, 5, 17, 16, 4);

// ERROS

float erro_pos_dir;
float erro_neg_esq;

float erro_ant_neg_esq = 0;
float erro_ant_pos_dir = 0;

int sign = -1;

// VELOCIDADE

int pwm_val_dir = 100;
int pwm_val_esq = 100;

int pwm_max_val = 3700;

float u_diff;

// DISPLAY

int bot_1;
int bot_2;
int bot_3;

int const_index = 1;

bool stat_bot_3 = false;

// False -> PRONTO/desativado
// True -> OCUPADO/ativado

const int ch1 = 12;
const int ch2 = 13;
const int ch3 = 21;

void setup() {

  //Serial.begin(38400);

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

  //pinMode(Vsens, OUTPUT);

  lcd.begin(16, 2); // (colunas, linhas)
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Iniciando...");
  delay(1000);
  lcd.clear();

}

void print_sens() {

   Serial.print(sen2E); Serial.print("  ");
   Serial.print(sen1E); Serial.print("  ");
   Serial.print(senCT); Serial.print("  ");
   Serial.print(sen1D); Serial.print("  ");
   Serial.print(sen2D); Serial.print("  ");

   // olhando para os valores na serial o robô estará seguindo na direção da parte de cima da tela

}

void print_cons() {

   Serial.print("Kp: ");
   Serial.print(Kp, 5);
   Serial.print("  ");

   Serial.print("Ki:  ");
   Serial.print(Ki, 3);
   Serial.print("  ");

   Serial.print("Kd: ");
   Serial.print(Kd, 3);
   Serial.print("  ");

}

void print_pid() {

   Serial.print("P: ");
   Serial.print(bloco_p);
   Serial.print("  ");

   Serial.print("I: ");
   Serial.print(bloco_i);
   Serial.print("  ");

   Serial.print("D: ");
   Serial.print(bloco_d);
   Serial.print("  ");

   Serial.print("[erro_n_esq]: ");
   Serial.print(erro_neg_esq);
   Serial.print("  ");

   Serial.print("[erro_p_dir]: ");
   Serial.print(erro_pos_dir);
   Serial.print(" ");

   Serial.print("dt: ");
   Serial.print(delta);
   Serial.print(" ms ");

}

void print_signs() {

   Serial.print("[u_diff]: ");
   Serial.print(u_diff);
   Serial.print("  ");

   Serial.print("PWM esq: ");
   Serial.print(pwm_val_dir); // verificar se não está estourando sempre

   Serial.print("  ");

   Serial.print("PWM dir: ");
   Serial.println(pwm_val_esq); // verificar se não está estourando sempre

}

void f5_param_disp() {

   bot_1 = digitalRead(ch1);
   bot_2 = digitalRead(ch2);
   bot_3 = digitalRead(ch3);


   // aumenta
   switch (const_index) {
      case 1:

         lcd.setCursor(0, 0);
         lcd.print("Kp ");//lcd.print(pwm_val_esq); lcd.print(" "); lcd.print(pwm_val_dir);
         lcd.setCursor(0, 1);
         lcd.print(Kp, 5);

         while (bot_2 == 1) {

            bot_2 = digitalRead(ch2);

            Kp += 0.00001;

            if (Kp >= 1000000000) {
               Kp = 0.00001;
            }

            lcd.setCursor(0, 1);
            //lcd.print("Kp ");
            //lcd.setCursor(0, 3);
            lcd.print(Kp, 5);

         }

         break;

      case 2:

         lcd.setCursor(0, 0);
         lcd.print("Ki "); //lcd.print(pwm_val_esq); lcd.print(" "); lcd.print(pwm_val_dir);
         lcd.setCursor(0, 1);
         lcd.print(Ki, 3);

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

      case 3:

         lcd.setCursor(0, 0);
         lcd.print("Kd "); //lcd.print(pwm_val_esq); lcd.print(" "); lcd.print(pwm_val_dir);
         lcd.setCursor(0, 1);
         lcd.print(Kd, 3);

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

   // diminui
   switch (const_index) {
      case 1:

         lcd.setCursor(0, 0);
         lcd.print("Kp "); //lcd.print(pwm_val_esq); lcd.print(" "); lcd.print(pwm_val_dir);
         lcd.setCursor(0, 1);
         lcd.print(Kp, 8);

         while (bot_1 == 1) {

            bot_1 = digitalRead(ch1);

            Kp -= 0.000001;

            if (Kp < 0) {
               Kp = 0.00001;
            }

            lcd.setCursor(0, 1);
            lcd.print(Kp, 8);

         }

         break;

      case 2:

         lcd.setCursor(0, 0);
         lcd.print("Ki "); //lcd.print(pwm_val_esq); lcd.print(" "); lcd.print(pwm_val_dir);
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

         break;

      case 3:

         lcd.setCursor(0, 0);
         lcd.print("Kd "); //lcd.print(pwm_val_esq); lcd.print(" "); lcd.print(pwm_val_dir);
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

         break;

   }

   // troca a constante

   if (bot_3 == 1 && stat_bot_3 == false) {

      stat_bot_3 = true;
      const_index++;

      if (const_index == 4) {
         const_index = 1;
      }

   }
   else if (bot_3 == 0 && stat_bot_3 == true) {
      stat_bot_3 = false;
   }

}

void f5_param_serial() {

   bot_1 = digitalRead(ch1);
   bot_2 = digitalRead(ch2);
   bot_3 = digitalRead(ch3);

   // aumenta
   switch (const_index) {
      case 1:

         if (bot_2 == 1) {

            Kp += 0.00001;

            if (Kp >= 1000000000) {
               Kp = 0.00001;
            }

         }

         break;

      case 2:

         if (bot_2 == 1) {

            Ki += 0.001;

            if (Ki >= 1000000000) {
               Ki = 0.001;
            }

         }

         break;

      case 3:

         if (bot_2 == 1) {

            Kd += 0.001;

            if (Kd >= 1000000000) {
               Kd = 0.001;
            }

         }

         break;

   }

   // diminui
   switch (const_index) {
      case 1:

         if (bot_1 == 1) {

            Kp -= 0.00001;

            if (Kp < 0) {
               Kp = 0.00001;
            }

         }

         break;

      case 2:

         if (bot_1 == 1) {

            Ki -= 0.001;

            if (Ki < 0) {
               Ki = 0.001;
            }

         }

         break;

      case 3:

         if (bot_1 == 1) {

            Kd -= 0.001;

            if (Kd < 0) {
               Kd = 0.001;
            }

         }

         break;

   }

   // troca a constante

   if (bot_3 == 1 && stat_bot_3 == false) {

      stat_bot_3 = true;
      const_index++;

      if (const_index == 4) {
         const_index = 1;
      }

   }
   else if (bot_3 == 0 && stat_bot_3 == true) {
      stat_bot_3 = false;
   }


}

void ler() {

  //digitalWrite(Vsens, HIGH); // alimentar os sensores com fonte externa e usar FET para chavear (corrigir esse erro!!)
  //delayMicroseconds(30); // inércia do circuito conforme datasheet

  // NÃO É POSSÍVEL implementar a alimentação intermitente porque não tem um relé pra ligar e desligar a alimentação

  sen2E = analogRead(pin_sen_2E);
  sen1E = analogRead(pin_sen_1E);

  senCT = analogRead(pin_sen_CT);

  sen1D = analogRead(pin_sen_1D);
  sen2D = analogRead(pin_sen_2D);

  // digitalWrite(Vsens, LOW);

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

   erro_neg_esq = ((senCT * 1) + (sen1E * 2) + (sen2E * 3))/6; // verificar se o erro não está estourando sempre
   erro_pos_dir = ((senCT * 1) + (sen1D * 2) + (sen2D * 3))/6; // pode implicar em uma correção sempre alta e causar instabilidade
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

   if (pwm_val_dir > 4000) {pwm_val_dir = pwm_max_val;} // prevent the motor from going beyond max speed
   if (pwm_val_esq > 4000) {pwm_val_esq = pwm_max_val;} // prevent the motor from going beyond max speed
   if (pwm_val_dir < 0) {pwm_val_dir = 0;} // keep the motor speed positive
   if (pwm_val_esq < 0) {pwm_val_esq = 0;} // keep the motor speed positive

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

void loop()
{
   f5_param_disp();
   //f5_param_serial();

   ler();

   delta = millis() - chk_point;
   chk_point = millis();

   calc_erros();

   //print_sens();
   //print_cons();
   //print_pid();
   //print_signs();

   set_motores(chk_dir(erro_neg_esq, erro_pos_dir));
}*/
