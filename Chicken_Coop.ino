//LED
#include <Adafruit_NeoPixel.h>

//Encoder
#include <Encoder.h>

//Display
#include <DogGraphicDisplay.h>
#include "ubuntumono_b_16.h" //Schriftart 16p
#include "ubuntumono_b_8.h" //Schriftart 8p
#include "font_6x8.h"

//RTC
#include <DS3231.h>
#include <Wire.h>

//Arduino
#include <FlashStorage.h>
#include <ArduinoLowPower.h>

//Stromsensor
#include <Adafruit_INA219.h>

//ADC
#include<ADS1115_WE.h>

//DAC
#include <stdio.h>
#include <stdlib.h>
#include "MCP4725.h"

//RTC EEPROM
#include <AT24Cxx.h>
//  eep.update(address, val); address = speicherbank, value = wert (byte!)
//  value = eep.read(address);

//SD Update
#include <SDU.h>



//Defines
#define HGB_ON 0          //Ansteuerung Display Hintergrundbeleuchtung HIGH Aktiv -OUT PWM
#define PH_ON 1           //PWM Ansteuerung Motorphase HIGH Aktiv -OUT PWM
#define CNTRL_PH1 2       //Umschaltung Anschluss X1-1 HIGH Aktiv -OUT
#define CNTRL_PH2 3       //Umschaltung Anschluss X1-2 HIGH Aktiv -OUT
#define ENDSTOP_TOP 4     //Anschluss X4 HIGH Aktiv -IN
#define ENDSTOP_BOTTOM 5  //Anschluss X5 HIGH Aktiv -IN
#define ENCODER_2 6       //Drehencoder -IN LIB
#define ENCODER_1 7       //Drehencoder -IN LIB
#define SPI_MOSI 8        //SPI BUS MOSI LIB
#define SPI_SCK 9         //SPI BUS SCK LIB
#define LCD_CD 10         //Umschaltung Command<->Data Display -OUT LIB
#define I2C_SDA 11        //I2C BUS Data LIB
#define I2C_SCL 12        //I2C BUS Clock LIB
#define LCD_RST 13        //LCD Reset -OUT LIB
#define LCD_CS 14         //SPI BUS Chip Select LCD -OUT LIB
#define VCC_CTRL A0       //Schalten der Endstop-Versorgungsspannung HIGH Aktiv -OUT
#define LDR A1            //Erkennung Helligkeit über LDR -IN ANALOG
#define RTC_INT A2        //Programmierbarer Interrupt der RTC -IN
#define LED_PIN A3        //WS2812 Ausgang -OUT LIB
#define NOTAUS A4         //NOTAUS Eingang LOW Aktiv -IN
#define USER_BTN A5       //User Taster Eingang LOW Aktiv -IN
#define ENCODER_BTN A6    //Button des Drehencoders LOW Aktiv Interner Pullup -IN



