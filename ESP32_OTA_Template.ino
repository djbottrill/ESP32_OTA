//Needed for OTA
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <credentials.h>  //My WiFi credentials are in a custom Library

#include <Adafruit_NeoPixel.h>

#define LED_PIN 48   // GPIO pin connected to the WS2812 LED
#define LED_COUNT 1  // Number of LEDs in the strip

Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

bool S3 = false;
bool debug = false;
bool flasher = false;

TaskHandle_t Task1;              //Task handle
const char *hostName = "ESP32";  //Hostname
bool ota_t = false;              //Flag to indicate WiFi / ota startup has completed
#include "ota.h"


void setup() {

#ifdef BUILTIN_LED  //Legacy approach to check if a board has an LED as LED_BUILTIN is a const uint8_t
  pinMode(LED_BUILTIN, OUTPUT);
#endif

  String boardName = ARDUINO_BOARD;
  if (boardName.indexOf("ESP32S3_DEV") != -1) {
    strip.begin();
    strip.setPixelColor(0, strip.Color(0, 0, 8));
    strip.show();  // Set LED to Blue
    S3 = true;
  }

  Serial.begin(115200);
  int ii = 100;
  while (ii > 0) {  //Wait for serial port to connect
    if (S3) {
      if (!flasher) {
        strip.setPixelColor(0, strip.Color(0, 0, 8));  //Flash the LED as blue while serial connects or times out
      } else {
        strip.setPixelColor(0, strip.Color(0, 0, 0));
      }
      strip.show();
    }
#ifdef BUILTIN_LED
    if (S3 == false) digitalWrite(BUILTIN_LED, flasher);  //If the board has an LED then flash it
#endif
    flasher = !flasher;
    delay(100);
    if (Serial) {
      debug = true;  //Flag to say serial port has connected
      ii = 1;        //Bail out if serial port connects
    }
    ii--;
  }
  if (debug == true) {
    Serial.println();
    Serial.println();
    Serial.println("  OOOOOO   TTTTTTTT   AAAAAA  ");
    Serial.println(" O      O     TT     A      A ");
    Serial.println(" O      O     TT     A      A ");
    Serial.println(" O      O     TT     AAAAAAAA ");
    Serial.println(" O      O     TT     A      A ");
    Serial.println(" O      O     TT     A      A ");
    Serial.println("  OOOOOO      TT     A      A ");
    Serial.println("                              ");
    Serial.println("     OTA Loader for ESP32     ");
    Serial.println("        David Bottrill        ");
    Serial.println(" Shady Grove Electronics 2023 ");
    Serial.println();
  }





  xTaskCreatePinnedToCore(  //Create OTA update task
    OTAtask,
    "OTAtask",
    3000,
    NULL,
    1,  //Priority 1 (lowest)
    &Task1,
    0);  //Run on Core 0

  while (ota_t == false) {  //wait for OTA service to start
    vTaskDelay(10);
  }
}

void loop() {
  if (debug) Serial.println("Waiting for OTA update to start");
  for (int i = 0; i < 120; i++) {

    if (S3) {
      if (flasher) {
        strip.setPixelColor(0, strip.Color(8, 0, 0));
      } else {
        strip.setPixelColor(0, strip.Color(0, 8, 0));
      }
      strip.show();
    }

#ifdef BUILTIN_LED
    if (S3 == false) digitalWrite(BUILTIN_LED, flasher);  //If the board has an LED then flash it
#endif
    flasher = !flasher;
    vTaskDelay(500);
  }
}
