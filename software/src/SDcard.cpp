#include <SPI.h>
#include <SD.h>
#include "SDcard.h"
#include "RTCDS1307.h"
#include "measure.h"

bool initsdcard=false;

void SD_setup(){
  Serial.println("Initializing SD card...");
  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
    initsdcard=false;
    return;
  }
  Serial.println("initialization done.");
  initsdcard=true;

  // re-open the file for reading:
  File myFile = SD.open("database.txt");
  if (myFile) {
    Serial.println("database.txt");

    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      Serial.write(myFile.read());
    }
    // close the file:
    myFile.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening file");
  }
}

void SD_write(String namedata, String value)
{
  File myFile = SD.open(namedata, FILE_WRITE);
  if (myFile) {
    Serial.print("Writing to ");Serial.println(namedata);
	  myFile.println(value);
    myFile.close();
    Serial.println("done.");
  }
  else {
    Serial.println("error opening file");
  }
}

void SD_delete(String namedata)
{
  if (SD.exists(namedata)) {
    Serial.print("Removing ");Serial.println(namedata);
    SD.remove(namedata);
  }
}
