/*
  SD card read/write

 This example shows how to read and write data to and from an SD card file
 The circuit:
 * SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13

 created   Nov 2010
 by David A. Mellis
 modified 9 Apr 2012
 by Tom Igoe

 This example code is in the public domain.

 */

#include <SPI.h>
#include "FishinoSdFat.h"
SdFat SD;

#ifdef SDCS
	#define SD_CS_PIN SDCS
#else
	#define SD_CS_PIN SS
#endif

File myFile;

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.print("Initializing SD card...");

  if (!SD.begin(SD_CS_PIN, SD_SCK_MHZ(15))) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");

  // open the file. note that only one file can be open at a time,
  // so you have to close this one before opening another.
  myFile = SD.open("config.txt", FILE_WRITE);
  myFile.remove();
  myFile.open("config.txt", FILE_WRITE);
  
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to config.txt...");
    String configData = "MOD340";
    configData += " passM340";
    configData += " bad6f4d7-3c35-4d34-92c0-f18839a5372a";
    configData += " /w2p/scl/default/devices/api/recorded_value";
    configData += " 164.128.168.41";
    myFile.println(configData);
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening config.txt");
  }

  // re-open the file for reading:
  myFile = SD.open("config.txt");
  if (myFile) {
    Serial.println("config.txt:");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening config.txt");
  }
}

void loop() {
  // nothing happens after setup
}
