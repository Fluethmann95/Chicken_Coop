bool steuerunghoch()
{
  if (!relaison)
  {
    currentmillis = millis();
    relaison = true;
    EndstopEin();
    digitalWrite(CNTRL_PH1, HIGH);
    digitalWrite(CNTRL_PH2, LOW);
    setStateLED(100, 100, 0);
    digitalWrite(PH_ON, HIGH);
  }
  return true;
}

bool steuerungrunter()
{
  if (!relaison)
  {
    currentmillis = millis();
    relaison = true;
    EndstopEin();
    digitalWrite(CNTRL_PH1, LOW);
    digitalWrite(CNTRL_PH2, HIGH);
    setStateLED(0, 100, 100);
    digitalWrite(PH_ON, HIGH);
  }
  return true;
}

bool RelaisOff (unsigned long duration)
{


  duration = duration * 100;
  if (relaison &&  ((millis() - currentmillis) > duration))
  {
    digitalWrite(CNTRL_PH1, LOW);
    digitalWrite(CNTRL_PH2, LOW);
    digitalWrite(PH_ON, LOW);
    EndstopAus();
    StateLEDoff();
    relaison = false;
    //setStateLED(0, 0, 20);
  }
  return true;
}

bool HandRelaisOff ()
{
  if(relaison && handbetrieb)
  {
    digitalWrite(CNTRL_PH1, LOW);
    digitalWrite(CNTRL_PH2, LOW);
    digitalWrite(PH_ON, LOW);
    EndstopAus();
    StateLEDoff();
    relaison = false;
    //setStateLED(0, 0, 20);
  }
  return true;
}

bool EndstopEin()
{
  digitalWrite(VCC_CTRL, HIGH);

  return true;
}

bool EndstopAus()
{
  digitalWrite(VCC_CTRL, LOW);

  return true;
}

bool AlarmOff(float maxCurrent)
{
  if (current_mA > maxCurrent)
  {
    setFaultLED(255, 0, 0);
    RelaisOff(0);
  }

  if (NOTAUSstate)
  {
    setFaultLED(255, 255, 0);
    RelaisOff(0);
  }
  if (FallingEdge)
  {
    RelaisOff(0);
  }
  return true;
}

bool DAC_SET(int voltage) //mV
{
  MCP.setValue(voltage);
  return true;
}
