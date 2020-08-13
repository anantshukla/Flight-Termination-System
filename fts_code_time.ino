#include <Servo.h>
#include <time.h>

Servo Roll;
Servo Pitch;
Servo Throttle;
Servo Yaw;
Servo ppmfltmode;
unsigned long tstart;
unsigned long tnow;
unsigned long diff;
 
void setup()
{
  Serial.begin(9600);
  pinMode(10,INPUT); //Master Switch For FTS
  pinMode(11,INPUT); //FTS MODE
  pinMode(9,INPUT); //Arkbird Flight mode input for the detection of RC loss
  pinMode(6,OUTPUT);//Common SELECT line for MUXs
  pinMode(7,OUTPUT);//Common SELECT line for MUXs
  Roll.attach(2);
  Pitch.attach(3);
  Throttle.attach(4);
  Yaw.attach(5);
  ppmfltmode.attach(8);
}

void loop()
{   
    int master = pulseIn(10,HIGH); //ON, OFF, ch-aux1
    int FTSmode = pulseIn(11,HIGH);  //Pix, Man, FTS ch-aux2
    int fltmode = pulseIn(9,HIGH);
    int thr = pulseIn(12, HIGH);
    ppmfltmode.writeMicroseconds(fltmode);

    Serial.println(thr);
    
    if(master <= 1100)      //Pixhawk output
    {
      digitalWrite(6,LOW);
      digitalWrite(7,LOW);
      Serial.println("Pixhawk O/P");
    }

    
    else
    {
      if(thr<1000)    //WHEN RC LOSS
      {
         tstart = millis();
         while(thr<1000)
         {
          thr = pulseIn(12, HIGH);
          tnow = millis();
          diff = (tnow-tstart);

          Serial.print("THR<1000   ");
          Serial.print(thr);
          Serial.print("   ");
          Serial.print(diff);
  
          if((diff>30000) && (diff<180000))
          {
            ppmfltmode.writeMicroseconds(1150);
            Serial.print("   Doing RTH rn ");              
          }
  
          if(diff>180000)
          {
            digitalWrite(6,HIGH); 
            digitalWrite(7,LOW);
            Roll.writeMicroseconds(1950); //aleron
            Pitch.writeMicroseconds(1050); //rudder
            Throttle.writeMicroseconds(1100); //elevator
            Yaw.writeMicroseconds(1950); //throttle
            Serial.print("   Doing FTS rn");
          }
          Serial.println();
        }
      }


    else              //Thr>1000
    {
      if(FTSmode<=1100)                            //FLY on Pixhawk
        {
          digitalWrite(6,LOW);
          digitalWrite(7,LOW);
          Serial.println("Pixhawk");

        }
        else if((FTSmode>=1300) && (FTSmode<=1700))  //FLY ON RC
        {
          digitalWrite(6,LOW);
          digitalWrite(7,HIGH);
          Serial.println("RC");
        }
        else if(FTSmode>1900)             //DO FTS MANUALLY
        {
          digitalWrite(6,HIGH);
          digitalWrite(7,LOW);
          Roll.writeMicroseconds(1950);
          Pitch.writeMicroseconds(1050);
          Throttle.writeMicroseconds(1100);
          Yaw.writeMicroseconds(1950);
          Serial.print("  Doing Manual FTS rn");
        }
    }
  }
}
