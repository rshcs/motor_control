#define DATA_POINTS 10

volatile boolean intstate = 0;
uint32_t t0; // to calc frequency
uint32_t t1;
uint32_t tmrp; // serial print tmr
//String instr = "";
int8_t motor_pin = 6;
int16_t pwm_out = 0;

float kp = 0;
float ki = 0;
float kd = 0;
int setvalue = 0;
int16_t error, errorlast;
int32_t errorinc;
uint32_t tmrc;
// Filter variables
int16_t data[DATA_POINTS];
int16_t avg;
int32_t total = 0;
int16_t indx = 0;

void setup()
{
    Serial.begin(9600);
    attachInterrupt(0, state_change, RISING);
    pinMode(motor_pin, OUTPUT);
    t0 = micros();
    tmrp = millis();
    tmrc = millis();
}

void loop()
{
    tperiod();
    /*
    if(millis() - tmrp > 1000)
    {
        Serial.print(setvalue);
        Serial.print("|");
        Serial.print(f());
        Serial.print("|");
        Serial.print(kp);
        Serial.print("|");
        Serial.print(ki, 6);
        Serial.print("|");
        Serial.print(kd);
        Serial.print("|");
        Serial.print(pid_out());
        Serial.println();

        tmrp = millis();
    }
    */
   
    if(millis() - tmrc > 10)
    {
        assignvalues();
        pwm_out = pid_out();
        analogWrite(motor_pin, pwm_out);
        tmrc = millis();
    }
    
    if(millis() - tmrp > 100)
    {
        Serial.print(setvalue);
        Serial.print(",");
        Serial.print(f()); //<===
        Serial.println();
       
        tmrp = millis();
    }
    
}

int16_t filter()
{
    total = total - data[indx];
    data[indx] = f();
    total = total + data[indx];
    indx++;
    if(indx >= DATA_POINTS)
    {
        indx = 0;
    }    
    return (total / DATA_POINTS);
    /*
    Serial.print(analogRead(A0));
    Serial.print(",");
    Serial.print(avg);
    Serial.println();
    delay(100);
    */
}

int16_t vc_read(int16_t inv)
{
    inv = constrain(inv, 20, 980);
    inv = map(inv, 20, 980, 0, 100);
    return inv;
}

void assignvalues()
{
    setvalue = 14 * vc_read(analogRead(A3));
    kp =  ((float)vc_read(analogRead(A2))) / 50;
    ki = ((float)vc_read(analogRead(A1))) / 1000;
    kd = ((float)vc_read(analogRead(A0))) / 100;
}

void state_change()
{
    intstate = 1;

}

uint32_t tperiod()
{
    //uint32_t t1;
    if (intstate)
    {
        intstate = 0;
        t1 = micros() - t0;
        t0 = micros();
    }
    if (micros() - t0 > 100000)
    {
        t1 = 4294967295; 
    }
    return t1;
    

}

uint16_t f() //frequency
{
    return 1000000 / t1;
}

float pout()
{
    error = setvalue - f();
    return kp * error;
}

float iout()
{
    
    if(ki != 0)
    {
        if(pwm_out < 0 && error < 0){}
        else if(pwm_out > 255 && error > 0){}
        else {errorinc += error;}
        float outv = ki * errorinc;
        //if(outv < 0)
            //outv = 0;
        return outv;
    }
    else
    {
        errorinc = 0;
    }
    
    return 0;
}

float dout()
{
    float outv = kd * (error - errorlast);
    errorlast = error;
    return outv;
}

int16_t pid_out()
{
    float outv = pout() + iout() + dout() + .5;
    if(outv > 255)
        outv = 255;
    else if(outv < 0)
    {
        outv = 0;
    }
    return (int16_t)outv;
}