#include "31.h"
#include <Preferences.h>

Preferences preferences;
void setup(){
  SerialBT.begin("Home Control");
  lcd.init();          
  lcd.backlight();
  dht.begin();
  attachInterrupt(digitalPinToInterrupt(33), isr, FALLING); //definicja przerwania
  pinMode(23,INPUT); //żółty przycisk
  pinMode(13, OUTPUT); //glosnik
  preferences.begin("jedntemp", false); //funkncje włączające zmienną.
  preferences.begin("budzik1", false);  //dla false jest read/write
  preferences.begin("drzemka", false); //dla true jest tylko read
  preferences.begin("prawda", false);
  preferences.begin("nap", false);
  if (!preferences.isKey("jedntemp")) { //ustawnianie wartości domyślnych
    preferences.putChar("jedntemp", 'C');
  }
  if (!preferences.isKey("drzemka")) {
    preferences.putBool("drzemka", false);
  }
  if (!preferences.isKey("budzik")) {
    preferences.putString("budzik", "");
  }
  if (!preferences.isKey("prawda")) {
    preferences.putBool("prawda", false);
  }
    if (!preferences.isKey("nap")) {
    preferences.putChar("nap", 5);
  }
}

void loop(){ //pętla główna
  display_temp_hum();
  display_time();
}