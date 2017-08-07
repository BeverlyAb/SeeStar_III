/* Author : Beverly Abadines 
 * Date : 8/1/17
 * 
 * Based on 5_Serial.ino, this uses 4 of the available ports. Ideally, 3 ports are designated for 3 sensors with 
 * 1 port reserved for SeeStar, but for this application one of the ports (Sensor2) is used to produce dummy data
 * for Sensor 1 and Sensor3 to read from. 
 * 
 * myWakeUpv9 Protocol:
 * The Feather needs to be woken up by an initial signal or byte sent from the keyboard. Once awake, it will parse 
 * and process commands before it goes back to sleep. This means that the initial byte is not read and excluded from the buffer. 
 * 
 * Commands: 
 * $1TS : Orders Sensor1 to read data and store onto file
 * $2TS : Orders Sensor2 to read data and store onto file (Unused for this application)
 * $3TS : Orders Sensor3 to read data and store onto file
 * $ATS : Orders ALL sensors to read data and store onto file
 * $LOG : Reads and sends the last log(s) to the user. (In progress) 
 * $SET : Updates the Feather's time  
 * $GET : Sends the time back to the user. (In progress) 
 */
#include <Arduino.h>        // sercom
#include "wiring_private.h" // sercom
#include <SPI.h>            // SD
#include <SD.h>             // SD
#include <RTCZero.h>        // time

#define SD_LED 8

#define SeeStar Serial4
#define Sensor1 Serial1
//#define Sensor2 Serial2 // Uncomment this block once Sensor2 is no longer used for data producing and remove all Serial2.write(i++);
#define Sensor3 Serial3

#define S1 1
#define S2 2
#define S3 3
#define SS 4

#define idle 10 // timeout period
//------------------------------------------------------------------------------------------------------------
//global instantiations
RTCZero rtc;
String command = "";
const int chipSelect = 4;
uint8_t i = 0;
String sensor = "";
String Time = "";
uint8_t seconds = 0;
uint8_t minutes = 32;
uint8_t hours = 2;
uint8_t day = 0;
uint8_t month = 0;
uint8_t year =  0;

//Sensor1: RX = 0, TX = 1
Uart Sensor1 (&sercom0, 0, 1, SERCOM_RX_PAD_3, UART_TX_PAD_2);
void SERCOM0_Handler()
{
  Sensor1.IrqHandler();
}

