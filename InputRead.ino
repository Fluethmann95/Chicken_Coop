bool ReadEncoder() {
  EncodernewPosition = Drehencoder.read();
  //  Serial.println(EncodernewPosition);
  if (EncodernewPosition != EncoderoldPosition) {
    EncoderoldPosition = EncodernewPosition;
    displaytimer = true;
  }
  return true;
}

bool setEncoderPosition(long setEncoderPosition) {
  Drehencoder.write(setEncoderPosition);
  //  Serial.println(EncodernewPosition);
  return true;
}

bool readEncoderBtn() {
  int savemenuposition = 0;
  EncoderbtnState = !digitalRead(ENCODER_BTN);  //EncoderbtnState ist true wenn Taste gedrückt
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (EncoderbtnState && !EncoderbtnLastState)  //wenn Button Gedrückt
    {
      if (EncoderbuttonToggle)  //Menü wird verlassen
      {
        if (menuposition == 6 || menuposition == 7) {
          Uhr.setHour(stundeset);
          Uhr.setMinute(minuteset);
          setEncoderPosition(savemenuposition);
          Serial.println("Zeit gestellt");

        } else {
          setEncoderPosition(savemenuposition);
          Save();
          //Serial.println("Speichern");
        }
      }
      if (!EncoderbuttonToggle)  //Menü wird geöffnet
      {
        savemenuposition = menuposition;
        LoadValue();
      }
      EncoderbuttonToggle = !EncoderbuttonToggle;
      lastDebounceTime = millis();
      displaytimer = true;
    }
    EncoderbtnLastState = EncoderbtnState;
  }

  return true;
}

bool readBtn() {
  btnState = !digitalRead(USER_BTN);
  return true;
}

bool readEndstopTop() {
  EndstopTop = digitalRead(ENDSTOP_TOP);

  return true;
}

bool readEndstopBottom() {
  EndstopBottom = digitalRead(ENDSTOP_BOTTOM);
  return true;
}

bool readNOTAUS() {
  NOTAUSstate = !digitalRead(NOTAUS);
  return true;
}

bool readPotiValue() {
  int WerteSumme = 0;
  int AnzahlSamples = 32;
  for (int i = 0; i < AnzahlSamples; i++) {
    WerteSumme = WerteSumme + SENSITIVITY;
  }
  PotiValue = WerteSumme / AnzahlSamples;

  return true;
}

bool readLDRValue() {
  int WerteSumme = 0;
  int unmappedLDRValue = 0;
  int AnzahlSamples = 120;
  for (int i = 0; i < AnzahlSamples; i++) {
    WerteSumme = WerteSumme + analogRead(LDR);
  }
  unmappedLDRValue = WerteSumme / AnzahlSamples;
  LDRValue = map(unmappedLDRValue, 500, 1023, 0, 1023);
  return true;
}

bool readUbat() {
  float WerteSumme = 0;
  float unmappedUbatValue = 0;
  int AnzahlSamples = 50;
  for (int i = 0; i < AnzahlSamples; i++) {
    WerteSumme = WerteSumme + UBAT;
  }
  unmappedUbatValue = WerteSumme / AnzahlSamples;
  UBATValue = (unmappedUbatValue - 0) * (250 - 0) / (2.28 - 0) + 0;
  return true;
}

bool readINA() {



  current_mA = ina219.getCurrent_mA();
  //Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");


  return true;
}

bool readADC() {
  UBAT = readChannel(ADS1115_COMP_0_GND);
  USOLAR = readChannel(ADS1115_COMP_1_GND);
  SENSITIVITY = readChannel(ADS1115_COMP_2_GND);
  return true;
}

float readChannel(ADS1115_MUX channel) {
  float voltage = 0.0;
  adc.setCompareChannels(channel);
  voltage = adc.getResult_V();  // alternative: getResult_mV for Millivolt
  return voltage;
}
