#include <Fishino.h>
#include <FishinoSdFat.h>
#include <SPI.h>
#include "FishinoSdFat.h"
#include "FreeStack.h"

//----------------------------Other----------------------------
FishinoClient client;

String ssid = "";
String password = "";
String api_key = "";
String serverPath = "";
char server[] = "";

SdFat SD;
File file;
SdFile dirFile;

//------------------------------------------------------------

//---------------------------SD Card--------------------------
// SD card chip select pin.
#ifdef SDCS
  const uint8_t SD_CS_PIN = SDCS;
#else
  const uint8_t SD_CS_PIN = SS;
#endif

// Number of files found.
uint16_t n = 0;
// Max of ten files since files are selected with a single digit.
const uint16_t nMax = 10;
// Position of file's directory entry.
uint16_t dirIndex[nMax];
// l'ultima volta che vi siete connessi al server, in millisecondi
unsigned long lastConnectionTime = 0;


void printSDCardFiles(){
  Serial.print("Initializing SD card...");
  if (!SD.begin(SD_CS_PIN, SD_SCK_MHZ(15))) {
    Serial.println("initialization failed!");
    SD.initErrorHalt();
  }
  Serial.println("Initialization done.");
  // List files in root directory.
  if (!dirFile.open("/", O_READ)) {
    SD.errorHalt("open root failed");
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

String readSDCardFile(int c = 0) {
  if (c >= n) {
    Serial.println(F("Invald number"));
    return;
  }
  String fileContent = "";
  char last = 0;
  file = SD.open("config.txt");
  // Copy up to 500 characters to Serial.
  for (int k = 0; k < 500 && (c = file.read()) > 0; k++)  {
    fileContent += (char)c;
  }
  file.close();
  delay(100);
  return fileContent;
}

String strs[5];

void parseFromSDCardFile() {
  String str = readSDCardFile();
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
}
//------------------------------------------------------------

//----------------------------WIFI----------------------------
void connectWiFi(String ssid,String password) {
  while (!Fishino.begin(ssid.c_str(), password.c_str())) {} 
  Fishino.staStartDHCP();
  Serial.print("Connecting to WIFI");
  while (Fishino.status() != STATION_GOT_IP) {
    Serial.print(".");
    delay(650);
  }
  Serial << F("OK\r\n");
}
//------------------------------------------------------------


void dataSender(){
  client.stop();

  if (client.connect(server, 80))
  {
    Serial << F("connecting...\n");
    String json_data = "{'api_key':" + api_key + ",'mail_present':'"+"'yes'}";
    
    // invia la richiesta HTTP:
    client << F("POST /w2p/scl/default/api/recorded_value HTTP/1.1\r\n");
    client << F("Host: www.fishino.it\r\n");
    client << F("Content-Type: application/json\r\n");
    client << F("User-Agent: FishinoWiFi/1.1\r\n");
    client << F("Connection: close\r\n");
    client.println();
  }else{
    Serial.println("connection error");
  }

  lastConnectionTime = millis();
}

void setup()
{
  Serial.begin(115200);
  
  // Sensor
  pinMode(2, INPUT);
  pinMode(4, OUTPUT);

  // WIFI
  Fishino.setMode(STATION_MODE);
  
  printSDCardFiles();
  parseFromSDCardFile();
  ssid = strs[0];
  password = strs[1];
  api_key = strs[2];
  serverPath = strs[3];
  String stringServer = strs[4];
  stringServer.toCharArray(server,stringServer.length());
  connectWiFi(ssid,password);
}

//------------------------------------------------------------------------------
int oldSensorVal = 1;
int inviato = 0;
void loop ()
{
  int currentSensorVal = digitalRead(2);
  Serial.print("valore sensore: ");
  Serial.println(currentSensorVal);

  /*
   * non invia il pacchetto quando c'è un cambiamento di stato
   * TODO
   */
  if(currentSensorVal != oldSensorVal){
    Serial.println("cambiamento di stato");
    if (currentSensorVal == 0)
    {
      Serial.println("invio");
      inviato = 1;
      //dataSender();
    }
    else {
      oldSensorVal = currentSensorVal;
      inviato = 0;
    }
  }
}

//------------------------------------------------------------------------------