//Dummy Data Producer: RX = 12, TX = 10  
Uart Serial2 (&sercom1, 12, 10, SERCOM_RX_PAD_3, UART_TX_PAD_2);
void SERCOM1_Handler()
{
  Serial2.IrqHandler();
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

/*------------------------------------------------------------------------------------------------------------
 *  Initializes SD config.
 */
void sdInit()
{
    //--------------SD -------------------------------------------------------------
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
 * This is used to reattach the Serials after being disconnected before sleeping. 
 */
void reattach()
{
  Serial.begin(9600);
  
  Sensor1.begin(9600);
  pinPeripheral(1, PIO_SERCOM); //TX
  pinPeripheral(0, PIO_SERCOM); //RX

  Serial2.begin(9600);
  pinPeripheral(10, PIO_SERCOM); //TX
  pinPeripheral(12, PIO_SERCOM); //RX

  Sensor3.begin(9600);
  pinPeripheral(11, PIO_SERCOM_ALT);//TX
  pinPeripheral(6, PIO_SERCOM_ALT); //RX
  
  SeeStar.begin(9600);
  pinPeripheral(20, PIO_SERCOM_ALT); //TX
  pinPeripheral(21, PIO_SERCOM_ALT); //RX

  sdInit();
}//reattach()

/*------------------------------------------------------------------------------------------------------------
 * Detaches 1 to "n" Serials
 */
void detachSerial(int n)
{
  switch(n) {
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
 *  Used to serially push out a String with Serial.write()
 *  serNum dictates which Serial writes
 */
void writeString(String stringData, int serNum) { 
  switch(serNum) {
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
 * Ensures that Feather sleeps after "n" secs. of being awake
 */
bool timeout(int n)
{
    return (rtc.getSeconds() > seconds + n);
}//timeout()


/*------------------------------------------------------------------------------------------------------------
 * An initial signal from the keyboard wakes up the Feather. The subsequent typed keys are then the received 
 * commands.
 */
void awaken()
{
 command = "";
 command = "\n\rAwake at " + String(rtc.getSeconds()) + "secs";
  writeString(command, SS);
  seconds = rtc.getSeconds();
  while(!SeeStar.available()){
    if(timeout(idle)){
     writeString("Going back to sleep", SS);
      break;
    }
  }
  //Dummy Data 
  Serial2.write(i++);  

  if(SeeStar.available()) {
    command = SeeStar.readString(); 
    writeString("Read " + command + " at " + String(rtc.getSeconds()) + "secs", SS);  
    parseCommand(command);
  }
 // writeString("Going back to sleep\n\r", 4);
    writeString("Going back to sleep", 4);
}//awaken()

/*------------------------------------------------------------------------------------------------------------
 * Sets and updates time through user input. In Progress
 */
void updateClk()
{
 clkPrompt("Hour?");
 clkPrompt("Minute?");
 clkPrompt("Second?");
 rtc.setTime(hours,minutes,seconds);
}
/*------------------------------------------------------------------------------------------------------------
 * Sets time parameter. In Progress
 */
void clkPrompt(String setT)
{
  writeString(setT, SS);
  while(!SeeStar.available()){
    if(timeout(idle)){
      writeString("Going back to sleep", SS);
      break;
    }
  }
  byte mybyte = 0;
 // if(SeeStar.available()){
    if(setT == "Hour?") {
      while(SeeStar.read() != 10 || timeout(idle)){ //10 == '\n' 
        if(SeeStar.available()) {
          mybyte +=SeeStar.read();
          hours = mybyte - 48;
          writeString(String(hours), SS);
        }
      }
      writeString(String(hours), SS);
    } else if(setT == "Minute?") {
      minutes = SeeStar.read() -48;
      writeString(String(minutes), SS);
    } else if(setT == "Second?") {
      seconds = SeeStar.read() -48;
      writeString(String(seconds), SS);
    } else {
      SeeStar.println("Error with clkPrompt()\r");
    }
 // } 
}
/*------------------------------------------------------------------------------------------------------------
 * Parses Message sent into an operation. Calls sdWrite()
 */
int parseCommand(String parseCom) 
{  
    parseCom.trim(); 
    hours = rtc.getHours();
    minutes = rtc.getMinutes();
    seconds = rtc.getSeconds();
     
    sensor = "";  
    if(parseCom == "$1TS"){
      while(!Sensor1.available());
      sensor = "S1";
   //   sensor += "\t\t\t\t" + String(Sensor1.read()) + "\n\r";
      sensor += "\t\t\t\t" + String(Sensor1.read());
    } /*else if(parseCom == "$2TS") { // Uncomment this block once Sensor2 is no longer used for data producing
      i = Sensor2.read();
      sensor = "S2";
      Sensor2.write(i); 
    }*/ else if(parseCom == "$3TS") {
      while(!Sensor3.available());
      sensor = "S3";
      //sensor += "\t\t\t\t" + String(Sensor3.read()) + "\n\r";
      sensor += "\t\t\t\t" + String(Sensor3.read());
    }  else if(parseCom == "$ATS") { 
      parseCommand("$1TS");
     // parseCommand("go2"); // Uncomment this line once Sensor2 is no longer used for data producing
      parseCommand("$3TS");
      return 1;
    } else if(parseCom == "$SET") {
      updateClk();
      sensor = "Updated Clock";
    } else if(parseCom == "$GET") {
      sensor = String(hours) + " : " + String(minutes) + " : " + String(seconds);
    } else if(parseCom == "$LOG") {
      returnLog();
      return 2;
    }
    else {
      //writeString("Invalid Command\n\r.", SS);
      writeString("Invalid Command.", SS);
      //sensor = "NO GO SENSOR\n\r";
      sensor = "NO GO SENSOR";
    }
    sdWrite(sensor);
    writeString(sensor, 4);
    return 0;
}//parseCommand(String)

/*------------------------------------------------------------------------------------------------------------
 *  Records specific sensor data onto SD Card
 */
void sdWrite(String writeCom)
{
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
  refreshDate();
  // if the file is available, write to it:
  if (dataFile) {
    digitalWrite(SD_LED, HIGH);
    Time =  String(month) + "/" + String(day) + "/" + String(year) + "\t\t\t\t" +
            String(hours) + ":" + String(minutes) + ":" + String(seconds) + "\t\t\t";
    dataFile.print(Time);
    Serial.print(Time);
    sensor = writeCom;
    dataFile.println(sensor);
    Serial.println(sensor);
    dataFile.close();
  }
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog.txt");
  }
  //indicator of writes
  digitalWrite(SD_LED, LOW);
  delay(700);
}//sdWrite(String)

/*------------------------------------------------------------------------------------------------------------
 * Updates date at midnight. Only works if the time rolls over to midnight. Called by sdWrite();  
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
 *  Blinks RED LED and verifies if Feather is Awake. Should be removed from loop if deployed to decrease 
 *  power consumption.
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

/*------------------------------------------------------------------------------------------------------------
 *  Reads and sends the last log(s) to the user. In progress  
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
   command =  command.substring(command.length() - 50, command.length());
   delay(100);
   writeString(command,SS);
   dataFile.close();
  } else {
    // if the file didn't open, print an error:
    SeeStar.println("error opening test.txt");
  }
}
//---------------------------------------------------------------------------------------------
void setup() {
  reattach();
  rtc.begin();
  rtc.setTime(hours,minutes,seconds);

  //-------------Time Settings---------------------------------------------------
  // get current date
  year = rtc.getYear();
  month = rtc.getMonth();
  day = rtc.getDay();

  //Dummy Data
  Serial2.write(i);
}//setup()

//---------------------------------------------------------------------------------------------
void loop() 
{
  attachInterrupt(16,0,LOW); //Digital Pins A2 == 16
  detachSerial(4);
  rtc.standbyMode();    // Sleep until next alarm match
  
  lightON();
  reattach();
  // Wait until Serial is ready
  while((!(Sensor1 || Serial2 || SeeStar))); 
  awaken();
}//loop()





