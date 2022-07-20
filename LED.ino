bool LED_INIT()
{
  LED.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  LED.show();            // Turn OFF all pixels ASAP
  LED.setPixelColor(0, 255, 0, 0);  //Nummer, R, G, B
  LED.setPixelColor(1, 255, 0, 0); //Nummer, R, G, B
  LED.show();
  delay(200);

  LED.setPixelColor(0, 0, 255, 0);  //Nummer, R, G, B
  LED.setPixelColor(1, 0, 255, 0); //Nummer, R, G, B
  LED.show();
  delay(200);

  LED.setPixelColor(0, 0, 0, 255);  //Nummer, R, G, B
  LED.setPixelColor(1, 0, 0, 255); //Nummer, R, G, B
  LED.show();
  delay(200);

  LED.setPixelColor(0, 0, 0, 0);  //Nummer, R, G, B
  LED.setPixelColor(1, 0, 0, 0); //Nummer, R, G, B
  LED.show();

  return true;
}

bool setFaultLED (int r, int g, int b)
{
  LED.setPixelColor(0, r, g, b);
  LED.show();
  return true;
}

bool FaultLEDoff()
{
  LED.setPixelColor(0, 0, 0, 0);
  LED.show();
  return true;
}

bool setStateLED (int r, int g, int b)
{
  LED.setPixelColor(1, r, g, b);
  LED.show();
  return true;
}

bool StateLEDoff()
{
  LED.setPixelColor(1, 0, 0, 0);
  LED.show();
  return true;
}

bool HGBon()
{
  digitalWrite(HGB_ON, HIGH);
  return true;
}

bool HGBoff()
{
  digitalWrite(HGB_ON, LOW);
  return true;
}

bool HGBTimer()
{
  if (displaytimer)
  {
    displaytime = millis();
    HGBon();
   // DOG.sleep(false);
    displaytimer = false;
  }

  if ((millis()-displaytime)>displaytimeout)
  {
   // DOG.sleep(true);
    HGBoff();
  }
  return true;
}
