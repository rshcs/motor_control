
volatile boolean pinstate = 0;
uint32_t t0; // to calc frequency
uint32_t tmrp; // serial print tmr
void setup()
{
    Serial.begin(9600);
    attachInterrupt(0, state_change, RISING);
    t0 = micros();
    tmrp = micros();
}

void loop()
{
   sprinter(1000);
    
}

void state_change()
{
    pinstate = 1;
}

uint16_t tperiod()
{
    uint32_t t1 = micros() - t0;
    t0 = micros();
    /*
    if(t1 > 1000)
    {
        t1 = 0;
    }
    */
    return t1;
}

uint32_t f() //frequency
{
    return 1000/ tperiod();
}

void sprinter(uint16_t indly)
{
    if(millis() - tmrp > indly)
    {
        Serial.println(tperiod());
        tmrp = millis();
    }
}