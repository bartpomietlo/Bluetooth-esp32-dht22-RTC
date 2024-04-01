#include <LiquidCrystal_I2C.h>
#include <virtuabotixRTC.h>
#include <DHT.h>
#include <Preferences.h>
#define DHTPIN 4 //pin sterujący DHT22
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#include "BluetoothSerial.h"
BluetoothSerial SerialBT;
virtuabotixRTC myRTC(16, 17, 18); //16, 17 i 18 pin pod RTC (CLK DAT RST)
LiquidCrystal_I2C lcd(0x27 , 16, 2);  //ustawienie wyświetlacza pod I2C
//I2C: SCL-22, SDA-21 

extern Preferences preferences;


String message = "";
String temp1;
String durat;
char incomingChar;
short int i=0;
short int cz=10;
short int te=10;
float temp,hum;
bool prawda=true;
bool drzemka = preferences.getBool("drzemka");
String budzik = preferences.getString("budzik1");
//przerwanie
void IRAM_ATTR isr(){ //nie można dać preferences.putBool, bo jest to zbyt obciazajace dla uC
  noTone(13);
  prawda = false;
  drzemka = false;
}



void mil(unsigned char ms, unsigned char freq, unsigned char x) {
    tone(13,freq*100,x);
    unsigned int start = millis(); // Get the current time
    while (millis() - start < ms*10) {
      if (digitalRead(23)==HIGH){

        noTone(13);
        if (myRTC.minutes+preferences.getChar("nap")>=60 && myRTC.hours == 23){
           budzik = "alarm " +String("00") + ":0" + String(myRTC.minutes-(60-preferences.getChar("nap")));

        }
        else if (myRTC.minutes+preferences.getChar("nap")>=60){
            budzik = "alarm " +String(myRTC.hours+1) + ":0" + String(myRTC.minutes-(60-preferences.getChar("nap")));
        }
        else {
           budzik = "alarm " +String(myRTC.hours) + ":" + String(myRTC.minutes+preferences.getChar("nap"));
        }
        if (!drzemka){
          SerialBT.println("Ustawiono budzik na godzine: " + budzik.substring(6,11));
          preferences.putString("budzik1", budzik);

          preferences.putBool("drzemka", true);
          prawda = false;
          preferences.putBool("prawda", false);
          drzemka = true;
        }
      }
    }
}


String day_of_week[7]={"pn","wt","sr","cz","pt","sb","nd"}; //dni tygodnia

