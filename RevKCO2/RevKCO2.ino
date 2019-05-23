// Example application

#include <Wire.h>
#include <ESPRevK.h>
#include "Adafruit_CCS811.h"

Adafruit_CCS811 ccs;

#define app_settings  \

// TODO ref temp, drive mode, etc

#define s(n) const char *n=NULL
  app_settings
#undef s

  ESPRevK revk(__FILE__, __DATE__ " " __TIME__);

  const char* app_setting(const char *tag, const byte *value, size_t len)
  { // Called for settings retrieved from EEPROM, return PSTR for tag if setting is OK
#define s(n) do{const char *t=PSTR(#n);if(!strcmp_P(tag,t)){n=(const char *)value;return t;}}while(0)
    app_settings
#undef s
    return NULL; // Failed
  }

  boolean app_command(const char*tag, const byte *message, size_t len)
  { // Called for incoming MQTT messages, return true if message is OK
    return false; // Failed
  }

  void setup()
  { // Your set up here as usual
    Wire.begin(0, 2);
    ccs.begin();
    while (!ccs.available());
    // TODO DHT11 temp/humidity?
    float temp = ccs.calculateTemperature();
    ccs.setTempOffset(temp - 21.0);
    ccs.setEnvironmentalData(35, 21.0);
    ccs.setDriveMode(CCS811_DRIVE_MODE_60SEC);
  }

  void loop()
  {
    revk.loop();
    unsigned long now = millis();
    static unsigned long next = 0;
    if ((int)(next - now) <= 0)
    {
      next = now + 60000;
      if (ccs.available()) {
        if (!ccs.readData()) {
          revk.info(F("Env"), F("CO2 %dppm TVOC %dppb Temp %f"), ccs.geteCO2(), ccs.getTVOC(), ccs.calculateTemperature());
        }
        else revk.error(F("Env"), F("Cannot read data"));
      }
      else revk.error(F("Env"), F("Not available"));
    }
  }
