#include "../conf.h"// must define MY_SSID and MY_SSID_PASSWD
#include "common.h"

#include <WiFi.h>
#include <ArduinoOTA.h>

// #include "sdcard.h"
#include "gui.h"


WiFiUDP udp;

/* ntp setup */
EasyNTPClient ntp(udp, "pool.ntp.org", 2*(60*60)); // GMT+2




const char* ssid = MY_SSID;
const char* password = MY_SSID_PASSWD;
void wifiSetup() {
    WiFi.mode(WIFI_STA); //Optional
    WiFi.begin(ssid, password);
    Serial.println("\nConnecting");

    while(WiFi.status() != WL_CONNECTED){
        Serial.print(".");
        delay(100);
    }

    Serial.println("\nConnected to the WiFi network");
    Serial.print("Local ESP32 IP: ");
    Serial.println(WiFi.localIP());

}

void arduinoOTASetup( ) {
  // Port defaults to 8266
  // ArduinoOTA.setPort(8266);

  // Hostname defaults to esp8266-[ChipID]
  ArduinoOTA.setHostname("esp32s3-display");

  // No authentication by default
  ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

#ifdef UM_FEATHER_S3
static bool shutdown_request = false;
void IRAM_ATTR ISR_shutdown() {
  shutdown_request = true;
}
#endif

void shutdownSetup() {
  #ifdef UM_FEATHER_S3
	pinMode(0, INPUT_PULLUP);
	attachInterrupt(0, ISR_shutdown, CHANGE);
  #endif
}




void setup()
{
    wifiSetup();
    arduinoOTASetup();
    boardSetup();
    shutdownSetup();
    imGuiSetup();

    Serial.println( "Setup done" );

   // init time
   // currentTime.getDateTime(ntp.getUnixTime());
}

void loop()
{
    // check for WiFi OTA updates
    ArduinoOTA.handle();
  	boardLoop();
    imGuiLoop();
    //delay( 5 );// ms
    #ifdef UM_FEATHER_S3
    if (shutdown_request) {
        shutdown_request = false;
        #ifdef SHARPMEMORYDISPLAY
        display.clearDisplay();
        #endif
        esp_deep_sleep_start();
    }
    #endif
}