void temp_hum(){
  hum = dht.readHumidity();
  if ((char)preferences.getChar("jedntemp")=='F'){
    temp= dht.readTemperature()*(9/5.0)+32; //zamiana jednostek na F
  }
  else{
    temp= dht.readTemperature(); //po prostu jednostka C
  }
}
void clearLCDLine(int line) {//czyszczeenie wyświetlacza              
  lcd.setCursor(0,line);
  for(i = 0; i < 20; i++){
      lcd.print(" ");
      }
}
bool wlazl_kotek() {
  mil(50,50,250);
  if (!prawda) {return 0;} //tutaj po każdym dźwięku
  mil(50,40,250);          //sprawdzane jest, czy kliknięty został przycisk
  if (!prawda) {return 0;} //gdyż przycisk wywołuje przerwanie
  mil(50,40,250);          //które zmienia wartość zmiennej "prawda"
  if (!prawda) {return 0;} //w innym przypadku, melodia przestawałaby grać
                           //dopiero po zakończeniu funkcji wlazl_kotek(),
  mil(50,44,250);          //a stosując "if (!prawda) {return 0;}" po każdej 
  if (!prawda) {return 0;} //melodii, przerywamy granie przyciskiem od razu.
  mil(50,34,250);          //jest to użyteczne jeśli mamy w miarę krótką melodię
  if (!prawda) {return 0;}
  mil(50,34,250);
  if (!prawda) {return 0;}

  mil(27,30,200);
  if (!prawda) {return 0;}
  mil(27,40,200);
  if (!prawda) {return 0;}
  mil(100,50,250);
  if (!prawda) {return 0;}

  mil(50,50,250);
  if (!prawda) {return 0;}
  mil(50,40,250);
  if (!prawda) {return 0;}
  mil(50,40,250);
  if (!prawda) {return 0;}

  mil(50,44,250);
  if (!prawda) {return 0;}
  mil(50,34,250);
  if (!prawda) {return 0;}
  mil(50,34,250);
  if (!prawda) {return 0;}

  mil(27,30,200);
  if (!prawda) {return 0;}
  mil(27,40,200);
  if (!prawda) {return 0;}
  mil(100,30,250);
  if (!prawda) {return 0;}
}
void odczyt_bt(String message){
          if (message.substring(0, 4) =="help" || message.substring(0, 5) =="pomoc"){
          
              SerialBT.println("temp - wysw. temperatury");
              SerialBT.println("hum - wysw. wilgotnosci");
              SerialBT.println("data DD MM YYYY - ustawienie daty");
              SerialBT.println("czas GG MM SS - ustawienie godziny");
              SerialBT.println("dtyg [1-7] - ustawienie dnia tygodnia");
              SerialBT.println("wyl - wylaczenie wyswietlacza");
              SerialBT.println("wl - wlaczenie wyswietlacza");
              SerialBT.println("temp f - zmiana jednostki na Fahrenheita");
              SerialBT.println("temp c - zmiana jednostki na Celcjusza");
              SerialBT.println("duration CZAS1 CZAS2 - zmiana czasu wyswietlania sie ekranow (temp/hum, date/time) [s]");
              SerialBT.println("alarm GG MM - ustawienie budzika");
              SerialBT.println("alarm off -wylaczenie budzika");
              SerialBT.println("nap MM - ustawienie dlugosci drzemki (0-60 min)");
              SerialBT.println("help/pomoc - wyswietlenie dostepnych komend");
          }
          else if (message.substring(0, 4) == "data"){
            if ((message.length() - 1) == 15){
              myRTC.updateTime();
              myRTC.setDS1302Time(myRTC.seconds, myRTC.minutes, myRTC.hours, myRTC.dayofweek, message.substring(5, 7).toInt(), message.substring(8, 10).toInt(), message.substring(11,15).toInt());
              SerialBT.println("Date zaktualizowano pomyslnie");
            }
            else {
              SerialBT.println("format: data DD MM YYYY");
            }
          }
          else if (message.substring(0, 4) =="temp"){
            if ((message.length() - 1)==6){
              if (message.substring(0, 6) == "temp f"){
                  preferences.putChar("jedntemp", 'F');
                  SerialBT.println("Zmiana jednostki temperatury na stopnie Fahrenheita");
              }
              else if (message.substring(0, 6) == "temp c"){
                SerialBT.println("Zmiana jednostki temperatury na stopnie Celcjusza");
                preferences.putChar("jedntemp", 'C');

                
              }
            }
            else {
                  temp_hum();
                  temp1 = String(temp, 1) + " '" + (char)preferences.getChar("jedntemp");
                  SerialBT.println(temp1);
              }
          }
    
          else if (message.substring(0, 4) == "czas"){
            if ((message.length() - 1)==13) {
              myRTC.updateTime();
              myRTC.setDS1302Time(message.substring(11,13).toInt(), message.substring(8, 10).toInt(), message.substring(5, 7).toInt(), myRTC.dayofweek,myRTC.dayofmonth,myRTC.month,myRTC.year);
              SerialBT.println("Czas zaktualizowano pomyslnie");
            }
            else{
              SerialBT.println("format: data GG MM SS");
            }
          }

          else if (message.substring(0, 4) == "dtyg") {
            if ((message.length() - 1)==6){
              myRTC.updateTime();
              myRTC.setDS1302Time(myRTC.seconds, myRTC.minutes, myRTC.hours, message.substring(5, 6).toInt() ,myRTC.dayofmonth,myRTC.month,myRTC.year);
              SerialBT.println("Dzien tygodnia zaktualizowano pomyslnie");
            }
            else{
                SerialBT.println("Format: dtyg X (gdzie X: 1-pn, 2-wt, 3-sr, 4-cz, 5-pt, 6-sb, 7-nd)");
            }
          } 
          else if (message.substring(0, 3) =="hum"){
              temp1 = String(hum, 1) + '%';
              SerialBT.println(temp1);}
          else if (message.substring(0, 3) =="nap"){
              if (message.length()-1==6){
                preferences.putChar("nap", message.substring(4,6).toInt());
                if ( message.substring(4,6).toInt()<10){
                  SerialBT.println("Ustawienie dlugosci drzemki na: " + message.substring(5,6)+ " min");
                }
                else{
                  SerialBT.println("Ustawienie dlugosci drzemki na: " + message.substring(4,6)+" min");
                }
              }
              else{
                SerialBT.println("format: nap MM");
              }
          }
          else if (message.substring(0, 3) =="wyl"){
              lcd.noBacklight(); }
          else if (message.substring(0, 2) =="wl"){
              lcd.backlight();}
         
          else if (message.substring(0, 8) =="duration"){
                if (message.length()-1 == 8){
                  SerialBT.println("duration [ekran temp/hum] [ekran time/date] [s]");
                }
                else{
                  durat="";
                  for (i=9;i<(message.length()-1);i++){
                    if (message[i]==' '){
                        SerialBT.println("Ustawiono czas wyswietlania sie ekranu temp/hum na: " + durat+"s");
                        te = durat.toInt();
                        durat="";
                    }
                    else{
                      durat+=message[i];
                    }
                  }
                  SerialBT.println("Ustawiono czas wyswietlania sie ekranu time/date na: " + durat+"s");
                  cz = durat.toInt();
                }
          }
          else if (message.substring(0, 5) =="alarm"){
            if (message.substring(6,9)=="off"){
                  budzik="";
                  preferences.putString("budzik1",budzik);
                  SerialBT.println("dezaktywacja budzika");
            }
            else{
             if ((message.length() - 1) == 11){
                myRTC.updateTime();
                budzik = message;
                preferences.putString("budzik1",budzik);
                prawda = true;
                preferences.putBool("prawda", true);
                SerialBT.println("Ustawiono cykliczny budzik na godzine: " + message.substring(6,11));
                SerialBT.println("(dezaktywacja 'budzik off')");
             }
             else {
                SerialBT.println("alarm HH MM");
             }
          }
          }
          else {
            SerialBT.println("nieprawidlowa komenda!");
          }
      
}

