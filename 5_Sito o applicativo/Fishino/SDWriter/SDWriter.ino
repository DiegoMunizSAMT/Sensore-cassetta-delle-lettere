#include <FishinoSdFat.h>
SdFat sd;
File myFile;
void writeSDCardFile() {
  sd.remove("config.txt");
  String configData = "MOD340";
  configData += " passM340";
  configData += " bad6f4d7-3c35-4d34-92c0-f18839a5372a";
  configData += " /w2p/scl/default/devices/api/recorded_value";
  configData += " 164.128.168.41 ";
  
  myFile = sd.open("config.txt", FILE_WRITE);
  if(myFile) {
    myFile.print(configData);
    // close the file:
    myFile.close();
    //Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test.txt");
  }
}

void setup() {
  Serial.begin(115200);
  writeSDCardFile();
}

void loop() {
  
}
