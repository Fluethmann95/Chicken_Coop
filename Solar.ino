//bool DAC_SET(int voltage) //mV
//USOLAR (float)
//temperatur (kalibriert in °C

bool Solar()
{
  int voltageset = 0;
  if (temperatur > 0)
  {
    voltageset = (12.85 - (0.0175 * temperatur)) / 0.008175;
  }
  else
  {
    voltageset = 12.85 / 0.008175;
  }
  DAC_SET(voltageset);
   //Serial.println(voltageset);

   return true;
}