void bt() {
      preferences.putBool("prawda",prawda); //tutaj przy każdym wywołaniu funckji bt()
      if (SerialBT.available()){            // wpisywana jest wartość zmiennej "prawda"
                                            // do pamięci flash. Sprawdza to czy przerwanie
        incomingChar = SerialBT.read();     // zostało wywołane, gdyż nie można wpisywać
                                            // do pamięci podczas przerwania, bo jest
        if (incomingChar != '\n'){          // to zbyt długa operacja dla uC i powoduje
          message += incomingChar;          // to restartowanie.
        }
        else {
            odczyt_bt(message);
            message="";
        }
    } 
   myRTC.updateTime();
   budzik=preferences.getString("budzik1");
   if ((preferences.getBool("prawda") || drzemka) && budzik.substring(6, 8).toInt()==myRTC.hours && budzik.substring(9, 11).toInt()==myRTC.minutes)
   {                     //w linijce 278, gdybym chciał korzystać ze zsmiennej prawda, działałoby to tylko wtedy, gdy uC nie był resetowany.
    if (drzemka){        //dlatego zależało nam aby operować na zmiennych z pamięci flash zamiast z programu 
      prawda = true;
      preferences.putBool("prawda", true);
      }
      wlazl_kotek();
    }
}


void display_temp_hum(){//wyśweitlanie jednego wyświetlacza
  clearLCDLine(0);
  clearLCDLine(1);
  i=0;
  while (i<te*23){
    temp_hum();
    bt();
    lcd.setCursor(0, 0);
    lcd.print("wilg: ");
    lcd.print(hum);
    lcd.print('%');
    lcd.setCursor(0, 1);
    lcd.print("temp: ");
    lcd.print(temp);
    lcd.print((char)223);
    lcd.print((char)preferences.getChar("jedntemp"));
    i++;
  }
}
void display_time(){ //wyświetlanie drugiego
  clearLCDLine(0);
  clearLCDLine(1);
  i=0;
  while (i<cz*28){
    bt();
    myRTC.updateTime();
    lcd.setCursor(0, 0);
    if (myRTC.dayofmonth<10){
      lcd.print(0);
    }
    lcd.print(myRTC.dayofmonth);
    lcd.print('.');
    if (myRTC.month<10){
      lcd.print(0);
    }
    lcd.print(myRTC.month);
    lcd.print('.');
    lcd.print(myRTC.year);
    lcd.print(", ");
    lcd.print(day_of_week[myRTC.dayofweek-1]);
    lcd.setCursor(0, 1);
    if (myRTC.hours<10){
      lcd.print(0);
    }
    lcd.print(myRTC.hours);
    lcd.print(':');
    if (myRTC.minutes<10){
      lcd.print(0);
    }
    lcd.print(myRTC.minutes);
    lcd.print(':');
    if (myRTC.seconds<10){
      lcd.print(0);
    }
    lcd.print(myRTC.seconds);
    i++;
  }
}
