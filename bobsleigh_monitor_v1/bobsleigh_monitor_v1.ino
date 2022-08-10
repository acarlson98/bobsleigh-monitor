/*
  Example using the SparkFun HX711 breakout board with a scale
  By: Nathan Seidle
  Adapted By Andrew Carlson & Jacob Wehrman
  SparkFun Electronics
  Date: November 19th, 2014
  Revised July 8th, 2022

  This does basic scale output. See the calibration sketch to get the calibration_factor for your
  specific load cell setup.

  This code uses bogde's excellent library: https://github.com/bogde/HX711
  bogde's library is released under a GNU GENERAL PUBLIC LICENSE

  The HX711 does one thing well: read load cells. The breakout board is compatible with any wheat-stone bridge
  based load cell which should allow a user to measure everything from a few grams to tens of tons.

  The HX711 board will be powered at 3.3 V
  ---
  REVISED

  The updated code also outputs data to an SD card to be read later.
  The code also collects data upon receiving user input via a push button switch
  and outputs the status of the collection process via LEDs (Arduino on, green, and recording, red).

*/


#include "HX711.h"
#include <Arduino_LSM6DS3.h>
#include <SPI.h>
#include <SD.h>

#define calibration_factor 8300.0 //This value is obtained using the SparkFun_HX711_Calibration sketch

#define SWITCH 2
#define GRNLED 3
#define REDLED 4
#define CLK  5
//#define DOUTA  6
#define DOUTB 7
#define DOUTC 8
#define DOUTD 9
// D10-13 = SPI for SD Card Adapter

File myFile;
//HX711 scaleA;
HX711 scaleB;
HX711 scaleC;
HX711 scaleD;

int buttonState = 0;
unsigned long startMillis;
unsigned long runTime = 30000; // in milliseconds
unsigned long count = 0;

void setup() {

  pinMode(GRNLED, OUTPUT);
  pinMode(REDLED, OUTPUT);
  pinMode(10, OUTPUT);

  digitalWrite(GRNLED, HIGH);
  digitalWrite(REDLED, LOW);

  // Serial
  /*Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
    digitalWrite(GRNLED, LOW);
    digitalWrite(REDLED, HIGH);
  }*/

  digitalWrite(REDLED, LOW);
  digitalWrite(GRNLED, LOW);

  // SD Card
  Serial.println("Initializing SD card...");
  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    digitalWrite(GRNLED, LOW);
    digitalWrite(REDLED, HIGH);
    while (1);
  }

  digitalWrite(REDLED, LOW);
  digitalWrite(GRNLED, HIGH);
  
  // IMU
  Serial.println("Initializing IMU...");
  if (!IMU.begin()) {
    myFile.println("Failed to initialize IMU!");
    digitalWrite(GRNLED, LOW);
    digitalWrite(REDLED, HIGH);
    while (1);
  }

  digitalWrite(REDLED, LOW);
  digitalWrite(GRNLED, LOW);

  // Load Cells
  Serial.println("Setting Up HX711's...");

  /*
    scaleA.begin(DOUTA, CLK);
    scaleA.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
    scaleA.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0

    Serial.println("Finished A...");
  */
  scaleB.begin(DOUTB, CLK);
  scaleB.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scaleB.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0

  digitalWrite(GRNLED, HIGH);

  myFile.println("Finished B...");

  scaleC.begin(DOUTC, CLK);
  scaleC.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scaleC.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0

  digitalWrite(GRNLED, LOW);

  Serial.println("Finished C...");

  scaleD.begin(DOUTD, CLK);
  scaleD.set_scale(calibration_factor); //This value is obtained by using the SparkFun_HX711_Calibration sketch
  scaleD.tare(); //Assuming there is no weight on the scale at start up, reset the scale to 0

  digitalWrite(GRNLED, HIGH);

  Serial.println("Finished D...");

  Serial.print("Accelerometer sample rate = ");
  Serial.print(String(IMU.accelerationSampleRate(),3));
  Serial.println(" Hz");
  Serial.println("");
  Serial.println("Acceleration in G's");
  Serial.println("X\tY\tZ");

  Serial.println("Ready to Read");

  digitalWrite(GRNLED, LOW);
}

void loop() {

  // When button is pressed, record for 30 seconds and write to a new file on SD card
  buttonState = digitalRead(SWITCH);

  if (buttonState != HIGH) {

    char filename[] = "SLED0000.TXT";      //The file names must be in the 8.3 format
    for (uint8_t i = 0; i < 10000; i++) {
      filename[4] = i/1000 + '0';
      filename[5] = i/100 + '0';
      filename[6] = i/10 + '0';
      filename[7] = i%10 + '0';
      if (! SD.exists(filename)) {
        // only open a new file if it doesn't exist
        myFile = SD.open(filename, FILE_WRITE); 
        break;  // leave the loop!
      }
    }
    
    if (!myFile) {
      Serial.println("SD Open Error!");
      digitalWrite(REDLED, HIGH);
      while (1);
    }
    
    startMillis = millis(); // mark time
    count = 0; // reset count
    digitalWrite(GRNLED, HIGH); // show recording

    while (millis() - startMillis < runTime)
    {

      //float forceA = scaleA.get_units();
      float forceB = scaleB.get_units();
      float forceC = scaleC.get_units();
      float forceD = scaleD.get_units();

      float netforce = (forceB + forceC) - forceD;

      float x, y, z;

      // TODO: Do we want to only be taking data when the IMU is getting acceleration?
      if (IMU.accelerationAvailable()) {
        IMU.readAcceleration(x, y, z);

        myFile.print("Acceleration: ");
        myFile.print(String(x, 3));
        myFile.print(String('\t'));
        myFile.print(String(y, 3));
        myFile.print(String('\t'));
        myFile.println(String(z, 3));
      }

      /*myFile.print("Force A: ");
        myFile.print(forceA);
        myFile.print(", ");*/
      myFile.print("Force B: ");
      myFile.print(String(forceB, 3));
      myFile.print(", ");
      myFile.print("Force C: ");
      myFile.print(String(forceC, 3));
      myFile.print(", ");
      myFile.print("Force D: ");
      myFile.print(String(forceD, 3));
      myFile.print(", ");

      myFile.print("Netforce: ");
      myFile.print(String(netforce, 3));

      myFile.println(" lbs"); //You can change this to kg but you'll need to refactor the calibration_factor

      count ++;
    } // end timed while loop

    myFile.print(String(count));
    myFile.println(" readings taken.");
    myFile.close();
    digitalWrite(GRNLED, LOW);

  } // end if button pressed
} // end loop()
/*
void myFile.print(String toPrint) {
  Serial.print(toPrint);
  myFile.print(toPrint);
}

void myFile.println(String toPrint) {
  Serial.println(toPrint);
  myFile.println(toPrint);
}
*/
