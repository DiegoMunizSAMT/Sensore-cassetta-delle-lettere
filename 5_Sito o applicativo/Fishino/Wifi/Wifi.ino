#include <Fishino.h>
#include <FishinoSdFat.h>
#include <SPI.h>
#include "FishinoSdFat.h"
#include "FreeStack.h"

//----------------------------Other----------------------------
FishinoClient client;

String strs[5];
String ssid = "";
String password = "";
String api_key = "";
String serverPath = "";
String stringServer = "";
char server[50];


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
  Serial.print(F("Initializing SD card..."));
  if (!SD.begin(SD_CS_PIN, SD_SCK_MHZ(15))) {
    Serial.println(F("initialization failed!"));
    SD.initErrorHalt();
  }
  Serial.println(F("Initialization done."));
  // List files in root directory.
  if (!dirFile.open("/", O_READ)) {
    SD.errorHalt(F("open root failed"));
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
  Serial.print(F("file content: "));
  Serial.println(fileContent);
  file.close();
  return fileContent;
}

void parseFromSDCardFile() {
  String str = readSDCardFile();
  int i = 0;
  int j = 0;
  String temp = "";
  while(i < str.length()-1){
    if(str.charAt(i) == ','){
      strs[j]= temp;
      temp="";
      j++;
    }else{
      temp += str.charAt(i);
    }
    i++;
  }
}

//------------------------------------------------------------

//----------------------------WIFI----------------------------
void connectWiFi(String ssid,String password) {
  while (!Fishino.begin(ssid.c_str(), password.c_str())) {} 
  Fishino.staStartDHCP();
  Serial.print(F("Connecting to WIFI"));
  while (Fishino.status() != STATION_GOT_IP) {
    Serial.print(F("."));
    delay(650);
  }
  Serial.println(F("OK"));
}
//------------------------------------------------------------

void dataSender(){
  
  if (client.status()) {
    client.stop();
  }
  
  char json_data[100] = "{\"api_key\":\"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx\",\"mail_present\":\"yes\"}";

  int key_index = 0;
  char placeholder = 'x';
  for (int i=0; i<sizeof(json_data); i++){
    char found = json_data[i];
    char repl = api_key.charAt(key_index);
    if (found == placeholder){
      json_data[i] = repl;
      key_index++;
    }
  }
  Serial.print(F("json_data: "));
  Serial.println(json_data);  
  
  if (client.connect(server, 80)){
    client.print(F("POST "));
    client.print(serverPath);
    client.println(F(" HTTP/1.1"));
    client.print(F("Host: "));
    client.println(server);
    client.println(F("User-Agent: FISHINO_CA"));
    client.println(F("Content-Type: application/json; charset=utf-8"));
    client.println();
    client.print(json_data);
    client.flush();
    client.stop();    
  } else {
    Serial.println(F("no connection to server"));
  }
  Serial.println(F("end datasender"));
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
  stringServer = strs[4];
  
  for (int i=0; i<stringServer.length(); i++){
    char c = stringServer.charAt(i);
    server[i] = c;
  }

  Serial.print(F("server (string) : "));
  Serial.println(stringServer);
  Serial.print(F("server (char) : "));
  Serial.println(server);  
  Serial.print(F("server path: "));
  Serial.println(serverPath);
  Serial.print(F("api: "));
  Serial.println(api_key);
  Serial.print(F("SSID: "));
  Serial.println(ssid);
  Serial.print(F("password: "));
  Serial.println(password);
  
  connectWiFi(ssid,password);
}
//------------------------------------------------------------------------------
int oldSensorVal = 1;
void loop ()
{
  int currentSensorVal = digitalRead(2);
  if(currentSensorVal == 0 && oldSensorVal == 1){
    Serial.println(F("invio"));
    dataSender();
    oldSensorVal = 0;
  } else if (currentSensorVal == 1 && oldSensorVal == 0) {
    oldSensorVal = 1;
  }
}
//------------------------------------------------------------------------------
