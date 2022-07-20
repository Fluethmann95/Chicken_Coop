bool PIN_INIT()
{
  //OUTPUT
  pinMode(HGB_ON, OUTPUT);
  pinMode(PH_ON, OUTPUT);
  pinMode(CNTRL_PH1, OUTPUT);
  pinMode(CNTRL_PH2, OUTPUT);
  pinMode(VCC_CTRL, OUTPUT);

  //INPUT
  pinMode(ENDSTOP_TOP, INPUT);
  pinMode(ENDSTOP_BOTTOM, INPUT);
  pinMode(LDR, INPUT);
  pinMode(RTC_INT, INPUT);
  pinMode(NOTAUS, INPUT);
  pinMode(USER_BTN, INPUT_PULLUP);
  pinMode(ENCODER_BTN, INPUT_PULLUP);

  return true;
}

bool SERIAL_INIT()
{
  Serial.begin(115200);
  Serial.println("Hühnerklappe");
  delay(300);

  return true;
}

bool DISPLAY_INIT()
{
  DOG.initialize(LCD_CS, SPI_MOSI, SPI_SCK, LCD_CD, LCD_RST, DOGS102);
  DOG.clear();
  DOG.contrast(0x05);
  DOG.all_pixel_on(true);
  delay(700);
  //DOG.inverse(true);
  DOG.all_pixel_on(false);
  String file = __FILE__;
  String fileshort = file.substring(file.lastIndexOf('_') + 1);
  Serial.println(fileshort);
  DOG.string(0, 7 , UBUNTUMONO_B_8, "f.fluethmann" , ALIGN_CENTER);
  DOG.string(0, 2 , UBUNTUMONO_B_8, "Version:" , ALIGN_CENTER);
  DOG.string(0, 4, UBUNTUMONO_B_8, __DATE__, ALIGN_CENTER);
  DOG.string(0, 5, UBUNTUMONO_B_8, __TIME__, ALIGN_CENTER);
  delay(2000);
  return true;
}

bool RTC_INIT()
{
  if (Serial)
  {
    String Stunde;
    String Minute;
    String Sekunde;

    Stunde = CompileTime.substring(CompileTime.indexOf(':'), 0);
    Minute = CompileTime.substring(CompileTime.lastIndexOf(':'), CompileTime.indexOf(':') + 1);
    Sekunde = CompileTime.substring(CompileTime.lastIndexOf(':') + 1);
    Serial.print("Compile Time: ");
    Serial.print(Stunde + ":");
    Serial.print(Minute + ":");
    Serial.println(Sekunde);


    CompileHour = Stunde.toInt();
    CompileMinute = Minute.toInt();
    CompileSecond = Sekunde.toInt();

    Uhr.setHour(CompileHour);
    Uhr.setMinute(CompileMinute);
    Uhr.setSecond(CompileSecond);
  }

  return true;
}
bool I2C_INIT()
{
  Wire.begin();
  return true;
}

bool INA_INIT()
{
  if (! ina219.begin()) {
    Serial.println("Failed to find INA219 chip");
    while (1) {
      delay(10);
    }
  }
  return true;
}

bool ADC_INIT()
{
  if (!adc.init()) {
    Serial.println("ADC ADS1115 not connected!");
  }

  adc.setVoltageRange_mV(ADS1115_RANGE_6144); //comment line/change parameter to change range
  adc.setCompareChannels(ADS1115_COMP_0_GND); //comment line/change parameter to change channel
  adc.setMeasureMode(ADS1115_CONTINUOUS); //comment line/change parameter to change mode

  return true;
}

bool DAC_INIT()
{
  if (!  MCP.begin())
  {
    Serial.println("DAC MCP4725 not connected!");
  }
  else
  {
    Serial.println(MCP4725_VERSION);
  }
  return true;
}

bool EEPROM_INIT()
{
  AT24Cxx eep(i2c_address, EEPROM_SIZE); // Initilaize using AT24CXX(i2c_address, size of eeprom in KB).
  return true;
}

bool FlashRead()
{
  Flashdaten = Flashspeicher.read();
  Serial.println("Flash Gelesen!");
}
