// Keypad module connections
char  keypadPort at PORTD;
// End Keypad module connections

// LCD module connections
sbit LCD_RS at RC1_bit;
sbit LCD_EN at RC2_bit;
sbit LCD_D0 at RA0_bit;
sbit LCD_D1 at RA2_bit;
sbit LCD_D2 at RB2_bit;
sbit LCD_D3 at RB3_bit;
sbit LCD_D4 at RB4_bit;
sbit LCD_D5 at RB5_bit;
sbit LCD_D6 at RB6_bit;
sbit LCD_D7 at RB7_bit;
sbit LCD_RS_Direction at TRISC1_bit;
sbit LCD_EN_Direction at TRISC2_bit;
sbit LCD_D0_Direction at TRISA0_bit;
sbit LCD_D1_Direction at TRISA2_bit;
sbit LCD_D2_Direction at TRISB2_bit;
sbit LCD_D4_Direction at TRISB4_bit;
sbit LCD_D5_Direction at TRISB5_bit;
sbit LCD_D6_Direction at TRISB6_bit;
sbit LCD_D7_Direction at TRISB7_bit;
// End LCD module connections

#include <stdint.h>
unsigned short v0, v1, dat[2];
//int delta = 0x9E3779B9;
unsigned short delta = 0x9E3779B9;
unsigned short key[4] = {0x00112233,0x44556677,0x8899aabb,0xccddeeff};
unsigned short send[30], rec[30], word_e[30], word_s[30], kp;
short rows = 1,rowm = 1, col = 3;
int i, j, length, sum, num_rounds =32;

void main() {
  TRISC.B4 = 1;
  TRISC.B5 = 1;
  TRISC.B0 = 1;
  Keypad_Init();                           // Initialize Keypad
  ADRESL  = 0;                              // Configure AN pins as digital I/O
  ADRESH = 0;
  ADCON1=0;
  ADCON2=0;
  Lcd_Init(); // Initialize LCD
  Lcd_Cmd(_LCD_CLEAR); // Clear display
  Lcd_Cmd(_LCD_CURSOR_OFF); // Cursor off
  Lcd_Out(1, 1, "Welcome to Crypta!   ");

  do {
   while(PORTC.B5 == 0){
     Lcd_Out(2, 1, "Slave               ");
      UART1_Init(9600);
      delay_ms(100);
      length = UART1_Read();
      
      if(length < 20){
           Lcd_Out(2, 1, "Slave / receive     ");
           for(i=0;i<(2*length);i++){
                delay_ms(100);
                rec[i] = UART1_Read();
              }

       // Descryption Algorithm
       for(j=0;j<length;j++){
            Lcd_Out(2, 1, "Slave / decode...   ");
            dat[0] = rec[2*j];
            dat[1] = rec[2*j+1];
            v0=dat[0], v1=dat[1], sum=delta*num_rounds;
            for (i=0; i < num_rounds; i++) {
                v1 -= (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum>>11) & 3]);
                sum -= delta;
                v0 -= (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
            }
            dat[0]=v0; dat[1]=v1;
            word_s[j] = dat[0]*10 + dat[1];
        }
        // End of Decryption Algorithm

        // Display decrypted text
        for(i=0;i<length;i++){
             Lcd_Chr(col, rows, word_s[i]);

             // Organize Display
             rows += 1;
             if (rows > 20) {
                 col++;
               }
             if(rows > 20 & col == 4){
                 Lcd_Out(3, 1, "                    ");
                 Lcd_Out(4, 1, "                    ");
               }
           }
        }
      }

    while(PORTC.B5 == 1){
         Lcd_Out(2, 1, "Master    ");
         for(i=0;i<20;i++){
             kp = 0;

             Scan:
             Lcd_Out(2, 1, "Master / keyboard...");

             do {
                  kp = Keypad_Key_Click();
                       if(PORTC.B0 == 1){
                             goto ENC;
                         }
               } while (!kp);

               if (PORTC.B4 == 1){
                    switch (kp) {
                           case  1: kp = 65; break; // A
                           case  2: kp = 66; break; // B
                            case  3: kp = 67; break; // C
                            case  4: kp = 68; break; // D
                            case  5: kp = 69; break; // E
                            case  6: kp = 70; break; // F
                            case  7: kp = 71; break; // G
                            case  8: kp = 72; break; // H
                            case  9: kp = 73; break; // E
                            case 10: kp = 74; break; // J
                            case 11: kp = 75; break; // K
                            case 12: kp = 76; break; // L
                            case 13: kp = 77; break; // M
                            case 14: kp = 78; break; // N
                            case 15: kp = 79; break; // O
                            case 16: kp = 80; break; // P
                          }
                 }

                 else if (PORTC.B4 == 0) {
                      switch (kp) {
                            case  1: kp = 81; break; // Q
                            case  2: kp = 82; break; // R
                            case  3: kp = 83; break; // S
                            case  4: kp = 84; break; // T
                            case  5: kp = 85; break; // U
                            case  6: kp = 86; break; // V
                            case  7: kp = 87; break; // W
                            case  8: kp = 88; break; // X
                            case  9: kp = 89; break; // Y
                            case 10: kp = 90; break; // Z
                            case 11: kp = 33; break; // !
                            case 12: kp = 63; break; // ?
                            case 13: kp = 39; break; // '
                            case 14: kp = 32; break; // SPC
                            case 15: {rowm-=1;i--; kp = 0; break;} // DEL
                            case 16: kp = 35; break; // #
                            }
                      if (kp == 0) {
                             Lcd_Chr(col, rowm, kp);
                              goto Scan;
                        }
                      }

                  Lcd_Out(2, 1, "Master / displaying.");

                  // Print key ASCII value on LCD
                  word_e[i] = kp;
                  Lcd_Chr(col, rowm, word_e[i]);
                  length++;
                  rowm += 1;
                  if (rowm > 20) {
                     col++;
                    }
                  if(rowm > 20 & col == 4){
                     Lcd_Out(3, 1, "                    ");
                     Lcd_Out(4, 1, "                    ");
                    }
           }

           ENC:
           Lcd_Out(2, 1, "Master / encoding...");
           for(j=0;j<length;j++){
                // Encryption Algorithm
                dat[0]= word_e[j]/10 ;
                dat[1]= word_e[j]%10;
                v0=dat[0], v1=dat[1], sum=0;
                for (i=0; i < num_rounds; i++) {
                     v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + key[sum & 3]);
                     sum += delta;
                      v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + key[(sum >> 11) & 3]);
                   }
                dat[0]=v0; dat[1]=v1;
                send[2*j] = dat[0];
                send[2*j+1] = dat[1];
                // End of encryption algorithm
             }

               // Sending data using UART
                Lcd_Out(2, 1, "Master / sending... ");
                UART1_Init(9600);
                Delay_ms(100);
                UART1_Write(length);
                for(i=0;i<length;i++){
                     UART1_Write(send[2*i]);
                     Delay_ms(100);
                     UART1_Write(send[2*i+1]);
                     Delay_ms(100);
                  }
                for(i=0;i<length;i++){
                     send[i] = ' ';
                  }
                length = 0;
       }
    } while (1);
}
