#include <Fishino.h>

// SD values (hard coded)
#define SERVER "sclsamt.pythonanywhere.com" // server's address
// WIFI Parameters
#define SSID "MOD340"
#define PASSWORD "passM340"

void connectWiFi() {
  while (!Fishino.begin(SSID, PASSWORD)) {}
  Fishino.staStartDHCP();
  Serial.print("Connecting to WIFI..");
  while (Fishino.status() != STATION_GOT_IP) {
    Serial.print(".");
    delay(300);
  }
}

void setup()
{
  Serial.begin(9600);

  //WIFI
  Fishino.setMode(STATION_MODE);
  connectWiFi();
}

void loop ()
{
  Serial.println("in main loop");
  delay(2000);
}
