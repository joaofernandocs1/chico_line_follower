/*

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


}*/
