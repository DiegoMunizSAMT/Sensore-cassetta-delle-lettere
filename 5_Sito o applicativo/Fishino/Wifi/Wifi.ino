#include <Fishino.h>
#include <FishinoSdFat.h>
#include <SPI.h>
#include "FishinoSdFat.h"
#include "FreeStack.h"

//----------------------------Other----------------------------
String getIP() {
  // Get IP Address and covert to String
  IPAddress broadCast = Fishino.localIP();
  String ip = "";
  for(int i = 0; i < sizeof(broadCast) - 2; i++) {
    if(i != sizeof(broadCast) - 3) {
      ip += String((int)broadCast[i]) + ".";
    }else {
      ip += String((int)broadCast[i]);
    }
  }
  
  return ip;
}

String getMAC() {
  // Get MAC Address and covert to String
  uint8_t const *rawMac = Fishino.macAddress();
  String mac = "";
  for(int i = 0; i < WL_MAC_ADDR_LENGTH; i++)
  {
    if(i != WL_MAC_ADDR_LENGTH - 1) {
      if(String(rawMac[i], HEX).length() < 2) {
        mac += "0" + String(rawMac[i], HEX) + ":";
      }else {
        mac += String(rawMac[i], HEX) + ":";
      }
    }else {
      if(String(rawMac[i], HEX).length() < 2) {
        mac += "0" + String(rawMac[i], HEX);
      }else {
        mac += String(rawMac[i], HEX);
      }
    }
  }
  
  return mac;
}
//------------------------------------------------------------

//----------------------------JSON----------------------------
String createJsonText() {
  // Create Json
  String json_data = "{";
  json_data = json_data + "\"device_id\": \"" + getMAC() 
  + "\", \"ssid\": " + "MOD340" 
  + ", \"password\": " + "passM340" + "}";
  
  return json_data;
}
//------------------------------------------------------------

//---------------------------SD Card--------------------------
// SD card chip select pin.
#ifdef SDCS
  const uint8_t SD_CS_PIN = SDCS;
#else
  const uint8_t SD_CS_PIN = SS;
#endif

SdFat sd;
SdFile file;
SdFile dirFile;

// Number of files found.
uint16_t n = 0;
// Max of ten files since files are selected with a single digit.
const uint16_t nMax = 10;
// Position of file's directory entry.
uint16_t dirIndex[nMax];

void printSDCardFiles(){
  Serial.print("Initializing SD card...");
  if (!sd.begin(SD_CS_PIN, SD_SCK_MHZ(15))) {
    Serial.println("initialization failed!");
    sd.initErrorHalt();
  }
  Serial.println("Initialization done.");

  // List files in root directory.
  if (!dirFile.open("/", O_READ)) {
    sd.errorHalt("open root failed");
  }
  while (n < nMax && file.openNext(&dirFile, O_READ)) {

    // Skip directories and hidden files.
    if (!file.isSubDir() && !file.isHidden()) {

      // Save dirIndex of file in directory.
      dirIndex[n] = file.dirIndex();

      // Print the file number and name.
      Serial.print(n++);
      Serial.write(' ');
      file.printName(&Serial);
      Serial.println();
    }
    file.close();
  }
}

void deleteSDCardFile() { 
  sd.remove("config.txt");
}

File myFile;
void writeSDCardFile() {
  String configData = "MOD340";
  configData += " "; 
  configData += "passM340";
  configData += " "; 
  configData += "sclsamt.pythonanywhere.com";
  
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

String readSDCardFile(int c = 0) {
  if (c >= n) {
    Serial.println(F("Invald number"));
    return;
  }
  
  if (!file.open(&dirFile, dirIndex[c], O_READ)) {
    sd.errorHalt(F("open"));
  }
  //Serial.println();

  String fileContent = "";
  char last = 0;

  // Copy up to 500 characters to Serial.
  for (int k = 0; k < 500 && (c = file.read()) > 0; k++)  {
    fileContent += (char)c;
  }
  file.close();
  delay(100);
  return fileContent;
}

String parseSSIDFromSDCardFile(String str) {
  String strs[3];
  int StringCount = 0;
  
  // Split the string into substrings
  while (str.length() > 0)
  {
    int index = str.indexOf(' ');
    if (index == -1) // No space found
    {
      strs[StringCount++] = str;
      break;
    }
    else
    {
      strs[StringCount++] = str.substring(0, index);
      str = str.substring(index+1);
    }
  }
  return strs[0];
}

String parsePasswordFromSDCardFile(String str) {
  String strs[3];
  int StringCount = 0;
  
  // Split the string into substrings
  while (str.length() > 0)
  {
    int index = str.indexOf(' ');
    if (index == -1) // No space found
    {
      strs[StringCount++] = str;
      break;
    }
    else
    {
      strs[StringCount++] = str.substring(0, index);
      str = str.substring(index+1);
    }
  }
  return strs[1];
}

String parseServerFromSDCardFile(String str) {
  String strs[3];
  int StringCount = 0;
  
  // Split the string into substrings
  while (str.length() > 0)
  {
    int index = str.indexOf(' ');
    if (index == -1) // No space found
    {
      strs[StringCount++] = str;
      break;
    }
    else
    {
      strs[StringCount++] = str.substring(0, index);
      str = str.substring(index+1);
    }
  }
  return strs[2];
}
//------------------------------------------------------------

//----------------------------WIFI----------------------------
void connectWiFi(String ssid, String password) {
  while (!Fishino.begin(ssid.c_str(), password.c_str())) {} 
  Fishino.staStartDHCP();
  Serial.print("Connecting to WIFI..");
  while (Fishino.status() != STATION_GOT_IP) {
    Serial.print(".");
    delay(300);
  }
  Serial << F("OK\r\n");
}
//------------------------------------------------------------

void setup()
{
  Serial.begin(115200);
  
  // Sensor
  pinMode(2, INPUT);
  pinMode(4, OUTPUT);

  // WIFI
  Fishino.setMode(STATION_MODE);
  
  //printSDCardFiles();
  //deleteSDCardFile();
  //writeSDCardFile();
  //connectWiFi(parseSSIDFromSDCardFile(readSDCardFile()), parsePasswordFromSDCardFile(readSDCardFile()));
}

//------------------------------------------------------------------------------

int prevSensorVal = HIGH;
unsigned long blockTime = 0;
void loop ()
{
  /* IR Sensor
  int currentSensorVal = !digitalRead(2); // 1 = Recives IR, 0 = Doesn't recive IR 

  if (currentSensorVal == LOW && prevSensorVal == HIGH) {
    blockTime = millis();
  } 
  else {
    if (millis() - blockTime > 1000) {
      Serial.println("Object detected!");
      blockTime = millis();
    }
  }

  prevSensorVal = currentSensorVal;
  
  */

  int currentSensorVal = digitalRead(2);
  Serial.println(currentSensorVal);
}

//------------------------------------------------------------------------------
