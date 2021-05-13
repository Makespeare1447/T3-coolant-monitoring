/*
   (C) 2018 Thorsten Gurzan - beelogger.de

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

// beelogger.de - Arduino Datenlogger für Imker
// Erläuterungen dieses Programmcodes unter https://beelogger.de


#define ONE_WIRE_BUS 2
#define Anzahl_Sensoren_DS18B20 2 // Anzahl der angeschlossenen Sensoren - Mögliche Werte: '0','1','2',... 

#include <Time.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TFT.h>
#include <SPI.h>

#define cs 10
#define dc 9
#define rst 8
#define DS18B20_Aufloesung 12
DeviceAddress DS18B20_Adressen;

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature myDS18B20(&oneWire);
TFT TFTscreen = TFT(cs, dc, rst);



const float No_Val = 999.99;
float temperature[Anzahl_Sensoren_DS18B20];
char temp1_printout[6];
char temp2_printout[6];
String temp1_string;
String temp2_string;
unsigned int runtime = 0;
unsigned int seconds = 0;
unsigned int minutes = 0;
unsigned int hours = 0;

String temp1_string_old;
String temp2_string_old;


void setup(void) {
  TFTscreen.begin();
  TFTscreen.background(0, 0, 0);
  TFTscreen.stroke(255, 255, 255);
  TFTscreen.setTextSize(2);
  TFTscreen.text("starting up...", 0, 0);

  Serial.begin(9600);
  Serial.println("DS18B20 Test");
  Serial.println();
  delay(1000);

  Serial.print("Anzahl aktivierter Sensoren: ");
  Serial.println(Anzahl_Sensoren_DS18B20);

  if ((Anzahl_Sensoren_DS18B20 > 0)) {
    myDS18B20.begin();
    Serial.print("Anzahl angeschlossener Sensoren: ");
    Serial.println(myDS18B20.getDeviceCount(), DEC);
    Serial.println();

    for (byte i = 0 ; i < myDS18B20.getDeviceCount(); i++) {
      if (myDS18B20.getAddress(DS18B20_Adressen, i)) {
        myDS18B20.setResolution(DS18B20_Adressen, DS18B20_Aufloesung);
      }
    }
  }

}


void loop(void) {
  runtime = millis()*1000;
  hours = runtime;


  if ((Anzahl_Sensoren_DS18B20 > 0)) {
    myDS18B20.requestTemperatures();

    for (byte i = 0 ; i < Anzahl_Sensoren_DS18B20; i++) {
      if (i < myDS18B20.getDeviceCount()) {
        temperature[i] = myDS18B20.getTempCByIndex(i);
        if (temperature[i] == DEVICE_DISCONNECTED_C) {
          temperature[i] = No_Val;
          Serial.println("Fehler");
        }
        else {
          Serial.print(temperature[i]);
          Serial.println(" 'C");
        }
      }
    }
  }
  Serial.println();

  temp1_string = String(temperature[0]);
  temp1_string.toCharArray(temp1_printout, 6);
  temp2_string = String(temperature[1]);
  temp2_string.toCharArray(temp2_printout, 6);


  if (temp1_string != temp1_string_old || temp2_string != temp2_string_old) {
    TFTscreen.background(0, 0, 0);
    TFTscreen.text("Front: ", 0, 0);
    TFTscreen.text("Heck: ", 0, 20);
    TFTscreen.text(temp1_printout, 70, 0);
    TFTscreen.text(temp2_printout, 70, 20);
    
   
    TFTscreen.text("\x93", 0, 90);
  }


  temp1_string_old = temp1_string;
  temp2_string_old = temp2_string;

  delay(1000);
}