//Board LED
#define LED_COUNT 2 //Anzahl der angeschlossenen LEDS, default = 2
Adafruit_NeoPixel LED(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

//Encoder
Encoder Drehencoder(6, 7);
long EncodersetPosition = 0;
long EncodernewPosition = 0;
long EncoderoldPosition = -999;
bool EncoderbtnState = false;
bool EncoderbuttonToggle = false;
bool EncoderbtnLastState = false;
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

//Button
bool btnState = false;

//Endstops
bool EndstopTop = false;
bool EndstopBottom = false;
bool FallingEdge;
bool lastcheck;

//Poti
int PotiValue = 0;

//LDR
int LDRValue = 0;

//UBAT
int UBATValue = 0;

//NOTAUS
bool NOTAUSstate = false;

//Display
DogGraphicDisplay DOG;
bool displaytimer = true;
unsigned long displaytimeout = 10000; //Nachlaufzeit für die HGB
unsigned long displaytime = 0;

//RTC
DS3231 Uhr;
String CompileTime = __TIME__; //Compile Time / String / "hh:mm:ss"
int CompileHour = 0;
int CompileMinute = 0;
int CompileSecond = 0;

//Temperatur
int temperatur = 0;
int tempcal = -3;

//Flash
typedef struct {
  boolean valid;
  int stundehoch;
  int minutehoch;
  int stunderunter;
  int minuterunter;
  int ontime;
} Daten;
FlashStorage(Flashspeicher, Daten); //Speicherplatz für "Daten" in "Flashspeicher" freihalten
Daten Flashdaten; //Objekt "Flashdaten" aus Klasse "Daten" erzeugen

//INA
Adafruit_INA219 ina219;
float current_mA = 0;

//ADC
#define I2C_ADDRESS 0x48
ADS1115_WE adc = ADS1115_WE(I2C_ADDRESS);
float UBAT = 0; //Channel A0
float USOLAR = 0; //Channel A1
float SENSITIVITY = 0; //Channel A2
//Channel A3 des ADC ist frei

//DAC
MCP4725 MCP(0x60);  // 0x62 or 0x63
long DAC_OUTPUT = 0;

//Zeiteinstellung
int zwischenspeicherStunden = 0;
int zwischenspeicherMinuten = 0;
int zwischenspeicherOnTime  = 0;
int minuteset = 0;
int stundeset = 0;
int zwischenspeicherUhrzeitStunden = 0;
int zwischenspeicherUhrzeitMinuten = 0;

//Relais
bool relaison;
unsigned long currentmillis = 0;

//Loop-Variablen
int minuteold = 0;
int menuposition = 0;
bool menuentered;

//Handbetrieb
int handstate = 0;
bool pressed = false;
bool handbetrieb = false;

//EEPROM
#define i2c_address 0x57 //RTC EEPROM Adresse
#define EEPROM_SIZE 32 //kbyte


void setup() {
  PIN_INIT();
  LED_INIT();
  SERIAL_INIT();
  DISPLAY_INIT();
  I2C_INIT();
  INA_INIT();
  RTC_INIT();
  ADC_INIT();
  DAC_INIT();
  EEPROM_INIT();
  FlashRead();
  DOG.createCanvas(128, 64, 0, 0);

  setEncoderPosition(menuposition);
}

//Endlosschleife
void loop() {
  while (!ReadEncoder()) {}
  while (!readEncoderBtn()) {}
  while (!readBtn()) {}
  while (!readNOTAUS()) {}
  while (!readEndstopTop()) {}
  while (!readEndstopBottom()) {}
  while (!readPotiValue()) {}
  while (!readLDRValue()) {}
  while (!readUbat()) {}
  while (!readINA()) {}
  while (!HGBTimer()) {}
  while (!readADC()) {}
  while (!checkEdge()) {}

  Menu();
  RelaisOff(Flashdaten.ontime); //Relais nach Erreichen der Zeit abschalten (ms, unsigned long)
  AlarmOff(210); //Ausgänge bei Erreichen des Max. Stroms abschalten (mA, float)
  Solar(); //Laderegler
  Handbetrieb(); //Ablauf Handbetrieb
  UndervoltageCheck();

  if (minuteold != Uhr.getMinute()) { //"Programmdurchlauf" minütlich ausführen
    Programmdurchlauf();
    minuteold = Uhr.getMinute();
  }


}


//Wird minütlich ausgeführt
bool Programmdurchlauf()
{

  bool a, b;

  String hour = String(Uhr.getHour(a, b));
  String minute = String(Uhr.getMinute());
  String Null = "0";
  if (Uhr.getHour(a, b) < 10)
  {
    hour = Null + hour;
  }
  if (Uhr.getMinute() < 10)
  {
    minute = Null + minute;
  }
  String second = String(Uhr.getSecond());
  float UBATFloat = float(UBATValue) * 0.1;
  String UBAT = String(UBATFloat, 1);


  DOG.clear();

  DOG.drawLine(0, 17, 127, 17);
  DOG.string(0, 0, UBUNTUMONO_B_16, (hour + ":" + minute).c_str() , ALIGN_LEFT);
  DOG.string(0, 0, UBUNTUMONO_B_16, (UBAT + "V").c_str(), ALIGN_RIGHT);

  TimeCheck();

  return true;
}


bool Menu ()
{
  if (!EncoderbuttonToggle)
  {
    menuposition = EncodernewPosition;

    if (menuposition < 9 && menuposition >= 0)
    {
      switch (menuposition)
      {
        case 0:
        case 1:
          {
            temperatur = (Uhr.getTemperature() + tempcal);
            String sTemperatur = String(temperatur);
            if (!UndervoltageCheck())
            {
              DOG.string(0, 4, UBUNTUMONO_B_16, ("     " + sTemperatur + " C     ").c_str()  , ALIGN_CENTER);
              DOG.string(0, 6, UBUNTUMONO_B_16, "   Temperatur   ", ALIGN_CENTER);

            }
            else
            {
              DOG.string(0, 6, UBUNTUMONO_B_16, "Akku laden!"  , ALIGN_CENTER);
              DOG.string(0, 4, UBUNTUMONO_B_16, "   WARNUNG!   ", ALIGN_CENTER);

            }

          }
          break;
        case 2:
        case 3:
          { String NullStunde;
            String NullMinute;
            String stunderunter = String(Flashdaten.stunderunter);
            String minuterunter = String(Flashdaten.minuterunter);
            if (Flashdaten.stunderunter < 10)
            {
              NullStunde = "0";
            }
            else
            {
              NullStunde = "";
            }
            if (Flashdaten.minuterunter < 10)
            {
              NullMinute = "0";
            }
            else
            {
              NullMinute = "";
            }
            DOG.string(0, 4, UBUNTUMONO_B_16, ("     " + NullStunde + stunderunter + ":" + NullMinute + minuterunter + " Uhr" + "     ").c_str()  , ALIGN_CENTER);
            DOG.string(0, 6, UBUNTUMONO_B_16, "   Schliessen   ", ALIGN_CENTER);
          }
          break;
        case 4:
        case 5:
          {
            String NullStunde;
            String NullMinute;
            String stundehoch = String(Flashdaten.stundehoch);
            String minutehoch = String(Flashdaten.minutehoch);
            if (Flashdaten.stundehoch < 10)
            {
              NullStunde = "0";
            }
            else
            {
              NullStunde = "";
            }
            if (Flashdaten.minutehoch < 10)
            {
              NullMinute = "0";
            }
            else
            {
              NullMinute = "";
            }
            DOG.string(0, 4, UBUNTUMONO_B_16, ("     " + NullStunde + stundehoch + ":" + NullMinute + minutehoch + " Uhr" + "     ").c_str()  , ALIGN_CENTER);
            DOG.string(0, 6, UBUNTUMONO_B_16, "   Oeffnen   ", ALIGN_CENTER);
          }
          break;
        case 6:
        case 7:
          {
            bool a, b;
            String hour = String(Uhr.getHour(a, b));
            String minute = String(Uhr.getMinute());
            String second = String(Uhr.getSecond());
            String Null = "0";

            if (Uhr.getHour(a, b) < 10)
            {
              hour = Null + hour;
            }
            if (Uhr.getMinute() < 10)
            {
              minute = Null + minute;
            }
            if (Uhr.getSecond() < 10)
            {
              second = Null + second;
            }
            DOG.string(0, 4, UBUNTUMONO_B_16, ("" + hour + ":" + minute + ":" + second + "  Uhr" + "     ").c_str() , ALIGN_LEFT);
            DOG.string(0, 6, UBUNTUMONO_B_16, "   Zeit   ", ALIGN_CENTER);
          }
          break;
        case 8:
        case 9:
          {
            String laufzeit = "     " + String(Flashdaten.ontime) + " s/10" + "     ";
            DOG.string(0, 4, UBUNTUMONO_B_16, laufzeit.c_str(), ALIGN_CENTER);
            DOG.string(0, 6, UBUNTUMONO_B_16, "  Fahrdauer  ", ALIGN_CENTER);
          }
          break;
        default:

          DOG.string(0, 4, UBUNTUMONO_B_16, "   Default   ", ALIGN_CENTER);
          //dürfte nie eintreten!
          break;
      }
    }

    else if (menuposition > 9)
    {
      menuposition = 0;
      setEncoderPosition(menuposition);

    }
    else if (menuposition < 0)
    {
      menuposition = 8;
      setEncoderPosition(menuposition);

    }
  }


  else {
    DOG.string(0, 6, UBUNTUMONO_B_16, "   Set aktiv   ", ALIGN_CENTER);
    if (menuposition < 9 && menuposition >= 0)
    {

      switch (menuposition)
      {
        case 0:
        case 1:
          {
            //Platzhalter
          }
          break;
        case 2:
        case 3:
          {
            String NullStunde;
            String NullMinute;
            if (EncodernewPosition < 60 && EncodernewPosition >= 0)
            { Flashdaten.minuterunter = EncodernewPosition;
            }
            else if (EncodernewPosition > 59)
            {
              setEncoderPosition(0);
              if (Flashdaten.stunderunter < 23 && Flashdaten.stunderunter >= 0)
              {
                Flashdaten.stunderunter = Flashdaten.stunderunter + 1;
              }
              else
              {
                Flashdaten.stunderunter = 0;
              }
            }
            else if (EncodernewPosition < 0)
            {
              setEncoderPosition(58);
              if (Flashdaten.stunderunter < 24 && Flashdaten.stunderunter > 0)
              {
                Flashdaten.stunderunter = Flashdaten.stunderunter - 1;

              }
              else
              {
                Flashdaten.stunderunter = 23;
              }
            }
            if (Flashdaten.stunderunter < 10)
            {
              NullStunde = "0";
            }
            else
            {
              NullStunde = "";
            }
            if (EncodernewPosition < 10)
            {
              NullMinute = "0";
            }
            else
            {
              NullMinute = "";
            }
            String Value = "   " + NullStunde + String(Flashdaten.stunderunter) + ":" + NullMinute + String(EncodernewPosition) + " Uhr" + "   ";

            DOG.string(0, 4, UBUNTUMONO_B_16, Value.c_str() , ALIGN_CENTER);

          }

          break;
        case 4:
        case 5:
          {
            String NullStunde;
            String NullMinute;
            if (EncodernewPosition < 60 && EncodernewPosition >= 0)
            { Flashdaten.minutehoch = EncodernewPosition;
            }
            else if (EncodernewPosition > 59)
            {
              setEncoderPosition(0);
              if (Flashdaten.stundehoch < 23 && Flashdaten.stundehoch >= 0)
              {
                Flashdaten.stundehoch = Flashdaten.stundehoch + 1;
              }
              else
              {
                Flashdaten.stundehoch = 0;
              }
            }
            else if (EncodernewPosition < 0)
            {
              setEncoderPosition(58);
              if (Flashdaten.stundehoch < 24 && Flashdaten.stundehoch > 0)
              {
                Flashdaten.stundehoch = Flashdaten.stundehoch - 1;
              }
              else
              {
                Flashdaten.stundehoch = 23;
              }
            }
            if (Flashdaten.stundehoch < 10)
            {
              NullStunde = "0";
            }
            else
            {
              NullStunde = "";
            }
            if (EncodernewPosition < 10)
            {
              NullMinute = "0";
            }
            else
            {
              NullMinute = "";
            }
            String Value = "   " + NullStunde + String(Flashdaten.stundehoch) + ":" + NullMinute + String(EncodernewPosition) + " Uhr" + "   ";

            DOG.string(0, 4, UBUNTUMONO_B_16, Value.c_str() , ALIGN_CENTER);


          }
          break;
        case 6:
        case 7:
          {
            String NullStunde;
            String NullMinute;

            if (EncodernewPosition < 60 && EncodernewPosition >= 0)
            { minuteset = EncodernewPosition;
            }
            else if (EncodernewPosition > 59)
            {
              setEncoderPosition(0);
              if (stundeset < 23 && stundeset >= 0)
              {
                stundeset = stundeset + 1;
              }
              else
              {
                stundeset = 0;
              }
            }
            else if (EncodernewPosition < 0)
            {
              setEncoderPosition(58);
              if (stundeset < 24 && stundeset > 0)
              {
                stundeset = stundeset - 1;

              }
              else
              {
                stundeset = 23;
              }
            }

            if (stundeset < 10)
            {
              NullStunde = "0";
            }
            else
            {
              NullStunde = "";
            }
            if (EncodernewPosition < 10)
            {
              NullMinute = "0";
            }
            else
            {
              NullMinute = "";
            }
            String Value = "  " + NullStunde + String(stundeset) + ":" + NullMinute + String(EncodernewPosition) + " Uhr" + "   ";

            DOG.string(0, 4, UBUNTUMONO_B_16, Value.c_str() , ALIGN_CENTER);
          }
          break;
        case 8:
        case 9: //Fahrtzeiteinstellung
          {
            Flashdaten.ontime = EncodernewPosition;




            String laufzeit = "     " + String(Flashdaten.ontime) + " s/10" + "     ";


            DOG.string(0, 4, UBUNTUMONO_B_16, laufzeit.c_str(), ALIGN_CENTER);

          }
          break;
        default:

          DOG.string(0, 4, UBUNTUMONO_B_16, "Default", ALIGN_CENTER);
          //dürfte nie eintreten!
          break;
      }

    }
    else if (menuposition > 9)
    {
      menuposition = 0;
      setEncoderPosition(menuposition);

    }
    else if (menuposition < 0)
    {
      menuposition = 8;
      setEncoderPosition(menuposition);

    }
  }

}


bool TimeCheck()
{
  bool a, b;

  int hour = Uhr.getHour(a, b);
  int minute = Uhr.getMinute();


  if (hour == Flashdaten.stundehoch && minute == Flashdaten.minutehoch)
  {
    handbetrieb = false;
    steuerunghoch();
  }

  if (hour == Flashdaten.stunderunter && minute == Flashdaten.minuterunter)
  {
    handbetrieb = false;
    steuerungrunter();
  }

}

bool Save()
{
  Flashspeicher.write(Flashdaten);
}

bool LoadValue()
{
  switch (menuposition)
  {
    case 0: //Status
    case 1:
      {


      }
      break;
    case 2: //Schließzeit
    case 3:
      {
        zwischenspeicherStunden = Flashdaten.stunderunter;
        zwischenspeicherMinuten = Flashdaten.minuterunter;
        setEncoderPosition(zwischenspeicherMinuten);
      }
      break;
    case 4: //Öffnenzeit
    case 5:
      {
        zwischenspeicherStunden = Flashdaten.stundehoch;
        zwischenspeicherMinuten = Flashdaten.minutehoch;
        setEncoderPosition(zwischenspeicherMinuten);
      }
      break;
    case 6: //Uhrzeit
    case 7:
      {
        bool a;
        bool b;
        zwischenspeicherUhrzeitStunden =   Uhr.getHour(a, b);
        zwischenspeicherUhrzeitMinuten = Uhr.getMinute();
        setEncoderPosition(zwischenspeicherUhrzeitMinuten);
      }
      break;
    case 8:
    case 9: //Fahrtzeit
      zwischenspeicherOnTime = Flashdaten.ontime;
      setEncoderPosition(zwischenspeicherOnTime);
      break;
    default:
      //dürfte nie eintreten!
      break;
  }
}

bool Handbetrieb ()
{
  static int up_down = 0;

  if (btnState && !handstate)
  {
    Serial.println(up_down);
    up_down++;
    if (up_down > 3)
      up_down = 0;
    switch (up_down)
    {
      case 0:
        HandRelaisOff();
        break;

      case 1:
        handbetrieb = true;
        steuerunghoch();
        break;

      case 2:
        HandRelaisOff();
        break;

      case 3:
        handbetrieb = true;
        steuerungrunter();
        break;
      default:

        break;
    }
  }

  handstate = btnState;

  //pressed = false;
  //Serial.println(handstate);
  return true;


}

bool UndervoltageCheck ()
{
  if (UBATValue < 110)
  {
    setFaultLED(10, 0, 0);
    return true;
  }
  else if (UBATValue > 135)
  {
    setFaultLED(0, 10, 0);
    return false;
  }
  else
  {
    setFaultLED(0, 0, 0);
    return false;
  }
}

bool checkEdge()
{

  if (!EndstopTop && lastcheck)
  {
    FallingEdge = true;
  }
  else
  {
    FallingEdge = false;
  }
  lastcheck = EndstopTop;

  return true;
}
