#include <Arduino.h>        // sercom
#include "wiring_private.h" // sercom
#include <SPI.h>            // SD
#include <SD.h>             // SD
#include <RTCZero.h>        // time
#include "SerialBuf.h"
#include "SSMProtocol.h"

#define SD_LED 8      // write SD
#define idle 15       // secs of idle before sleeping
#define timeStart 4   // starting bit of log time
#define timeEnd   17  // ending bit of log time
#define totalSensors 2 //total amount of Serials available for Sensor

//Serial Names
#define SeeStar Serial4
#define Sensor1 Serial1
#define Sensor2 Serial2
#define Sensor3 Serial3

//Enumeration for writeString
#define S1 1
#define S2 2
#define S3 3
#define SS 4

uint8_t chkSum = 0;
//------------------------------------------------------------------------------------------------------------
//global instantiationstake
RTCZero rtc;
String command = "";
const int chipSelect = 4;
uint8_t i = 0;
String sensor = "";
String Time = "";
String Date = "";
uint8_t seconds = 0;
uint8_t minutes = 32;
uint8_t hours = 2;
uint8_t day = 0;
uint8_t month = 0;
uint8_t year =  0;

//Serial instantiations---------------------------------------------------------------------------------------
SerialBuf buf;
ResponsePacket resp;
RequestPacket req;

//Sensor1: RX = 0, TX = 1
Uart Sensor1 (&sercom0, 0, 1, SERCOM_RX_PAD_3, UART_TX_PAD_2);
void SERCOM0_Handler()
{
  Sensor1.IrqHandler();
}

//Dummy Data Producer: RX = 12, TX = 10
Uart Sensor2 (&sercom1, 12, 10, SERCOM_RX_PAD_3, UART_TX_PAD_2);
void SERCOM1_Handler()
{
  Sensor2.IrqHandler();
}

//Sensor3: RX = 6, TX = 11
Uart Sensor3 (&sercom3, 6, 11, SERCOM_RX_PAD_2, UART_TX_PAD_0);
void SERCOM3_Handler()
{
  Sensor3.IrqHandler();
}

