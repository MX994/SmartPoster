#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <FastLED.h>
#include <FastLED_NeoMatrix.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include "Configuration.h"
#include "main.h"

AsyncWebServer server(80);
String buf = "";

int x = matrix->width(), y = 0;
int strLen = -6 * buf.length(), state = -1, color = 0xFFFFFF;
bool imageChanged = false;

void changeState(int newState) {
  if (newState != state) {
    state = newState;
    matrix->fillScreen(0);
    switch (state) {
      case STATE_TEXT_DISPLAY:
        x = matrix->width();
        break;
      case STATE_IMAGE_DISPLAY:
        imageChanged = true;
        break;
    } 
  }
}

void setup() {
  Serial.begin(9600);
  if (!SPIFFS.begin()) {
     Serial.println("SPIFFS failed.");
     return;
  }

  WiFi.softAP(ssid, password);
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  matrix->begin();
  matrix->setTextWrap(false);
  matrix->setRemapFunction(remapXY);
  matrix->setBrightness(8);
  matrix->setTextColor(color);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", "text/html");
  });

  server.on("/textdisplay.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/textdisplay.html", "text/html");
    changeState(STATE_TEXT_DISPLAY);
  });
 
  server.on("/imagedisplay.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/imagedisplay.html", "text/html");
    changeState(STATE_IMAGE_DISPLAY);
  });

  server.on("/streaming.html", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/streaming.html", "text/html");
    changeState(STATE_STREAM);
  });

  server.on("/updateTextDisplay", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("y_offset")) {
      Serial.println(request->getParam("y_offset")->value().toInt());
      y = request->getParam("y_offset")->value().toInt();
    }
    if (request->hasParam("message")) {
      buf = request->getParam("message")->value();
      strLen = -6 * buf.length();
    }
    if (request->hasParam("color")) {
      color = strtoul(request->getParam("color")->value().substring(2).c_str(), NULL, 16);
    }
    changeState(STATE_TEXT_DISPLAY);
    request->send(SPIFFS, "/textdisplay.html", "text/html");
  });

  server.begin();
}

void loop() {
  switch (state) {
    case STATE_TEXT_DISPLAY: {
      matrix->fillScreen(0);
      matrix->setCursor(x, y); 
      matrix->print(buf);
      if (--x < strLen) {
        x = matrix->width();
        matrix->setTextColor(color);
      }
      matrix->show();
      delay(25); 
      break;
    }
    case STATE_IMAGE_DISPLAY: {
      File file = SPIFFS.open("/images/heart.bin", "r");
      unsigned int magic = file.read() | (file.read() << 0x8) | (file.read() << 0x10) | (file.read() << 0x18);
      if (magic != 0x49425241) {
        return;
      }
      matrix->fillScreen(0);
      uint16_t width = file.read() | (file.read() << 0x8), height = file.read() | (file.read() << 0x8);
      for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
          uint32_t clr = file.read() | (file.read() << 0x8) | (file.read() << 0x10);
          matrix->drawPixel(x, y, clr);
        }
      }
      matrix->show();
      file.close();
      break;
    }
    case STATE_STREAM: {
      break;
    }
  }
}
