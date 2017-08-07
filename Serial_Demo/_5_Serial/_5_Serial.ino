
#include <Arduino.h>
#include "wiring_private.h"
/* Daisy chains 5 serials together so that the output of Serial1 is the input of Serial2.
 * Output of Serial2 is input of Serial3 and so on. The I/O value counts up from 0 to 255. 
 * 
 * MAX number of serials is only 5 as Sercom2 has no pins available given that MISO is used.
 * If the SD Card is involved, then Sercom4 is also unavailable, which leaves only 4 serials.
 */
//Uart SerialN (&sercomM, RX, TX, SERCOM_RX_PAD_P, UART_TX_PAD_Q);
//UART_TX_PAD cannot be 1 or 3
/*****************************************************************
 *  Sercom 0 : Mux
 *  
SERCOM  Arduino Pins  PAD
------------------------------------------------------------------
  0       0           3   PIO_SERCOM
  0       1           2   PIO_SERCOM
  0       3           1   PIO_SERCOM
  0       4 (CS)      0   PIO_SERCOM
  0       9           3   PIO_SERCOM
  0       21 (SCL)    2   PIO_SERCOM
  0       17 (A3)     0   PIO_SERCOM
  0       18 (A4)     1   PIO_SERCOM
 *******************************************************************/

Uart Serial1 (&sercom0, 0, 1, SERCOM_RX_PAD_3, UART_TX_PAD_2);
void SERCOM0_Handler()
{
  Serial1.IrqHandler();
}

/*****************************************************************
 *  Sercom 1 : Mux
 *  
SERCOM   Arduino Pins  PAD
------------------------------------------------------------------
  1       8           1   PIO_SERCOM
  1       10          2   PIO_SERCOM
  1       11          0   PIO_SERCOM
  1       12          3   PIO_SERCOM
  1       13          1   PIO_SERCOM
******************************************************************/

Uart Serial2 (&sercom1, 12, 10, SERCOM_RX_PAD_3, UART_TX_PAD_2);
void SERCOM1_Handler()
{
  Serial2.IrqHandler();
}

/*****************************************************************
 *  Sercom 3 : Mux
 *  
SERCOM  Arduino Pins  PAD
------------------------------------------------------------------
  3       6           2   PIO_SERCOM_ALT
  3       10          2   PIO_SERCOM_ALT
  3       11          0   PIO_SERCOM_ALT
  3       20(SDA)     0   PIO_SERCOM_ALT
  3       21(SCL)     0   PIO_SERCOM_ALT
*******************************************************************/

Uart Serial3 (&sercom3, 6, 11, SERCOM_RX_PAD_2, UART_TX_PAD_0);
void SERCOM3_Handler()
{
  Serial3.IrqHandler();
}
/*****************************************************************
 *  Sercom 4 : Mux
 *  
SERCOM  Arduino Pins  PAD
------------------------------------------------------------------
4       2             2   PIO_SERCOM_ALT
4       5             3   PIO_SERCOM_ALT
4       15 (A1)       0   PIO_SERCOM_ALT
4       16 (A2)       1   PIO_SERCOM_ALT
*******************************************************************/
Uart Serial4 (&sercom4, 16, 15, SERCOM_RX_PAD_1, UART_TX_PAD_0);
void SERCOM4_Handler()
{
  Serial4.IrqHandler();
}

/*****************************************************************
 *  Sercom 5 : Mux
 *  
SERCOM  Arduino Pins  PAD
------------------------------------------------------------------
5       6             2   PIO_SERCOM_ALT
5       7             3   PIO_SERCOM_ALT
5       19 (A5)       0   PIO_SERCOM_ALT
5       20 (SDA)      0   PIO_SERCOM_ALT
5       21 (SCL)      1   PIO_SERCOM_ALT
*******************************************************************/
Uart Serial5 (&sercom5, 21, 20, SERCOM_RX_PAD_1, UART_TX_PAD_0);
void SERCOM5_Handler()
{
  Serial5.IrqHandler();
}

/*****************************************************************
 *  Sercom 2 : Mux
 *  
SERCOM  Arduino Pins  PAD
------------------------------------------------------------------
2       0             3   PIO_SERCOM_ALT
2       2             2   PIO_SERCOM_ALT
2       3             1   PIO_SERCOM_ALT
2       4             0   PIO_SERCOM_ALT
2       5             3   PIO_SERCOM_ALT
2       22 (MISO)     0   PIO_SERCOM_ALT
  No pins available because MISO is used 
 */
/* 
Uart Serial6 (&sercom2, 5, 22, SERCOM_RX_PAD_3, UART_TX_PAD_0);
void SERCOM2_Handler()
{
  Serial6.IrqHandler();
}*/
 
uint8_t i = 0;
void setup() {
  Serial.begin(9600);
 
  Serial1.begin(9600);
  pinPeripheral(1, PIO_SERCOM); //TX
  pinPeripheral(0, PIO_SERCOM); //RX

  Serial2.begin(9600);
  pinPeripheral(10, PIO_SERCOM);//TX
  pinPeripheral(12, PIO_SERCOM);//RX

  Serial3.begin(9600);
  pinPeripheral(11, PIO_SERCOM_ALT);//TX
  pinPeripheral(6, PIO_SERCOM_ALT); //RX

  Serial4.begin(9600);
  pinPeripheral(15, PIO_SERCOM_ALT);//TX
  pinPeripheral(16, PIO_SERCOM_ALT); //RX

  Serial5.begin(9600);
  pinPeripheral(20, PIO_SERCOM_ALT);//TX
  pinPeripheral(21, PIO_SERCOM_ALT);//RX 
/*
  Serial6.begin(9600);
  pinPeripheral(5, PIO_SERCOM_ALT);
  pinPeripheral(22, PIO_SERCOM_ALT); 
*/
  delay(300);
  Serial1.write(i);
}
 
void loop() {
  // One to Two
  if (Serial1.available()) {
    i = Serial1.read();  
    Serial1.write(++i); 
    delay(100); 
  
    Serial.print("S1: ");
    Serial.println(i);
  }  

  // Two to Three
  if (Serial2.available()) {
    i = Serial2.read(); 
    Serial2.write(++i);
    delay(100); 
  
    Serial.print("S2: ");
    Serial.println(i);
  } 

  // Three to Four
  if (Serial3.available()) {
    i = Serial3.read(); 
    Serial3.write(++i);
    delay(100); 
   
    Serial.print("S3: ");
    Serial.println(i);
  } 
  // Four to Five
  if (Serial4.available()) {
    i = Serial4.read(); 
    Serial4.write(++i);
    delay(100); 
   
    Serial.print("S4: ");
    Serial.println(i);
  } 

  // Five to Six
  if (Serial5.available()) {
    i = Serial5.read(); 
    Serial5.write(++i);
    delay(100); 
   
    Serial.print("S5: ");
    Serial.println(i);
  } 
/*
  //Six to One
    if (Serial6.available()) {
    i = Serial6.read(); 
    Serial6.write(++i);
    delay(100); 
   
    Serial.print("S6: ");
    Serial.println(i);
  } */
}