//SeeStar sensor
Uart SeeStar (&sercom5, 21, 20, SERCOM_RX_PAD_1, UART_TX_PAD_0);
void SERCOM5_Handler()
{
  Serial4.IrqHandler();
}
/*//container of sensors
Sercom allSensors[totalSensors] = {Sensor1, Sensor3}; //Sensor 2 is for Dummy Data for now

Sercom getSensor()
{
  return allSensors[0];
} */ //Work on making Config for Sercoms
/*------------------------------------------------------------------------------------------------------------
    Initializes SD config.
*/
void sdInit()
{
  //--------------SD -----------------------------------------------------------------------------------------
  Serial.print("Initializing SD card...");
  // see if the card is present and can be initialized:
  pinMode(SD_LED, OUTPUT);
  digitalWrite(SD_LED, LOW);

  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");

  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:
  if (dataFile) {
    digitalWrite(SD_LED, HIGH);
    Time =  "Date\t\t\t\t\t Time\t\t Sensor\t\t\t Reading";
    dataFile.println(Time);
    Serial.println(Time);
    dataFile.close();
    Time = "";
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
  digitalWrite(SD_LED, LOW);
}//sdInit()

/*------------------------------------------------------------------------------------------------------------
   This is used to reattach the Serials after being disconnected before sleeping.
*/
void reattach()
{
  Serial.begin(9600);

  Sensor1.begin(9600);
  pinPeripheral(1, PIO_SERCOM);
  pinPeripheral(0, PIO_SERCOM);

  Serial2.begin(9600);
  pinPeripheral(10, PIO_SERCOM);
  pinPeripheral(12, PIO_SERCOM);

  Sensor3.begin(9600);
  pinPeripheral(11, PIO_SERCOM_ALT);
  pinPeripheral(6, PIO_SERCOM_ALT);

  SeeStar.begin(9600);
  pinPeripheral(20, PIO_SERCOM_ALT);
  pinPeripheral(21, PIO_SERCOM_ALT);

  sdInit();
}//reattach()

/*------------------------------------------------------------------------------------------------------------
   Detaches 1 to "n" Serials
*/
void detachSerial(int n)
{
  switch (n) {
    case 1:
      Sensor1.end();
      break;
    case 2:
      Sensor1.end();
      Serial2.end();
      break;
    case 3:
      Sensor1.end();
      Serial2.end();
      Sensor3.end();
      break;
    case 4:
      Sensor1.end();
      Serial2.end();
      Sensor3.end();
      SeeStar.end();
      break;
    default:
      SeeStar.println("Error Detaching Serials\n\r");
  }
}//detachSerial()

/*------------------------------------------------------------------------------------------------------------
    Used to serially push out a String with Serial.write()
    serNum dictates which Serial writes
*/
void writeString(String stringData, int serNum) {
  switch (serNum) {
    case 0:
      Serial.println(stringData);
      break;
    case S1:
      for (int j = 0; j < stringData.length(); j++)
      {
        Sensor1.write(stringData[j]);   // Push each char 1 by 1 on each loop pass
      }
      Sensor1.write('\n');
      Sensor1.write('\r');
      break;
    case S2:
      for (int i = 0; i < stringData.length(); i++)
      {
        Serial2.write(stringData[i]);   // Push each char 1 by 1 on each loop pass
      }
      Serial2.write('\n');
      Serial2.write('\r');
      break;
    case S3:
      for (int i = 0; i < stringData.length(); i++)
      {
        Sensor3.write(stringData[i]);   // Push each char 1 by 1 on each loop pass
      }
      Sensor3.write('\n');
      Sensor3.write('\r');
      break;
    case SS:
      for (int i = 0; i < stringData.length(); i++)
      {
        SeeStar.write(stringData[i]);   // Push each char 1 by 1 on each loop pass
      }
      SeeStar.write('\n');
      SeeStar.write('\r');
      break;
    default:
      for (int j = 0; j < stringData.length(); j++)
      {
        Sensor1.write(stringData[j]);   // Push each char 1 by 1 on each loop pass
      }
  }
}//writeString()

/*------------------------------------------------------------------------------------------------------------
   Ensures that Feather sleeps after "n" secs. of being awake
*/
bool timeout(int n)
{
  return (rtc.getSeconds() > seconds + n);
}//timeout()

/*------------------------------------------------------------------------------------------------------------
   Verifies that the information sent by a user is received correctly by the Feather. Writes "ACK" if last byte 
   of buffer (AKA checksum byte) equals the modulos sum of each preceding char, and "NACK" if otherwise, unless 
   if the checksum byte is 'X', then treat this as a Don't Care statement and just parse and process the buffer 
   input.
*/
void checkSum(String bufIn)
{
  chkSum = 0;
  //https://henryforceblog.wordpress.com/2015/03/12/designing-a-communication-protocol-using-arduinos-serial-library/
  //Bufin == $1TSXXXXXXXXXXXXX0?  
  if(bufIn[bufIn.length() - 3] != 'X') {// Added 2 Char. Debug
    for(int j = 0; j < 16; j++) { //for(int j = timeStart; j < timeEnd; j++) {
      chkSum += bufIn[j];
    }
    chkSum = -chkSum;
    chkSum = chkSum & 0xFF;
    SeeStar.println("");
    
    if(chkSum == bufIn[bufIn.length() -3])
      writeString("ACK",SS);
    else
      writeString("NACK",SS);
  } else {
     SeeStar.println("");
     writeString("ACK",SS);
  }
}


/*------------------------------------------------------------------------------------------------------------
   Delineates whether to Take Sample, Return Log, or Send Error Message
*/
void handleReq(int processOut)
{
  switch(processOut) {
    case 0:
      writeString("Invalid Command",SS);
      break;
    case 1: 
      takeSample();
      break;
    case 2: 
      returnLog();
      break;
    default:
      writeString("Invalid Command",SS);
      break;
  }
}
/*------------------------------------------------------------------------------------------------------------
   An initial signal from the keyboard wakes up the Feather. The subsequent typed keys are then the received
   commands
*/
int awaken()
{
  //Produce Dummy Data
  Serial2.write(i++);
  seconds = rtc.getSeconds();
  writeString("Awake", SS);
   
  while (!SeeStar.available()) {
     if (timeout(idle)) {
       writeString("Sleep", SS); 
       delay(100); //gives time for carriage return and newline
       reset(&buf);
       sdWrite("",""); //Need to Debug; Doesn't really sleep unless this command is called :(
       return 0;
     }
   }

  while(!isFull(&buf)) { 
   if (timeout(idle)) { 
       writeString("Sleep", SS);
       delay(100); //gives time for carriage return and newline
       reset(&buf);
       sdWrite("",""); //Need to Debug; 
       return 1;
    } 
   if (SeeStar.available()) {
     // Read serial data from CM
     char rx = SeeStar.read();
     
     // If '$' detected, append to buffer.
     if (buf.state == ST_SERBUF_ENABLED) {
       appendChar(&buf, rx);
       SeeStar.write(rx);
       if(rx == '\r') {//Note: TS will still execute if only $1TS was typed
        writeString("\n\r",SS);
        break;
        }
     }
     // Detect '$' to start buffering serial input
     else if (rx == '$') {
       appendChar(&buf, rx);
       buf.state = ST_SERBUF_ENABLED;
       SeeStar.write(rx);
     }
   }

  }
  checkSum(buf.string);
  decodeRequest(buf.string, &req);
  handleReq(processRequest(&req, &resp));
  reset(&buf); 
  sdWrite("",""); //Need to Debug; 
  writeString("Sleep", SS);
  delay(100); //gives time for carriage return and newline
  return 2;
}//awaken()



  /*------------------------------------------------------------------------------------------------------------
   * Takes sample from all sensors
  */
  void takeSample()
  {
   //  writeString("$1RD\r",SS); // call sensor to thermistor
     sensor = "S1";
     sensor += "\t\t\t\t" + String(Sensor1.read());
     sdWrite(String(buf.string), sensor);

  //   writeString("$1RD\r",SS); // call sensor to thermistor
     sensor = "S3";
     sensor += "\t\t\t\t" + String(Sensor3.read());
     sdWrite(String(buf.string), sensor);
  }//takeSample()
  
  /*------------------------------------------------------------------------------------------------------------
      Reads and sends the last log(s) to the user. Called by parseCommand
  */
  void returnLog()
  {
    File  dataFile = SD.open("datalog.txt");
    if (dataFile) {
      SeeStar.println("datalog.txt:");
      // read from the file and overwrite command until contains the last log(s)
      while (dataFile.available()) {
        command = dataFile.readString();
      }
      command =  command.substring(command.length() - 30, command.length());
      writeString(command, SS);
      dataFile.close();
    } else {
      // if the file didn't open, print an error:
      SeeStar.println("error opening datalog.txt");
    }
  }
   /*------------------------------------------------------------------------------------------------------------
     Updates date at midnight. Only works if the time rolls over to midnight. Called by sdWrite();
  */
  void refreshDate()
  {
    if (rtc.getHours() == 0) { // midnight, refresh date
      year = rtc.getYear();
      month = rtc.getMonth();
      day = rtc.getMonth();
    }
  }//refreshDate
  
  /*------------------------------------------------------------------------------------------------------------
      Rewrites time to for readability
  */
  void timeFormat(String logTime)
  {
    //022446
    Date = logTime.substring(2,4) + "/" + logTime.substring(4,6) + "/" + logTime.substring(0,2) + "\t\t";
    Time = logTime.substring(6,8) + ":" + logTime.substring(8,10) + ":" + logTime.substring(10,12) + "\t\t"; 
  }
  
  /*------------------------------------------------------------------------------------------------------------
      Records specific sensor data onto SD Card. Calls refreshDate()
  */
  void sdWrite(String timeLog, String sense)
  {
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
      refreshDate();
      // if the file is available, write to it:
      if (dataFile) {
        digitalWrite(SD_LED, HIGH);
      //  Time =  String(month) + "/" + String(day) + "/" + String(year) + "\t\t\t\t" +
        //        String(hours) + ":" + String(minutes) + ":" + String(seconds) + "\t\t\t"; 
        timeFormat(timeLog.substring(timeStart,timeEnd-1));

        if(Date.length() > 6) { //Need to debug. If/else statement is used because of sleep power problem
          dataFile.print(Date); 
          dataFile.print(Time);
          
          sensor = sense;
          dataFile.println(sensor);
        } else {
          dataFile.print(" "); //Takes up space :( 
        }
      //   SeeStar.println(sensor);
        dataFile.close();
     }
      // if the file isn't open, pop up an error:
      else {
        SeeStar.println("error opening datalog.txt");
      }
      //indicator of writes
      digitalWrite(SD_LED, LOW);
      delay(700);
   //   SeeStar.println("Done writing\r");
  }//sdWrite(String)

 

  /*------------------------------------------------------------------------------------------------------------
      Blinks RED LED when Feather is Awake. Used for debugging purposes. Should be removed during deployment to
      minimize consumption.
  */
  void lightON()
  {
    digitalWrite(13, HIGH);   // turn the LED on
    delay(100);
    digitalWrite(13, LOW);    // turn the LED off
    delay(100);
    digitalWrite(13, HIGH);   // turn the LED on
    delay(100);
    digitalWrite(13, LOW);    // turn the LED off
  }//lightON()

  //---------------------------------------------------------------------------------------------
  void setup() {
    reattach();
    rtc.begin();
    rtc.setTime(hours, minutes, seconds);

    //-------------Time Settings---------------------------------------------------------------
    // get current date
    year = rtc.getYear();
    month = rtc.getMonth();
    day = rtc.getDay();

    //-------------Buffer Settings--------------------------------------------------------------
    buf = newSerialBuf();
    reset(&resp);
    reset(&req);

    //Dummy Data
    Serial2.write(i);
  }//setup()

  //---------------------------------------------------------------------------------------------
  void loop()
  {
    attachInterrupt(16,0,LOW); //Digital Pins A2 == 16
    detachSerial(4);
    rtc.standbyMode();    // Sleep until next alarm match

   // lightON();
    reattach();
    // Wait until Serial is ready
    while((!(Sensor1 || Serial2 || Sensor3 || SeeStar)));

    awaken();
  }//loop()





